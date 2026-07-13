#include "ChessBoard.h"
#include <cstdlib>

ChessBoard::ChessBoard() : board(10, std::vector<std::optional<Piece>>(9)) {
    resetBoard();
}

const std::optional<Piece>& ChessBoard::pieceAt(int row, int col) const {
    return board[row][col];
}

void ChessBoard::setPiece(int row, int col, const std::optional<Piece>& piece) {
    board[row][col] = piece;
}

void ChessBoard::clearPiece(int row, int col) {
    board[row][col] = std::nullopt;
}

void ChessBoard::resetBoard() {
    for (int r = 0; r < 10; ++r)
        for (int c = 0; c < 9; ++c)
            board[r][c] = std::nullopt;

    // 黑方
    board[0][0] = Piece{ PieceColor::Black, PieceType::Rook };
    board[0][1] = Piece{ PieceColor::Black, PieceType::Knight };
    board[0][2] = Piece{ PieceColor::Black, PieceType::Elephant };
    board[0][3] = Piece{ PieceColor::Black, PieceType::Advisor };
    board[0][4] = Piece{ PieceColor::Black, PieceType::King };
    board[0][5] = Piece{ PieceColor::Black, PieceType::Advisor };
    board[0][6] = Piece{ PieceColor::Black, PieceType::Elephant };
    board[0][7] = Piece{ PieceColor::Black, PieceType::Knight };
    board[0][8] = Piece{ PieceColor::Black, PieceType::Rook };
    board[2][1] = Piece{ PieceColor::Black, PieceType::Cannon };
    board[2][7] = Piece{ PieceColor::Black, PieceType::Cannon };
    for (int c = 0; c <= 8; c += 2)
        board[3][c] = Piece{ PieceColor::Black, PieceType::Pawn };

    // 红方
    board[9][0] = Piece{ PieceColor::Red, PieceType::Rook };
    board[9][1] = Piece{ PieceColor::Red, PieceType::Knight };
    board[9][2] = Piece{ PieceColor::Red, PieceType::Elephant };
    board[9][3] = Piece{ PieceColor::Red, PieceType::Advisor };
    board[9][4] = Piece{ PieceColor::Red, PieceType::King };
    board[9][5] = Piece{ PieceColor::Red, PieceType::Advisor };
    board[9][6] = Piece{ PieceColor::Red, PieceType::Elephant };
    board[9][7] = Piece{ PieceColor::Red, PieceType::Knight };
    board[9][8] = Piece{ PieceColor::Red, PieceType::Rook };
    board[7][1] = Piece{ PieceColor::Red, PieceType::Cannon };
    board[7][7] = Piece{ PieceColor::Red, PieceType::Cannon };
    for (int c = 0; c <= 8; c += 2)
        board[6][c] = Piece{ PieceColor::Red, PieceType::Pawn };
}

bool ChessBoard::inBoard(int r, int c) const {
    return r >= 0 && r < 10 && c >= 0 && c < 9;
}

std::vector<std::pair<int, int>> ChessBoard::rawMoves(int row, int col) const {
    return {};
}
