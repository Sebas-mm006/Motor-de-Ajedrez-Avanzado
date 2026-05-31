#ifndef PAWN_HPP
#define PAWN_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa un peón de ajedrez
 * El peón es la pieza más débil pero con movimientos especiales (doble paso, captura al paso, promoción)
 */
class Pawn : public Piece {
public:
    Pawn(PieceColor color);                              // Constructor del peón
    ~Pawn() override = default;                            // Destructor
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;                        // Verifica si es una pieza nula
    char toChar() const override;                        // Convierte a representación ASCII
    std::unique_ptr<Piece> clone() const override;       // Clona la pieza
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override; // Genera movimientos
};

#endif // PAWN_HPP