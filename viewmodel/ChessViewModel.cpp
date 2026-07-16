#include "ChessViewModel.h"
#include "../model/UciEngine.h"
#include <map>
#include <QTimer>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

ChessViewModel::ChessViewModel(QObject* parent)
    : QObject(parent)
{
    rebuildSnapshots();
}

ChessViewModel::~ChessViewModel() {
    if (uciEngine_) {
        uciEngine_->stop();
        delete uciEngine_;
    }
}

// ── 引擎管理（皮卡鱼优先，内置 AI 回退） ──

void ChessViewModel::tryStartEngine() {
    // 查找引擎文件
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/pikafish-bmi2.exe",
        QCoreApplication::applicationDirPath() + "/pikafish-avx2.exe",
        QCoreApplication::applicationDirPath() + "/pikafish-sse41-popcnt.exe",
        QCoreApplication::applicationDirPath() + "/pikafish-vnni512.exe",
        QCoreApplication::applicationDirPath() + "/pikafish-avxvnni.exe",
        QCoreApplication::applicationDirPath() + "/pikafish-avx512.exe",
        QCoreApplication::applicationDirPath() + "/pikafish.exe",
        QCoreApplication::applicationDirPath() + "/engine/pikafish-bmi2.exe",
        "D:/midchess/midchess/code/pikafish-bmi2.exe",
    };

    QString enginePath;
    for (const auto& path : searchPaths) {
        if (QFile::exists(path)) {
            enginePath = path;
            break;
        }
    }

    if (enginePath.isEmpty()) {
        // 没找到外部引擎，使用内置 AI
        useExternalEngine_ = false;
        qDebug() << "未找到皮卡鱼引擎，使用内置 AI";
        return;
    }

    if (!uciEngine_) {
        uciEngine_ = new UciEngine(this);
        connect(uciEngine_, &UciEngine::started,
                this, &ChessViewModel::onEngineStarted);
        connect(uciEngine_, &UciEngine::bestMove,
                this, &ChessViewModel::onEngineBestMove);
        connect(uciEngine_, &UciEngine::error,
                this, &ChessViewModel::onEngineError);
    }

    uciEngine_->start(enginePath);
}

void ChessViewModel::onEngineStarted() {
    useExternalEngine_ = true;
    qDebug() << "皮卡鱼引擎就绪";
}

void ChessViewModel::onEngineBestMove(const QString& uciMove) {
    qDebug() << "onEngineBestMove:" << uciMove;
    Move aiMove = UciEngine::uciToMove(uciMove);
    qDebug() << "  converted to:" << aiMove.fromRow << aiMove.fromCol
             << "->" << aiMove.toRow << aiMove.toCol;
    applyAIMove(aiMove);
}

void ChessViewModel::onEngineError(const QString& msg) {
    qDebug() << "引擎错误:" << msg << "，回退到内置AI";
    useExternalEngine_ = false;
}

// ── 统一的 AI 走子应用 ──

void ChessViewModel::applyAIMove(const Move& aiMove) {
    aiComputing_ = false;

    qDebug() << "applyAIMove: fromRow=" << aiMove.fromRow
             << "gameOver=" << engine_.isGameOver()
             << "currentPlayer=" << (engine_.currentPlayer() == PieceColor::Black ? "Black" : "Red");

    if (aiMove.fromRow == -1) { qDebug() << "  FAIL: invalid move"; return; }
    if (engine_.isGameOver()) { qDebug() << "  FAIL: game over"; return; }
    if (engine_.currentPlayer() != PieceColor::Black) { qDebug() << "  FAIL: not Black's turn"; return; }

    bool success = engine_.makeMove(aiMove);
    qDebug() << "  makeMove success:" << success;
    if (!success) return;

    emit moveMade();
    undoUsed_ = false;
    clearHighlight();

    rebuildSnapshots();
    notifyIfChanged();

    if (engine_.isGameOver()) {
        PieceColor winner = (engine_.currentPlayer() == PieceColor::Red)
            ? PieceColor::Black : PieceColor::Red;
        QString winnerStr = (winner == PieceColor::Red) ? "红方" : "黑方";
        emit statusMessage(winnerStr + QStringLiteral("获胜！"));
    }
}

// ── 高亮 ──

void ChessViewModel::clearHighlight() {
    hasSelection_ = false;
    selectedRow_ = selectedCol_ = -1;
    validMoves_.clear();
    emit highlightChanged();
}

// ── 点击处理 ──

