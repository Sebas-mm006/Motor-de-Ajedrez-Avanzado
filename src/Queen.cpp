#include "./../include/Queen.hpp"
#include "./../include/Board.hpp"
#include <cctype>

/**
 * @brief Constructor de la reina
 * @param color Color de la reina (blanco o negro)
 */
Queen::Queen(PieceColor color) : Piece(PieceType::QUEEN, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool Queen::isNull() const {
    return false;
}

/**
 * @brief Convierte la reina a representación ASCII
 */
char Queen::toChar() const {
    char pieceChar = 'Q';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia de la reina
 */
std::unique_ptr<Piece> Queen::clone() const {
    return std::make_unique<Queen>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales de la reina
 * La reina combina movimientos de torre y alfil
 */
void Queen::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // Las 8 direcciones posibles: todo el tablero menos caballo y rey
    const std::array<std::pair<int,int>, 8> dirs = {{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};
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