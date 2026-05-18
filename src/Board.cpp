#include "./../include/Board.hpp"
#include <algorithm>
#include <cctype>

Board::Board() : turn_(PieceColor::WHITE), en_passant_sq_(-1) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board_[i][j] = Piece();
        }
    }
    castling_rights_ = CastlingRights();
}

void Board::initStartPosition() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board_[i][j] = Piece();
        }
    }
    for (int col = 0; col < BOARD_SIZE; ++col) {
        board_[1][col] = Piece(PieceType::PAWN, PieceColor::BLACK);
        board_[6][col] = Piece(PieceType::PAWN, PieceColor::WHITE);
    }
    std::array<PieceType, 8> backRank = {
        PieceType::ROOK, PieceType::KNIGHT, PieceType::BISHOP,
        PieceType::QUEEN, PieceType::KING, PieceType::BISHOP,
        PieceType::KNIGHT, PieceType::ROOK
    };
    for (int col = 0; col < BOARD_SIZE; ++col) {
        board_[0][col] = Piece(backRank[col], PieceColor::BLACK);
        board_[7][col] = Piece(backRank[col], PieceColor::WHITE);
    }
    turn_ = PieceColor::WHITE;
    castling_rights_ = CastlingRights();
    en_passant_sq_ = -1;
    history_.clear();
}

