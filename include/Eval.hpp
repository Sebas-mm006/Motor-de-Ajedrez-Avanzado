#ifndef EVAL_HPP
#define EVAL_HPP

#include "Board.hpp"

/**
 * @brief Evalúa la posición del tablero usando una función heurística
 */
class Eval {
public:
    Eval();                                        // Constructor por defecto
    
    /**
     * @brief Evalúa el tablero desde la perspectiva de las piezas blancas
     * @param board Tablero a evaluar
     * @return Puntuación en centipawns (positivo favorece a blanco, negativo a negro)
     */
    int evaluate(const Board& board) const;
    
private:
    // Valores estándar de las piezas en centipawns
    static constexpr int pawnValue = 100;      // Peón
    static constexpr int knightValue = 320;    // Caballo
    static constexpr int bishopValue = 330;    // Alfil
    static constexpr int rookValue = 500;      // Torre
    static constexpr int queenValue = 900;     // Reina
    static constexpr int kingValue = 20000;    // Rey (valor alto para seguridad)
    
    // Tablas de piezas-casilla (simplificadas, valores en centipawns)
    static const int pawnTable[8][8];      // Bonificaciones/penalizaciones para peones
    static const int knightTable[8][8];    // Para caballos
    static const int bishopTable[8][8];    // Para alfiles
    static const int rookTable[8][8];      // Para torres
    static const int queenTable[8][8];     // Para reinas
    static const int kingTable[8][8];      // Para reyes
    
    /**
     * @brief Obtiene el valor material de un tipo de pieza
     * @param type Tipo de pieza
     * @return Valor en centipawns
     */
    int pieceValue(PieceType type) const;
    
    /**
     * @brief Obtiene el bonus posicional de una pieza en una casilla específica
     * @param type Tipo de pieza
     * @param color Color de la pieza
     * @param row Fila (0-7)
     * @param col Columna (0-7)
     * @return Bonus posicional en centipawns
     */
    int pieceSquareBonus(PieceType type, PieceColor color, int row, int col) const;
};

#endif // EVAL_HPP