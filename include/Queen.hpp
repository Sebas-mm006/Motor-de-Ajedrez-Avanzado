#ifndef QUEEN_HPP
#define QUEEN_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa una reina de ajedrez
 * La reina combina movimientos de torre y alfil (toda la fila, columna y diagonales)
 */
class Queen : public Piece {
public:
    Queen(PieceColor color);                             // Constructor de la reina
    ~Queen() override = default;                          // Destructor
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;                        // Verifica si es una pieza nula
    char toChar() const override;                        // Convierte a representación ASCII
    std::unique_ptr<Piece> clone() const override;       // Clona la pieza
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override; // Genera movimientos
};

#endif // QUEEN_HPP