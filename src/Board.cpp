#include "./../include/Board.hpp"
#include <algorithm>
#include <cctype>

Board::Board() : turn_(PieceColor::WHITE), lastCapture_() {
    // Initialize empty board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board_[i][j] = Piece();
        }
    }
}

void Board::initStartPosition() {
    // Clear board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board_[i][j] = Piece();
        }
    }
    // Set up pawns
    for (int col = 0; col < BOARD_SIZE; ++col) {
        board_[1][col] = Piece(PieceType::PAWN, PieceColor::BLACK);
        board_[6][col] = Piece(PieceType::PAWN, PieceColor::WHITE);
    }
    // Set up other pieces
    std::array<PieceType, 8> backRank = {
        PieceType::ROOK, PieceType::KNIGHT, PieceType::BISHOP,
        PieceType::QUEEN, PieceType::KING, PieceType::BISHOP,
        PieceType::KNIGHT, PieceType::ROOK
    };
    for (int col = 0; col < BOARD_SIZE; ++col) {
        board_[0][col] = Piece(backRank[col], PieceColor::BLACK);
        board_[7][col] = Piece(backRank[col], PieceColor::WHITE);
    }
    turn_ = PieceColor::WHITE; // white to move at start
}

Piece Board::getPiece(int row, int col) const {
    if (!isSquareInside(row, col)) {
        return Piece(); // Return null piece if out of bounds
    }
    return board_[row][col];
}

void Board::setPiece(int row, int col, const Piece& piece) {
    if (isSquareInside(row, col)) {
        board_[row][col] = piece;
    }
}

