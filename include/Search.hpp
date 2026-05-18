#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "Board.hpp"
#include "Eval.hpp"
#include <vector>
#include <limits>

/**
 * @brief Implementa el algoritmo de búsqueda Minimax con poda alfa-beta (usando Negamax)
 * para encontrar el mejor movimiento en una posición dada.
 */
class Search {
public:
    Search();                                        // Constructor por defecto
    
    /**
     * @brief Busca el mejor movimiento usando Negamax con poda alfa-beta
     * @param board Posición actual del tablero
     * @param depth Profundidad máxima de búsqueda
     * @return Mejor movimiento para el jugador que tiene el turno
     */
    Move searchBestMove(const Board& board, int depth);
    
    /**
     * @brief Búsqueda iterativa con profundidad progresiva
     * @param board Posición actual del tablero
     * @param maxDepth Profundidad máxima de búsqueda
     * @return Mejor movimiento encontrado
     */
    Move iterativeDeepening(const Board& board, int maxDepth);
    
private:
    /**
     * @brief Algoritmo Negamax con poda alfa-beta
     * @param board Posición actual del tablero
     * @param depth Profundidad restante de búsqueda
     * @param alpha Límite inferior de la poda
     * @param beta Límite superior de la poda
     * @return Evaluación de la posición desde la perspectiva del jugador que tiene el turno
     */
    int negamax(Board board, int depth, int alpha, int beta);
    
    /**
     * @brief Ordena los movimientos para mejorar la eficiencia de la poda alfa-beta
     * (prioriza capturas primero)
     * @param moves Vector de movimientos a ordenar
     * @param board Posición actual del tablero
     * @return Vector de movimientos ordenados
     */
    std::vector<Move> orderMoves(const std::vector<Move>& moves, const Board& board) const;
    
    Eval eval_;                                      // Función de evaluación de posiciones
};

#endif // SEARCH_HPP