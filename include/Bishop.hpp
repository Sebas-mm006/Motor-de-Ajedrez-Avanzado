#ifndef BISHOP_HPP
#define BISHOP_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa un alfil de ajedrez
 * El alfil se mueve diagonalmente cualquier número de casillas
 */
class Bishop : public Piece {
public:
    Bishop(PieceColor color);                            // Constructor del alfil
    ~Bishop() override = default;                        // Destructor
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;                        // Verifica si es una pieza nula
    char toChar() const override;                        // Convierte a representación ASCII
    std::unique_ptr<Piece> clone() const override;       // Clona la pieza
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override; // Genera movimientos
};

#endif // BISHOP_HPP