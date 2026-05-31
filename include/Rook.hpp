#ifndef ROOK_HPP
#define ROOK_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa una torre de ajedrez
 * La torre se mueve horizontal y verticalmente cualquier número de casillas
 */
class Rook : public Piece {
public:
    Rook(PieceColor color);                             // Constructor de la torre
    ~Rook() override = default;                           // Destructor
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;                        // Verifica si es una pieza nula
    char toChar() const override;                        // Convierte a representación ASCII
    std::unique_ptr<Piece> clone() const override;       // Clona la pieza
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override; // Genera movimientos
};

#endif // ROOK_HPP