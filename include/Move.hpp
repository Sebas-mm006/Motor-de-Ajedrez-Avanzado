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
    Piece promoted_piece;    // Pieza a la que se promociona un peón (si aplica)
    MoveType type;           // Tipo de movimiento
    Piece captured_piece;    // Pieza capturada (si aplica)

    /**
     * @brief Constructor por defecto (crea movimiento nulo)
     */
    Move() : from_row(-1), from_col(-1), to_row(-1), to_col(-1), 
             promoted_piece(), type(MoveType::NORMAL), captured_piece() {}

    /**
     * @brief Constructor con parámetros
     * @param fr Fila de origen
     * @param fc Columna de origen
     * @param tr Fila de destino
     * @param tc Columna de destino
     * @param promo Pieza de promoción (por defecto, pieza nula)
     */
    Move(int fr, int fc, int tr, int tc, Piece promo = Piece(), MoveType mt = MoveType::NORMAL) 
        : from_row(fr), from_col(fc), to_row(tr), to_col(tc), 
          promoted_piece(promo), type(mt), captured_piece() {}

    /**
     * @brief Verifica si el movimiento es nulo
     * @return true si es nulo, false en caso contrario
     */
    bool isNull() const {
        return from_row == -1 && from_col == -1 && to_row == -1 && to_col == -1;
    }

    /**
     * @brief Sobrecarga del operador de salida para imprimir el movimiento
     * @param os Flujo de salida
     * @param m Movimiento a imprimir
     * @return Referencia al flujo de salida
     */
    friend std::ostream& operator<<(std::ostream& os, const Move& m) {
        if (m.isNull()) {
            os << "movimiento nulo";
            return os;
        }
        char from_file = 'a' + m.from_col;
        char from_rank = '1' + m.from_row;
        char to_file = 'a' + m.to_col;
        char to_rank = '1' + m.to_row;
        os << from_file << from_rank << to_file << to_rank;
        if (!m.promoted_piece.isNull()) {
            os << m.promoted_piece.toChar();
        }
        return os;
    }
};

#endif // MOVE_HPP