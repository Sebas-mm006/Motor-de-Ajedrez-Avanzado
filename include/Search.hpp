#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "Board.hpp"
#include "Eval.hpp"
#include <vector>
#include <limits>

/**
 * @brief Implementa un algoritmo de búsqueda recursivo
 * para encontrar el mejor movimiento en una posición dada.
 */
class Search {
public:
    Search();                                        // Constructor por defecto
    
    /**
     * @brief Busca el mejor movimiento usando un algoritmo de búsqueda
     * @param board Posición actual del tablero
     * @param depth Profundidad máxima de búsqueda
     * @return Mejor movimiento para el jugador que tiene el turno
     */
    Move searchBestMove(const Board& board, int depth);
    
    /**
     * @brief Busca el mejor movimiento con diferentes profundidades
     * @param board Posición actual del tablero
     * @param maxDepth Profundidad máxima de búsqueda
     * @return Mejor movimiento encontrado
     */
    Move iterativeDeepening(const Board& board, int maxDepth);
    
private:
    /**
     * @brief Algoritmo recursivo de búsqueda
     * @param board Posición actual del tablero
     * @param depth Profundidad restante de búsqueda
     * @param alpha Límite inferior
     * @param beta Límite superior
     * @return Evaluación de la posición desde la perspectiva del jugador que tiene el turno
     */
    int searchRecursive(Board board, int depth, int alpha, int beta);
    
    /**
     * @brief Ordena los movimientos priorizando capturas
     * @param moves Vector de movimientos a ordenar
     * @param board Posición actual del tablero
     * @return Vector de movimientos ordenados
     */
    std::vector<Move> orderMoves(const std::vector<Move>& moves, const Board& board) const;
    
    Eval eval_;                                      // Función de evaluación de posiciones
};

#endif // SEARCH_HPP