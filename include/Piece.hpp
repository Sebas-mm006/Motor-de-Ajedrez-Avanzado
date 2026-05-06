#ifndef PIECE_HPP
#define PIECE_HPP

#include <iostream>

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
 * @brief Representa una pieza de ajedrez
 */
class Piece {
public:
    Piece();                                    // Constructor por defecto
    Piece(PieceType type, PieceColor color);    // Constructor con tipo y color
    
    PieceType getType() const;                  // Obtiene el tipo de pieza
    PieceColor getColor() const;                // Obtiene el color de la pieza
    
    void setType(PieceType type);               // Establece el tipo de pieza
    void setColor(PieceColor color);            // Establece el color de la pieza
    
    bool isNull() const;                        // Verifica si es una pieza nula
    char toChar() const;                        // Convierte a representación ASCII
    
private:
    PieceType type_;                            // Tipo de pieza
    PieceColor color_;                          // Color de la pieza
};

#endif // PIECE_HPP