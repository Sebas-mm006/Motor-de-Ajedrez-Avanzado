#ifndef KNIGHT_HPP
#define KNIGHT_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa un caballo de ajedrez
 * El caballo es la única pieza que salta sobre otras piezas
 * Se mueve en forma de L: 2 casillas en una dirección y 1 en perpendicular
 */
class Knight : public Piece {
public:
    Knight(PieceColor color);                               // Constructor del caballo
    ~Knight() override = default;                           // Destructor
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;                           // Verifica si es una pieza nula
    char toChar() const override;                           // Convierte a representación ASCII
    std::unique_ptr<Piece> clone() const override;          // Clona la pieza
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override; // Genera movimientos
};

#endif // KNIGHT_HPP