void ChessViewModel::onCellClicked(int row, int col) {
    if (engine_.isGameOver()) {
        emit statusMessage(QStringLiteral("游戏已结束，请重新开始"));
        return;
    }

    if (gameMode_ == GameMode::PlayerVsAI && engine_.currentPlayer() == PieceColor::Black) {
        emit statusMessage(QStringLiteral("电脑思考中..."));
        return;
    }

    const auto& piece = engine_.board().pieceAt(row, col);
    PieceColor current = engine_.currentPlayer();

    if (!hasSelection_) {
        if (piece && piece->color == current) {
            selectedRow_ = row;
            selectedCol_ = col;
            hasSelection_ = true;
            validMoves_ = engine_.getValidMoves(row, col);
            emit highlightChanged();
        }
        return;
    }

    if (selectedRow_ == row && selectedCol_ == col) {
        clearHighlight();
        return;
    }

    if (piece && piece->color == current) {
        selectedRow_ = row;
        selectedCol_ = col;
        validMoves_ = engine_.getValidMoves(row, col);
        emit highlightChanged();
        return;
    }

    bool isLegal = false;
    for (auto [tr, tc] : validMoves_) {
        if (tr == row && tc == col) {
            isLegal = true;
            break;
        }
    }
    if (!isLegal) {
        clearHighlight();
        return;
    }

    Move move{ selectedRow_, selectedCol_, row, col };
    bool success = engine_.makeMove(move);
    if (!success) {
        emit statusMessage(QStringLiteral("走子失败"));
        clearHighlight();
        return;
    }

    emit moveMade();
    undoUsed_ = false;
    clearHighlight();

    rebuildSnapshots();
    notifyIfChanged();

    if (engine_.isGameOver()) {
        PieceColor winner = (engine_.currentPlayer() == PieceColor::Red)
            ? PieceColor::Black : PieceColor::Red;
        QString winnerStr = (winner == PieceColor::Red) ? "红方" : "黑方";
        emit statusMessage(winnerStr + QStringLiteral("获胜！"));
        return;
    }

    if (gameMode_ == GameMode::PlayerVsAI && engine_.currentPlayer() == PieceColor::Black) {
        if (aiComputing_) return;
        aiComputing_ = true;
        // 延迟走子，让 UI 先刷新
        QTimer::singleShot(150, this, [this]() { executeAIMove(); });
    }
}

void ChessViewModel::executeAIMove() {
    qDebug() << "executeAIMove: gameOver=" << engine_.isGameOver()
             << "currentPlayer=" << (engine_.currentPlayer() == PieceColor::Black ? "Black" : "Red")
             << "useExternalEngine=" << useExternalEngine_
             << "engineReady=" << (uciEngine_ ? uciEngine_->isReady() : false);

    if (engine_.isGameOver()) {
        aiComputing_ = false;
        return;
    }
    if (engine_.currentPlayer() != PieceColor::Black) {
        aiComputing_ = false;
        return;
    }

    if (useExternalEngine_ && uciEngine_ && uciEngine_->isReady()) {
        // ── 外部引擎 ──
        int timeMs;
        switch (aiDifficulty_) {
        case 0: timeMs = 500;  break;
        case 1: timeMs = 1500; break;
        case 2: timeMs = 3000; break;
        default: timeMs = 1000; break;
        }
        QString fen = UciEngine::boardToFen(engine_);
        qDebug() << "AI: sending to external engine, fen:" << fen;
        uciEngine_->search(fen, timeMs);

        // 安全兜底：如果外部引擎超时未响应，5 秒后强制用内置 AI
        QTimer::singleShot(timeMs + 2000, this, [this]() {
            if (aiComputing_) {
                qDebug() << "外部引擎超时，回退内置 AI";
                aiComputing_ = false;
                Move aiMove = AIPlayer::getBestMove(engine_, AIDifficulty::Easy);
                applyAIMove(aiMove);
            }
        });
    } else {
        // ── 内置 AI（限用 Easy，保证速度） ──
        qDebug() << "AI: using built-in AI";
        Move aiMove = AIPlayer::getBestMove(engine_, AIDifficulty::Easy);
        applyAIMove(aiMove);
    }
}

// ── 游戏控制 ──

void ChessViewModel::resetGame() {
    aiComputing_ = false;
    engine_.resetGame();
    undoUsed_ = false;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();
    emit statusMessage(QStringLiteral("游戏已重置"));
}

