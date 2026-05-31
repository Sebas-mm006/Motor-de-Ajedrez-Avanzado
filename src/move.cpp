#include "Move.hpp"
#include "Piece.hpp"

Move::Move() : from_row(-1), from_col(-1), to_row(-1), to_col(-1), 
               promoted_piece_type(PieceType::NONE), promoted_piece_color(PieceColor::NONE),
               type(MoveType::NORMAL), captured_piece_type(PieceType::NONE), captured_piece_color(PieceColor::NONE) {}

Move::Move(int fr, int fc, int tr, int tc, PieceType promo_type, PieceColor promo_color, MoveType mt) 
    : from_row(fr), from_col(fc), to_row(tr), to_col(tc), 
      promoted_piece_type(promo_type), promoted_piece_color(promo_color), type(mt), 
      captured_piece_type(PieceType::NONE), captured_piece_color(PieceColor::NONE) {}

bool Move::isNull() const {
    return from_row == -1 && from_col == -1 && to_row == -1 && to_col == -1;
}

std::ostream& operator<<(std::ostream& os, const Move& m) {
    if (m.isNull()) {
        os << "movimiento nulo";
        return os;
    }
    char from_file = 'a' + m.from_col;
    char from_rank = '8' - m.from_row;
    char to_file = 'a' + m.to_col;
    char to_rank = '8' - m.to_row;
    os << from_file << from_rank << to_file << to_rank;
    if (m.promoted_piece_type != PieceType::NONE) {
        os << Piece::pieceToChar(m.promoted_piece_type, m.promoted_piece_color);
    }
    return os;
}