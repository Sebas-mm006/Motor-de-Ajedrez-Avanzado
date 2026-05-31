#include "./../include/NullPiece.hpp"
#include <cctype>

NullPiece::NullPiece() : Piece(PieceType::NONE, PieceColor::NONE) {}

bool NullPiece::isNull() const {
    return true;
}

char NullPiece::toChar() const {
    return ' ';
}

std::unique_ptr<Piece> NullPiece::clone() const {
    return std::make_unique<NullPiece>();
}

void NullPiece::generateMoves(int row, int col, const class Board& board, std::vector<struct Move>& moves) const {
    // Null piece no genera movimientos (los parámetros están marcados como [[maybe_unused]] implícitamente)
    (void)row; (void)col; (void)board; (void)moves;
}