void ChessViewModel::setGameMode(int mode, int difficulty) {
    aiComputing_ = false;
    gameMode_ = static_cast<GameMode>(mode);
    aiDifficulty_ = difficulty;
    engine_.resetGame();
    undoUsed_ = false;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();

    if (gameMode_ == GameMode::PlayerVsAI) {
        tryStartEngine();
    }
}

void ChessViewModel::undoLastMove() {
    if (engine_.isGameOver()) {
        emit statusMessage(QStringLiteral("游戏已结束，不能悔棋"));
        return;
    }
    if (undoUsed_) {
        emit statusMessage(QStringLiteral("只能悔一步棋"));
        return;
    }

    if (gameMode_ == GameMode::PlayerVsAI) {
        if (!engine_.undoLastMove()) {
            emit statusMessage(QStringLiteral("没有可悔的棋"));
            return;
        }
        if (!engine_.undoLastMove()) {
            emit statusMessage(QStringLiteral("没有可悔的棋"));
            return;
        }
    } else {
        if (!engine_.undoLastMove()) {
            emit statusMessage(QStringLiteral("没有可悔的棋"));
            return;
        }
    }

    undoUsed_ = true;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();
}

// ── 快照 ──

void ChessViewModel::rebuildSnapshots() {
    prevBoardSnapshot_ = boardSnapshot_;
    prevGameSnapshot_ = gameSnapshot_;

    boardSnapshot_ = BoardSnapshot();
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            const auto& piece = engine_.board().pieceAt(r, c);
            if (piece) {
                PieceSnapshot snap;
                snap.text = pieceToChinese(piece).toStdString();
                snap.isRed = (piece->color == PieceColor::Red);
                boardSnapshot_.cells[r][c] = snap;
            }
        }
    }

    gameSnapshot_ = GameSnapshot();
    if (engine_.isGameOver()) {
        gameSnapshot_.gameOver = true;
        gameSnapshot_.statusMessage = "游戏结束";
        gameSnapshot_.currentPlayer = "";
    } else {
        gameSnapshot_.currentPlayer =
            (engine_.currentPlayer() == PieceColor::Red) ? "红方" : "黑方";
        gameSnapshot_.gameOver = false;
        gameSnapshot_.statusMessage = "";
    }
}

void ChessViewModel::notifyIfChanged() {
    bool boardDiff = false;
    for (int r = 0; r < 10 && !boardDiff; ++r)
        for (int c = 0; c < 9 && !boardDiff; ++c) {
            const auto& cur = boardSnapshot_.cells[r][c];
            const auto& prev = prevBoardSnapshot_.cells[r][c];
            if (cur.has_value() != prev.has_value()) {
                boardDiff = true;
            } else if (cur.has_value()) {
                if (cur->text != prev->text || cur->isRed != prev->isRed)
                    boardDiff = true;
            }
        }

    bool gameDiff =
        (gameSnapshot_.currentPlayer != prevGameSnapshot_.currentPlayer) ||
        (gameSnapshot_.gameOver != prevGameSnapshot_.gameOver);

    if (boardDiff) emit boardChanged();
    if (gameDiff) emit gameStateChanged();
}

QString ChessViewModel::pieceToChinese(const std::optional<Piece>& p) {
    if (!p) return QString();

    static const std::map<std::pair<PieceColor, PieceType>, QString> map{
        {{PieceColor::Red, PieceType::King},    QStringLiteral("帅")},
        {{PieceColor::Red, PieceType::Advisor},  QStringLiteral("仕")},
        {{PieceColor::Red, PieceType::Elephant}, QStringLiteral("相")},
        {{PieceColor::Red, PieceType::Knight},   QStringLiteral("馬")},
        {{PieceColor::Red, PieceType::Rook},     QStringLiteral("車")},
        {{PieceColor::Red, PieceType::Cannon},   QStringLiteral("炮")},
        {{PieceColor::Red, PieceType::Pawn},     QStringLiteral("兵")},
        {{PieceColor::Black, PieceType::King},    QStringLiteral("将")},
        {{PieceColor::Black, PieceType::Advisor},  QStringLiteral("士")},
        {{PieceColor::Black, PieceType::Elephant}, QStringLiteral("象")},
        {{PieceColor::Black, PieceType::Knight},   QStringLiteral("马")},
        {{PieceColor::Black, PieceType::Rook},     QStringLiteral("车")},
        {{PieceColor::Black, PieceType::Cannon},   QStringLiteral("砲")},
        {{PieceColor::Black, PieceType::Pawn},     QStringLiteral("卒")}
    };
    auto it = map.find({ p->color, p->type });
    return (it != map.end()) ? it->second : QStringLiteral("?");
}
