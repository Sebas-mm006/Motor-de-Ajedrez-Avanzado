#include "./../include/King.hpp"
#include "./../include/Board.hpp"
#include <cctype>

/**
 * @brief Constructor del rey
 * @param color Color del rey (blanco o negro)
 */
King::King(PieceColor color) : Piece(PieceType::KING, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool King::isNull() const {
    return false;
}

/**
 * @brief Convierte el rey a representación ASCII
 */
char King::toChar() const {
    char pieceChar = 'K';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia del rey
 */
std::unique_ptr<Piece> King::clone() const {
    return std::make_unique<King>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales del rey
 * El rey se mueve una casilla en cualquier dirección
 */
void King::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // Las 8 direcciones posibles de movimiento del rey
    const std::array<std::pair<int,int>, 8> dirs = {{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};
    for (const auto& d : dirs) {
        int r = row + d.first;
        int c = col + d.second;
        if (board.isSquareInside(r, c)) {
            const Piece& target = board.getPiece(r, c);
            // Puede mover a casilla vacía o capturar pieza enemiga
            if (target.isNull() || target.getColor() != color) {
                moves.emplace_back(row, col, r, c);
            }
        }
    }
    // El enroque será manejado en Board::generatePieceMoves
}