bool Board::isSquareInside(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

PieceColor Board::getTurn() const {
    return turn_;
}

void Board::make_move(const Move& move) {
    // Basic validation: ensure source square has a piece
    Piece movingPiece = getPiece(move.from_row, move.from_col);
    if (movingPiece.isNull()) {
        // Should not happen if caller uses generateLegalMoves
        return;
    }
    // Capture the piece at target square (if any) for undo
    lastCapture_ = getPiece(move.to_row, move.to_col);
    // Move the piece
    setPiece(move.to_row, move.to_col, movingPiece);
    setPiece(move.from_row, move.from_col, Piece());
    // Handle promotion (if promoted_piece is not null, replace)
    if (!move.promoted_piece.isNull()) {
        setPiece(move.to_row, move.to_col, move.promoted_piece);
    }
    // Switch turn
    turn_ = (turn_ == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
}

void Board::undo_move(const Move& move) {
    // Undo: move piece back from to to from, restore captured piece
    Piece current = getPiece(move.to_row, move.to_col);
    setPiece(move.from_row, move.from_col, current);
    setPiece(move.to_row, move.to_col, lastCapture_);
    // Switch turn back
    turn_ = (turn_ == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    // Note: we ignore promotion undo for simplicity.
}

std::vector<Move> Board::generateLegalMoves(PieceColor color) const {
    std::vector<Move> legalMoves;
    // For each square, if piece belongs to color, generate pseudo-legal moves
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            if (!p.isNull() && p.getColor() == color) {
                generatePieceMoves(row, col, legalMoves);
            }
        }
    }
    // Filter moves that would leave king in check
    std::vector<Move> filtered;
    for (const Move& m : legalMoves) {
        if (!wouldMoveBeInCheck(m, color)) {
            filtered.push_back(m);
        }
    }
    return filtered;
}

bool Board::is_mate(PieceColor color) const {
    // Checkmate: color is in check and has no legal moves
    if (!is_in_check(color)) {
        return false;
    }
    std::vector<Move> moves = generateLegalMoves(color);
    return moves.empty();
}

bool Board::is_in_check(PieceColor color) const {
    auto kingPos = findKing(color);
    if (kingPos.first == -1) {
        return false; // No king? should not happen
    }
    PieceColor opponent = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    // Check if any opponent piece can attack king's square
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            if (!p.isNull() && p.getColor() == opponent) {
                // Generate pseudo-legal moves for this piece; if any move can capture king, then in check
                std::vector<Move> pseudo;
                generatePieceMoves(row, col, pseudo);
                for (const Move& m : pseudo) {
                    if (m.to_row == kingPos.first && m.to_col == kingPos.second) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::pair<int, int> Board::findKing(PieceColor color) const {
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            if (!p.isNull() && p.getType() == PieceType::KING && p.getColor() == color) {
                return {row, col};
            }
        }
    }
    return {-1, -1}; // Not found
}

void Board::printBoard() const {
    std::cout << "  a b c d e f g h\n";
    for (int row = 0; row < BOARD_SIZE; ++row) {
        std::cout << 8 - row << " ";
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            char c = p.isNull() ? '.' : p.toChar();
            std::cout << c << ' ';
        }
        std::cout << 8 - row << '\n';
    }
    std::cout << "  a b c d e f g h\n";
}

// Helper to generate pseudo-legal moves for a piece at (row,col)
// This is a simplistic implementation; we'll implement per piece type later.
void Board::generatePieceMoves(int row, int col, std::vector<Move>& moves) const {
    Piece p = getPiece(row, col);
    if (p.isNull()) return;
    PieceType type = p.getType();
    PieceColor color = p.getColor();
    // Directions for sliding pieces
    const std::array<std::pair<int,int>, 8> dirs = {{
        {1,0}, {-1,0}, {0,1}, {0,-1}, // orthogonal
        {1,1}, {1,-1}, {-1,1}, {-1,-1} // diagonals
    }};
    switch (type) {
        case PieceType::PAWN: {
            int direction = (color == PieceColor::WHITE) ? -1 : 1; // white moves up (decreasing row)
            int startRow = (color == PieceColor::WHITE) ? 6 : 1;
            // One step forward
            int oneRow = row + direction;
            if (isSquareInside(oneRow, col) && getPiece(oneRow, col).isNull()) {
                moves.emplace_back(row, col, oneRow, col);
                // Two steps from start
                int twoRow = row + 2*direction;
                if (row == startRow && isSquareInside(twoRow, col) && getPiece(twoRow, col).isNull()) {
                    moves.emplace_back(row, col, twoRow, col);
                }
            }
            // Captures
            for (int dc : {-1, 1}) {
                int capRow = row + direction;
                int capCol = col + dc;
                if (isSquareInside(capRow, capCol)) {
                    Piece target = getPiece(capRow, capCol);
                    if (!target.isNull() && target.getColor() != color) {
                        moves.emplace_back(row, col, capRow, capCol);
                    }
                    // En passant could be added later
                }
            }
            break;
        }
        case PieceType::KNIGHT: {
            const std::array<std::pair<int,int>, 8> knightMoves = {{
                {2,1}, {2,-1}, {-2,1}, {-2,-1},
                {1,2}, {1,-2}, {-1,2}, {-1,-2}
            }};
            for (const auto& offset : knightMoves) {
                int toRow = row + offset.first;
                int toCol = col + offset.second;
                if (isSquareInside(toRow, toCol)) {
                    Piece target = getPiece(toRow, toCol);
                    if (target.isNull() || target.getColor() != color) {
                        moves.emplace_back(row, col, toRow, toCol);
                    }
                }
            }
            break;
        }
        case PieceType::BISHOP: {
            // Diagonals
            const std::array<std::pair<int,int>, 4> dirs = {{{1,1},{1,-1},{-1,1},{-1,-1}}};
            for (const auto& d : dirs) {
                int r = row + d.first;
                int c = col + d.second;
                while (isSquareInside(r, c)) {
                    Piece target = getPiece(r, c);
                    if (target.isNull()) {
                        moves.emplace_back(row, col, r, c);
                    } else {
                        if (target.getColor() != color) {
                            moves.emplace_back(row, col, r, c);
                        }
                        break;
                    }
                    r += d.first;
                    c += d.second;
                }
            }
            break;
        }
        case PieceType::ROOK: {
            const std::array<std::pair<int,int>, 4> dirs = {{{1,0},{-1,0},{0,1},{0,-1}}};
            for (const auto& d : dirs) {
                int r = row + d.first;
                int c = col + d.second;
                while (isSquareInside(r, c)) {
                    Piece target = getPiece(r, c);
                    if (target.isNull()) {
                        moves.emplace_back(row, col, r, c);
                    } else {
                        if (target.getColor() != color) {
                            moves.emplace_back(row, col, r, c);
                        }
                        break;
                    }
                    r += d.first;
                    c += d.second;
                }
            }
            break;
        }
        case PieceType::QUEEN: {
            // Combine rook and bishop directions
            const std::array<std::pair<int,int>, 8> dirs = {{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};
            for (const auto& d : dirs) {
                int r = row + d.first;
                int c = col + d.second;
                while (isSquareInside(r, c)) {
                    Piece target = getPiece(r, c);
                    if (target.isNull()) {
                        moves.emplace_back(row, col, r, c);
                    } else {
                        if (target.getColor() != color) {
                            moves.emplace_back(row, col, r, c);
                        }
                        break;
                    }
                    r += d.first;
                    c += d.second;
                }
            }
            break;
        }
        case PieceType::KING: {
            const std::array<std::pair<int,int>, 8> dirs = {{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};
            for (const auto& d : dirs) {
                int r = row + d.first;
                int c = col + d.second;
                if (isSquareInside(r, c)) {
                    Piece target = getPiece(r, c);
                    if (target.isNull() || target.getColor() != color) {
                        moves.emplace_back(row, col, r, c);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

// Check if making this move would leave the king of the same color in check
bool Board::wouldMoveBeInCheck(const Move& move, PieceColor color) const {
    // Make a copy of the board (using copy constructor)
    Board temp = *this;
    // Make the move on temp
    temp.make_move(move);
    // Now check if the side that moved is in check
    return temp.is_in_check(color);
}
