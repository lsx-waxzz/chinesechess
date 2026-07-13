#include "ChessViewModel.h"
#include <map>

ChessViewModel::ChessViewModel(QObject* parent)
    : QObject(parent)
{
    rebuildSnapshots();
}

// ===== 命令实现 =====

void ChessViewModel::onCellClicked(int row, int col) {
    // 中期不实现 — 仅预留接口
    Q_UNUSED(row);
    Q_UNUSED(col);
}

void ChessViewModel::resetGame() {
    engine_.resetGame();
    rebuildSnapshots();
    notifyIfChanged();
}

void ChessViewModel::setGameMode(int mode, int difficulty) {
    gameMode_ = static_cast<GameMode>(mode);
    aiDifficulty_ = static_cast<AIDifficulty>(difficulty);
    engine_.resetGame();
    rebuildSnapshots();
    notifyIfChanged();
}

void ChessViewModel::undoLastMove() {
    // 中期不实现
    emit undoRejected(QStringLiteral("悔棋功能暂未实现"));
}

// ===== 快照重建 =====

void ChessViewModel::rebuildSnapshots() {
    // 保存旧快照用于比较
    prevBoardSnapshot_ = boardSnapshot_;
    prevGameSnapshot_ = gameSnapshot_;

    // 重建棋盘快照
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

    // 重建游戏状态快照
    gameSnapshot_ = GameSnapshot();
    gameSnapshot_.currentPlayer =
        (engine_.currentPlayer() == PieceColor::Red) ? "红方" : "黑方";
    gameSnapshot_.gameOver = engine_.isGameOver();
    if (gameSnapshot_.gameOver) {
        gameSnapshot_.statusMessage = "游戏结束！";
    }
}

void ChessViewModel::notifyIfChanged() {
    // 比较快照，只有真正变化才发通知
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

    if (boardDiff)
        emit boardChanged();
    if (gameDiff)
        emit gameStateChanged();
}

// ===== 工具方法 =====

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
