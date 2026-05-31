#ifndef KING_HPP
#define KING_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <vector>

class Board; // Forward declaration

/**
 * @brief Representa un rey de ajedrez
 */
class King : public Piece {
public:
    King(PieceColor color);
    ~King() override = default;
    
    // Implementación de métodos virtuales puros
    bool isNull() const override;
    char toChar() const override;
    std::unique_ptr<Piece> clone() const override;
    void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const override;
};

#endif // KING_HPP