Piece Board::getPiece(int row, int col) const {
    if (!isSquareInside(row, col)) {
        return Piece();
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

void Board::updateCastlingRights(const Move& move, const Piece& piece) {
    int from_col = move.from_col;
    int from_row = move.from_row;
    
    if (piece.getType() == PieceType::KING) {
        if (piece.getColor() == PieceColor::WHITE) {
            castling_rights_.white_kingside = false;
            castling_rights_.white_queenside = false;
        } else {
            castling_rights_.black_kingside = false;
            castling_rights_.black_queenside = false;
        }
    } else if (piece.getType() == PieceType::ROOK) {
        if (piece.getColor() == PieceColor::WHITE) {
            if (from_row == 7 && from_col == 7) castling_rights_.white_kingside = false;
            if (from_row == 7 && from_col == 0) castling_rights_.white_queenside = false;
        } else {
            if (from_row == 0 && from_col == 7) castling_rights_.black_kingside = false;
            if (from_row == 0 && from_col == 0) castling_rights_.black_queenside = false;
        }
        Piece target = getPiece(move.to_row, move.to_col);
        if (!target.isNull() && target.getType() == PieceType::ROOK) {
            if (target.getColor() == PieceColor::WHITE) {
                if (move.to_row == 7 && move.to_col == 7) castling_rights_.white_kingside = false;
                if (move.to_row == 7 && move.to_col == 0) castling_rights_.white_queenside = false;
            } else {
                if (move.to_row == 0 && move.to_col == 7) castling_rights_.black_kingside = false;
                if (move.to_row == 0 && move.to_col == 0) castling_rights_.black_queenside = false;
            }
        }
    }
}

void Board::make_move(const Move& move) {
    Piece movingPiece = getPiece(move.from_row, move.from_col);
    if (movingPiece.isNull()) {
        return;
    }
    
    UndoInfo undo;
    undo.moved_piece = movingPiece;
    undo.promoted_type = (move.type == MoveType::PROMOTION) ? move.promoted_piece.getType() : PieceType::NONE;
    undo.castling_rights = castling_rights_;
    undo.en_passant_sq = en_passant_sq_;
    undo.captured_piece = Piece();
    
    if (move.type == MoveType::EN_PASSANT) {
        int capture_row = move.from_row;
        undo.captured_piece = getPiece(capture_row, move.to_col);
        setPiece(capture_row, move.to_col, Piece());
    } else {
        undo.captured_piece = getPiece(move.to_row, move.to_col);
    }
    
    updateCastlingRights(move, movingPiece);
    
    if (move.type == MoveType::CASTLING) {
        int row = move.from_row;
        if (move.to_col > move.from_col) {
            Piece rook = getPiece(row, 7);
            setPiece(row, 5, rook);
            setPiece(row, 7, Piece());
        } else {
            Piece rook = getPiece(row, 0);
            setPiece(row, 3, rook);
            setPiece(row, 0, Piece());
        }
    }
    
    if (move.type == MoveType::EN_PASSANT) {
        int capture_row = move.from_row;
        setPiece(capture_row, move.to_col, Piece());
        setPiece(move.to_row, move.to_col, movingPiece);
    } else {
        setPiece(move.to_row, move.to_col, movingPiece);
    }
    setPiece(move.from_row, move.from_col, Piece());
    
    if (!move.promoted_piece.isNull()) {
        setPiece(move.to_row, move.to_col, move.promoted_piece);
    }
    
    en_passant_sq_ = -1;
    if (movingPiece.getType() == PieceType::PAWN && abs(move.to_row - move.from_row) == 2) {
        en_passant_sq_ = move.from_row * 8 + move.from_col + 8 * (move.to_row > move.from_row ? 1 : -1);
    }
    
    turn_ = (turn_ == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    history_.push_back(undo);
}

void Board::undo_move(const Move& move) {
    if (history_.empty()) return;
    
    UndoInfo undo = history_.back();
    history_.pop_back();
    
    turn_ = (turn_ == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    castling_rights_ = undo.castling_rights;
    en_passant_sq_ = undo.en_passant_sq;
    
    if (move.type == MoveType::CASTLING) {
        int row = move.from_row;
        if (move.to_col > move.from_col) {
            Piece rook = getPiece(row, 5);
            setPiece(row, 7, rook);
            setPiece(row, 5, Piece());
        } else {
            Piece rook = getPiece(row, 3);
            setPiece(row, 0, rook);
            setPiece(row, 3, Piece());
        }
    }
    
    Piece piece = (undo.promoted_type != PieceType::NONE) ? 
        Piece(PieceType::PAWN, turn_) : undo.moved_piece;
    
    setPiece(move.from_row, move.from_col, piece);
    setPiece(move.to_row, move.to_col, undo.captured_piece);
    
    if (move.type == MoveType::EN_PASSANT) {
        int capture_row = (turn_ == PieceColor::WHITE) ? move.from_row + 1 : move.from_row - 1;
        setPiece(capture_row, move.to_col, undo.captured_piece);
        setPiece(move.to_row, move.to_col, Piece());
    }
}

std::vector<Move> Board::generateLegalMoves(PieceColor color) const {
    std::vector<Move> legalMoves;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            if (!p.isNull() && p.getColor() == color) {
                generatePieceMoves(row, col, legalMoves);
            }
        }
    }
    std::vector<Move> filtered;
    for (const Move& m : legalMoves) {
        if (!wouldMoveBeInCheck(m, color)) {
            filtered.push_back(m);
        }
    }
    return filtered;
}

bool Board::is_mate(PieceColor color) const {
    if (!is_in_check(color)) {
        return false;
    }
    std::vector<Move> moves = generateLegalMoves(color);
    return moves.empty();
}

bool Board::is_in_check(PieceColor color) const {
    auto kingPos = findKing(color);
    if (kingPos.first == -1) {
        return false;
    }
    PieceColor opponent = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = getPiece(row, col);
            if (!p.isNull() && p.getColor() == opponent) {
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
    return {-1, -1};
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

void Board::generatePieceMoves(int row, int col, std::vector<Move>& moves) const {
    Piece p = getPiece(row, col);
    if (p.isNull()) return;
    PieceType type = p.getType();
    PieceColor color = p.getColor();
    int direction = (color == PieceColor::WHITE) ? -1 : 1;
    int startRow = (color == PieceColor::WHITE) ? 6 : 1;
    
    switch (type) {
        case PieceType::PAWN: {
            int oneRow = row + direction;
            if (isSquareInside(oneRow, col) && getPiece(oneRow, col).isNull()) {
                if (oneRow == 0 || oneRow == 7) {
                    moves.emplace_back(row, col, oneRow, col, Piece(PieceType::QUEEN, color), MoveType::PROMOTION);
                } else {
                    moves.emplace_back(row, col, oneRow, col);
                }
                int twoRow = row + 2*direction;
                if (row == startRow && isSquareInside(twoRow, col) && getPiece(twoRow, col).isNull()) {
                    moves.emplace_back(row, col, twoRow, col);
                }
            }
            for (int dc : {-1, 1}) {
                int capRow = row + direction;
                int capCol = col + dc;
                if (isSquareInside(capRow, capCol)) {
                    Piece target = getPiece(capRow, capCol);
                    if (!target.isNull() && target.getColor() != color) {
                        if (capRow == 0 || capRow == 7) {
                            moves.emplace_back(row, col, capRow, capCol, Piece(PieceType::QUEEN, color), MoveType::PROMOTION);
                        } else {
                            moves.emplace_back(row, col, capRow, capCol);
                        }
                    } else if (capRow * 8 + capCol == en_passant_sq_) {
                        moves.emplace_back(row, col, capRow, capCol, Piece(), MoveType::EN_PASSANT);
                    }
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
            // Castling
            if (color == PieceColor::WHITE && row == 7) {
                if (castling_rights_.white_kingside && 
                    getPiece(7, 5).isNull() && getPiece(7, 6).isNull() &&
                    !is_in_check(PieceColor::WHITE)) {
                    Board temp = *this;
                    temp.make_move(Move(7, 4, 7, 5));
                    if (!temp.is_in_check(PieceColor::WHITE)) {
                        moves.emplace_back(7, 4, 7, 6, Piece(), MoveType::CASTLING);
                    }
                }
                if (castling_rights_.white_queenside &&
                    getPiece(7, 1).isNull() && getPiece(7, 2).isNull() && getPiece(7, 3).isNull() &&
                    !is_in_check(PieceColor::WHITE)) {
                    Board temp = *this;
                    temp.make_move(Move(7, 4, 7, 3));
                    if (!temp.is_in_check(PieceColor::WHITE)) {
                        moves.emplace_back(7, 4, 7, 2, Piece(), MoveType::CASTLING);
                    }
                }
            } else if (color == PieceColor::BLACK && row == 0) {
                if (castling_rights_.black_kingside &&
                    getPiece(0, 5).isNull() && getPiece(0, 6).isNull() &&
                    !is_in_check(PieceColor::BLACK)) {
                    Board temp = *this;
                    temp.make_move(Move(0, 4, 0, 5));
                    if (!temp.is_in_check(PieceColor::BLACK)) {
                        moves.emplace_back(0, 4, 0, 6, Piece(), MoveType::CASTLING);
                    }
                }
                if (castling_rights_.black_queenside &&
                    getPiece(0, 1).isNull() && getPiece(0, 2).isNull() && getPiece(0, 3).isNull() &&
                    !is_in_check(PieceColor::BLACK)) {
                    Board temp = *this;
                    temp.make_move(Move(0, 4, 0, 3));
                    if (!temp.is_in_check(PieceColor::BLACK)) {
                        moves.emplace_back(0, 4, 0, 2, Piece(), MoveType::CASTLING);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

bool Board::wouldMoveBeInCheck(const Move& move, PieceColor color) const {
    Board temp = *this;
    temp.make_move(move);
    return temp.is_in_check(color);
}