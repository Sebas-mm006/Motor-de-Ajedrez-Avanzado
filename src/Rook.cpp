#include "./../include/Rook.hpp"
#include "./../include/Board.hpp"
#include <cctype>

/**
 * @brief Constructor de la torre
 * @param color Color de la torre (blanco o negro)
 */
Rook::Rook(PieceColor color) : Piece(PieceType::ROOK, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool Rook::isNull() const {
    return false;
}

/**
 * @brief Convierte la torre a representación ASCII
 */
char Rook::toChar() const {
    char pieceChar = 'R';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia de la torre
 */
std::unique_ptr<Piece> Rook::clone() const {
    return std::make_unique<Rook>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales de la torre
 * La torre se mueve horizontal y verticalmente cualquier número de casillas
 */
void Rook::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // Las 4 direcciones rectas: arriba, abajo, derecha, izquierda
    const std::array<std::pair<int,int>, 4> dirs = {{{1,0},{-1,0},{0,1},{0,-1}}};
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