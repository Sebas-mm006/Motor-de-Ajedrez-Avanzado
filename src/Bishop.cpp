#include "./../include/Bishop.hpp"
#include "./../include/Board.hpp"
#include <cctype>

/**
 * @brief Constructor del alfil
 * @param color Color del alfil (blanco o negro)
 */
Bishop::Bishop(PieceColor color) : Piece(PieceType::BISHOP, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool Bishop::isNull() const {
    return false;
}

/**
 * @brief Convierte el alfil a representación ASCII
 */
char Bishop::toChar() const {
    char pieceChar = 'B';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia del alfil
 */
std::unique_ptr<Piece> Bishop::clone() const {
    return std::make_unique<Bishop>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales del alfil
 * El alfil se mueve diagonalmente cualquier número de casillas
 */
void Bishop::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // Las 4 direcciones diagonales
    const std::array<std::pair<int,int>, 4> dirs = {{{1,1},{1,-1},{-1,1},{-1,-1}}};
    for (const auto& d : dirs) {
        int r = row + d.first;
        int c = col + d.second;
        // Continuar mientras esté dentro del tablero
        while (board.isSquareInside(r, c)) {
            const Piece& target = board.getPiece(r, c);
            if (target.isNull()) {
                // Casilla vacía, movimiento válido
                moves.emplace_back(row, col, r, c);
            } else {
                // Hay una pieza - capturar solo si es del color opuesto
                if (target.getColor() != color) {
                    moves.emplace_back(row, col, r, c);
                }
                break;  // Detenerse al encontrar cualquier pieza
            }
            r += d.first;
            c += d.second;
        }
    }
}