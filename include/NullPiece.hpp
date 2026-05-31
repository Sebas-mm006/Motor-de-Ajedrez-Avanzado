#ifndef NULLPIECE_HPP
#define NULLPIECE_HPP

#include "Piece.hpp"

/**
 * @brief Representa una pieza nula (casilla vacía)
 */
class NullPiece : public Piece {
public:
    NullPiece();
    ~NullPiece() override = default;
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;
    char toChar() const override;
    std::unique_ptr<Piece> clone() const override;
    void generateMoves(int row, int col, const class Board& board, std::vector<struct Move>& moves) const override;
};

#endif // NULLPIECE_HPP