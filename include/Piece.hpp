#ifndef PIECE_HPP
#define PIECE_HPP

#include <iostream>
#include <memory>
#include <vector>

// Forward declarations
class Board;
struct Move;

enum class PieceType {
    PAWN,      // Peón
    KNIGHT,    // Caballo
    BISHOP,    // Alfil
    ROOK,      // Torre
    QUEEN,     // Reina
    KING,      // Rey
    NONE       // Ninguna pieza
};

enum class PieceColor {
    WHITE,     // Blanco
    BLACK,     // Negro
    NONE       // Ningún color
};

/**
 * @brief Clase base abstracta para representar una pieza de ajedrez
 */
class Piece {
public:
    Piece();                                    // Constructor por defecto
    Piece(PieceType type, PieceColor color);    // Constructor con tipo y color
    virtual ~Piece() = default;                 // Destructor virtual
    
    // Getters
    PieceType getType() const;                  // Obtiene el tipo de pieza
    PieceColor getColor() const;                // Obtiene el color de la pieza
    
    // Setters
    void setType(PieceType type);               // Establece el tipo de pieza
    void setColor(PieceColor color);            // Establece el color de la pieza
    
    // Métodos virtuales puramente (interfaz)
    virtual bool isNull() const = 0;                        // Verifica si es una pieza nula
    virtual char toChar() const = 0;                        // Convierte a representación ASCII
    virtual std::unique_ptr<Piece> clone() const = 0;       // Clona la pieza
    virtual void generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const = 0; // Genera movimientos
    
    /**
     * @brief Obtiene la representación ASCII de una pieza dado su tipo y color
     * @param type Tipo de pieza
     * @param color Color de la pieza
     * @return Carácter que representa la pieza (mayúscula para blanco, minúscula para negro)
     */
    static char pieceToChar(PieceType type, PieceColor color);

protected:
    PieceType type_;                            // Tipo de pieza
    PieceColor color_;                          // Color de la pieza
};

#endif // PIECE_HPP