#include "ChessViewModel.h"
#include <map>

ChessViewModel::ChessViewModel(QObject* parent)
    : QObject(parent)
{
    rebuildSnapshots();
}

void ChessViewModel::clearHighlight() {
    hasSelection_ = false;
    selectedRow_ = selectedCol_ = -1;
    validMoves_.clear();
    emit highlightChanged();
}

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

    // ---------- 走子成功 ----------
    emit moveMade();   // 触发音效

    undoUsed_ = false;
    clearHighlight();

    rebuildSnapshots();
    notifyIfChanged();

    if (engine_.isGameOver()) {
        PieceColor winner = (engine_.currentPlayer() == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
        QString winnerStr = (winner == PieceColor::Red) ? "红方" : "黑方";
        emit statusMessage(winnerStr + QStringLiteral("获胜！"));
    }
}

void ChessViewModel::resetGame() {
    engine_.resetGame();
    undoUsed_ = false;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();
    emit statusMessage(QStringLiteral("游戏已重置"));
}

void ChessViewModel::setGameMode(int mode, int difficulty) {
    gameMode_ = static_cast<GameMode>(mode);
    aiDifficulty_ = static_cast<AIDifficulty>(difficulty);
    engine_.resetGame();
    undoUsed_ = false;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();
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
    if (!engine_.undoLastMove()) {
        emit statusMessage(QStringLiteral("没有可悔的棋"));
        return;
    }
    undoUsed_ = true;
    clearHighlight();
    rebuildSnapshots();
    notifyIfChanged();
}

// ---------- 快照重建与通知 ----------
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
    }
    else {
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
            }
            else if (cur.has_value()) {
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
