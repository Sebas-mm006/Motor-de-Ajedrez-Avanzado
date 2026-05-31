#include "./../include/Knight.hpp"
#include "./../include/Board.hpp"
#include <cctype>

/**
 * @brief Constructor del caballo
 * @param color Color del caballo (blanco o negro)
 */
Knight::Knight(PieceColor color) : Piece(PieceType::KNIGHT, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool Knight::isNull() const {
    return false;
}

/**
 * @brief Convierte el caballo a representación ASCII
 */
char Knight::toChar() const {
    char pieceChar = 'N';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia del caballo
 */
std::unique_ptr<Piece> Knight::clone() const {
    return std::make_unique<Knight>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales del caballo
 * El caballo mueve en forma de L: 2 casillas en una dirección y 1 en perpendicular
 */
void Knight::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // Los 8 movimientos posibles del caballo
    const std::array<std::pair<int,int>, 8> knightMoves = {{
        {2,1}, {2,-1}, {-2,1}, {-2,-1},
        {1,2}, {1,-2}, {-1,2}, {-1,-2}
    }};
    for (const auto& offset : knightMoves) {
        int toRow = row + offset.first;
        int toCol = col + offset.second;
        if (board.isSquareInside(toRow, toCol)) {
            const Piece& target = board.getPiece(toRow, toCol);
            // Puede mover a casilla vacía o capturar pieza enemiga
            if (target.isNull() || target.getColor() != color) {
                moves.emplace_back(row, col, toRow, toCol);
            }
        }
    }
}