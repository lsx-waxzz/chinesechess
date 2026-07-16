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

    // 黑方（上方）
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

    // 红方（下方）
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

bool ChessBoard::hasPieceAt(int row, int col) const {
    return inBoard(row, col) && board[row][col].has_value();
}

bool ChessBoard::isSameColor(int row, int col, PieceColor color) const {
    if (!inBoard(row, col) || !board[row][col].has_value()) return false;
    return board[row][col]->color == color;
}

void ChessBoard::addMoveIfValid(int fromRow, int fromCol, int toRow, int toCol,
    std::vector<std::pair<int, int>>& moves) const {
    if (!inBoard(toRow, toCol)) return;
    const auto& target = board[toRow][toCol];
    if (target.has_value() && target->color == board[fromRow][fromCol]->color)
        return; // 不能吃己方棋子
    moves.push_back({ toRow, toCol });
}

void ChessBoard::addLineMoves(int row, int col, int dr, int dc,
    std::vector<std::pair<int, int>>& moves) const {
    int r = row + dr, c = col + dc;
    while (inBoard(r, c)) {
        if (board[r][c].has_value()) {
            if (board[r][c]->color != board[row][col]->color)
                moves.push_back({ r, c }); // 吃子
            break; // 遇到棋子停止（不论颜色）
        }
        moves.push_back({ r, c });
        r += dr; c += dc;
    }
}

std::vector<std::pair<int, int>> ChessBoard::rawMoves(int row, int col) const {
    std::vector<std::pair<int, int>> moves;
    if (!inBoard(row, col) || !board[row][col].has_value()) return moves;

    const Piece& piece = *board[row][col];
    PieceColor color = piece.color;
    PieceType type = piece.type;

    switch (type) {
    case PieceType::King: {
        // 九宫格内上下左右一步
        const std::vector<std::pair<int, int>> dirs = { {-1,0},{1,0},{0,-1},{0,1} };
        for (auto [dr, dc] : dirs) {
            int nr = row + dr, nc = col + dc;
            if (nr < 0 || nr > 9 || nc < 0 || nc > 8) continue;
            // 九宫格限制
            if (color == PieceColor::Red) {
                if (nr < 7 || nr > 9 || nc < 3 || nc > 5) continue;
            }
            else { // Black
                if (nr < 0 || nr > 2 || nc < 3 || nc > 5) continue;
            }
            addMoveIfValid(row, col, nr, nc, moves);
        }
        break;
    }
    case PieceType::Advisor: {
        // 九宫格斜走一步
        const std::vector<std::pair<int, int>> dirs = { {-1,-1},{-1,1},{1,-1},{1,1} };
        for (auto [dr, dc] : dirs) {
            int nr = row + dr, nc = col + dc;
            if (color == PieceColor::Red) {
                if (nr < 7 || nr > 9 || nc < 3 || nc > 5) continue;
            }
            else {
                if (nr < 0 || nr > 2 || nc < 3 || nc > 5) continue;
            }
            addMoveIfValid(row, col, nr, nc, moves);
        }
        break;
    }
    case PieceType::Elephant: {
        // 田字，不能过河
        const std::vector<std::pair<int, int>> dirs = { {-2,-2},{-2,2},{2,-2},{2,2} };
        for (auto [dr, dc] : dirs) {
            int nr = row + dr, nc = col + dc;
            if (!inBoard(nr, nc)) continue;
            // 不能过河
            if (color == PieceColor::Red && nr < 5) continue;
            if (color == PieceColor::Black && nr > 4) continue;
            // 象眼（田字中心）不能有棋子
            int blockerR = row + dr / 2, blockerC = col + dc / 2;
            if (board[blockerR][blockerC].has_value()) continue;
            addMoveIfValid(row, col, nr, nc, moves);
        }
        break;
    }
    case PieceType::Knight: {
        // 日字，蹩马腿
        const std::vector<std::pair<int, int>> jumps = {
            {-2,-1}, {-2,1}, {-1,-2}, {-1,2},
            {1,-2}, {1,2}, {2,-1}, {2,1}
        };
        // 对应马腿偏移 (dr/2, dc/2) 注意整除
        const std::vector<std::pair<int, int>> legs = {
            {-1,0}, {-1,0}, {0,-1}, {0,1},
            {0,-1}, {0,1}, {1,0}, {1,0}
        };
        for (size_t i = 0; i < jumps.size(); ++i) {
            int nr = row + jumps[i].first;
            int nc = col + jumps[i].second;
            if (!inBoard(nr, nc)) continue;
            int lr = row + legs[i].first;
            int lc = col + legs[i].second;
            if (board[lr][lc].has_value()) continue; // 蹩马腿
            addMoveIfValid(row, col, nr, nc, moves);
        }
        break;
    }
    case PieceType::Rook: {
        // 直线任意步，不能越过棋子
        const std::vector<std::pair<int, int>> dirs = { {-1,0},{1,0},{0,-1},{0,1} };
        for (auto [dr, dc] : dirs) {
            addLineMoves(row, col, dr, dc, moves);
        }
        break;
    }
    case PieceType::Cannon: {
        // 走法：直线，吃子必须隔一个棋子（炮架）
        const std::vector<std::pair<int, int>> dirs = { {-1,0},{1,0},{0,-1},{0,1} };
        for (auto [dr, dc] : dirs) {
            int r = row + dr, c = col + dc;
            bool foundScreen = false;
            while (inBoard(r, c)) {
                if (board[r][c].has_value()) {
                    if (!foundScreen) {
                        foundScreen = true; // 找到炮架
                    }
                    else {
                        // 可吃子（必须是对方棋子）
                        if (board[r][c]->color != color)
                            moves.push_back({ r, c });
                        break; // 不管是否吃子，停止
                    }
                }
                else {
                    if (!foundScreen) {
                        moves.push_back({ r, c }); // 空位可走
                    }
                    // 若已找到炮架，空位不能走（炮只能隔一子打）
                }
                r += dr; c += dc;
            }
        }
        break;
    }
    case PieceType::Pawn: {
        // 兵/卒：过河前只能向前，过河后可左右
        int forward = (color == PieceColor::Red) ? -1 : 1;
        // 是否过河
        bool crossed = (color == PieceColor::Red) ? (row <= 4) : (row >= 5);
        // 向前
        int nr = row + forward;
        if (inBoard(nr, col))
            addMoveIfValid(row, col, nr, col, moves);
        // 左右（只有过河后）
        if (crossed) {
            if (col - 1 >= 0) addMoveIfValid(row, col, row, col - 1, moves);
            if (col + 1 <= 8) addMoveIfValid(row, col, row, col + 1, moves);
        }
        break;
    }
    }
    return moves;
}
