#ifndef MOVE_HPP
#define MOVE_HPP

#include "Piece.hpp"
#include <iostream>

enum class MoveType {
    NORMAL,        // Movimiento normal
    CAPTURE,       // Captura simple
    EN_PASSANT,    // Captura al paso
    CASTLING,      // Enroque
    PROMOTION      // Promoción
};

/**
 * @brief Representa un movimiento en el tablero de ajedrez
 */
struct Move {
    int from_row, from_col;  // Posición de origen (fila, columna)
    int to_row, to_col;      // Posición de destino (fila, columna)
    PieceType promoted_piece_type;    // Tipo de pieza a la que se promociona un peón (si aplica)
    PieceColor promoted_piece_color;  // Color de la pieza a la que se promociona un peón (si aplica)
    MoveType type;           // Tipo de movimiento
    PieceType captured_piece_type;    // Tipo de pieza capturada (si aplica)
    PieceColor captured_piece_color;  // Color de la pieza capturada (si aplica)

    /**
     * @brief Constructor por defecto (crea movimiento nulo)
     */
    Move();

    /**
     * @brief Constructor con parámetros
     * @param fr Fila de origen
     * @param fc Columna de origen
     * @param tr Fila de destino
     * @param tc Columna de destino
     * @param promo_type Tipo de pieza de promoción (por defecto, pieza nula)
     * @param promo_color Color de pieza de promoción (por defecto, pieza nula)
     * @param mt Tipo de movimiento (por defecto, NORMAL)
     */
    Move(int fr, int fc, int tr, int tc, 
         PieceType promo_type = PieceType::NONE, 
         PieceColor promo_color = PieceColor::NONE, 
         MoveType mt = MoveType::NORMAL);

    /**
     * @brief Verifica si el movimiento es nulo
     * @return true si es nulo, false en caso contrario
     */
    bool isNull() const;

    /**
     * @brief Sobrecarga del operador de salida para imprimir el movimiento
     * @param os Flujo de salida
     * @param m Movimiento a imprimir
     * @return Referencia al flujo de salida
     */
    friend std::ostream& operator<<(std::ostream& os, const Move& m);
};

#endif // MOVE_HPP