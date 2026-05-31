#include "./../include/Search.hpp"
#include <algorithm>
#include <limits>


const int INF = 1000000;

/**
 * @brief Constructor por defecto de la clase Search
 */
Search::Search() {}

/**
 * @brief Busca el mejor movimiento usando un algoritmo de búsqueda recursiva
 * @param board Posición actual del tablero
 * @param depth Profundidad de búsqueda
 * @return Mejor movimiento encontrado
 */
Move Search::searchBestMove(const Board& board, int depth) {
    PieceColor side = board.getTurn();
    std::vector<Move> moves = board.generateLegalMoves(side);
    if (moves.empty()) {
        return Move();  // No hay movimientos legales
    }
    // Ordenar los movimientos para mejorar la eficiencia de la búsqueda
    moves = orderMoves(moves, board);
    Move bestMove;
    int bestValue = -INF;
    int alpha = -INF;
    int beta = INF;
    for (const Move& m : moves) {
        Board next = board;
        next.make_move_unchecked(m);  // Usar move sin validación para mayor eficiencia
        int value = -searchRecursive(next, depth-1, -beta, -alpha);
        if (value > bestValue) {
            bestValue = value;
            bestMove = m;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            break;  // Corte: no necesitamos explorar más
        }
    }
    return bestMove;
}

Move Search::iterativeDeepening(const Board& board, int maxDepth) {
    Move bestMove;
    for (int depth = 1; depth <= maxDepth; ++depth) {
        Move move = searchBestMove(board, depth);
        if (move.isNull()) {
            break;
        }
        bestMove = move;
    }
    return bestMove;
}

/**
 * @brief Algoritmo recursivo de búsqueda
 * @param board Posición actual del tablero
 * @param depth Profundidad restante de búsqueda
 * @param alpha Límite inferior
 * @param beta Límite superior
 * @return Evaluación de la posición desde la perspectiva del jugador que tiene el turno
 */
int Search::searchRecursive(Board board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return eval_.evaluate(board);  // Evaluar posición terminal
    }
    PieceColor side = board.getTurn();
    if (board.is_mate(side)) {
        return -INF + depth;  // Jaque mate - valor negativo alto
    }
    if (!board.is_in_check(side) && board.generateLegalMoves(side).empty()) {
        return 0;  // Empate (ahogado)
    }
    std::vector<Move> moves = board.generateLegalMoves(side);
    moves = orderMoves(moves, board);
    int value = -INF;
    for (const Move& m : moves) {
        // Usar make_move_unchecked para evitar validación redundante
        Board next = board;
        next.make_move_unchecked(m);
        int nextValue = -searchRecursive(next, depth-1, -beta, -alpha);
        if (nextValue > value) {
            value = nextValue;
        }
        if (value > alpha) {
            alpha = value;
        }
        if (alpha >= beta) {
            break;
        }
    }
    return value;
}

/**
 * @brief Ordena los movimientos priorizando capturas
 * (mejora la eficiencia de la búsqueda)
 * @param moves Vector de movimientos a ordenar
 * @param board Posición actual del tablero
 * @return Vector de movimientos ordenados por relevancia
 */
std::vector<Move> Search::orderMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(moves.size());
    
    for (const Move& m : moves) {
        int score = 0;
        const Piece& target = board.getPiece(m.to_row, m.to_col);
        
        if (m.type == MoveType::CASTLING) {
            score = 1000;  // Enroque es muy prioritario
        } else if (m.type == MoveType::EN_PASSANT) {
            score = 400;  // Captura al paso
        } else if (!target.isNull()) {
            // Captura de pieza - valor según tipo de pieza capturada
            int victimValue = 0;
            switch (target.getType()) {
                case PieceType::PAWN: victimValue = 100; break;
                case PieceType::KNIGHT: victimValue = 320; break;
                case PieceType::BISHOP: victimValue = 330; break;
                case PieceType::ROOK: victimValue = 500; break;
                case PieceType::QUEEN: victimValue = 900; break;
                default: victimValue = 0; break;
            }
            score = 1000 + victimValue;  // Priorizar capturas de piezas valiosas
        } else if (m.type == MoveType::PROMOTION) {
            score = 900;  // Promoción es muy valiosa
        }
        scored.emplace_back(score, m);
    }
    
    // Ordenar de mayor a menor puntuación (descendente)
    std::sort(scored.begin(), scored.end(), 
              [](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
                  return a.first > b.first;
              });
    
    std::vector<Move> ordered;
    ordered.reserve(moves.size());
    for (const auto& p : scored) {
        ordered.push_back(p.second);
    }
    return ordered;
}