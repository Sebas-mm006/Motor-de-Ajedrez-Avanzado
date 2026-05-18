#include "./../include/Search.hpp"
#include <algorithm>
#include <limits>

const int INF = 1000000;

Search::Search() {}

Move Search::searchBestMove(const Board& board, int depth) {
    PieceColor side = board.getTurn();
    std::vector<Move> moves = board.generateLegalMoves(side);
    if (moves.empty()) {
        return Move();
    }
    moves = orderMoves(moves, board);
    Move bestMove;
    int bestValue = -INF;
    int alpha = -INF;
    int beta = INF;
    for (const Move& m : moves) {
        Board next = board;
        next.make_move(m);
        int value = -negamax(next, depth-1, -beta, -alpha);
        if (value > bestValue) {
            bestValue = value;
            bestMove = m;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            break;
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

int Search::negamax(Board board, int depth, int alpha, int beta) {
    if (depth == 0) {
        int eval = eval_.evaluate(board);
        if (board.getTurn() == PieceColor::WHITE) {
            return eval;
        } else {
            return -eval;
        }
    }
    PieceColor side = board.getTurn();
    if (board.is_mate(side)) {
        return -INF + depth;
    }
    if (!board.is_in_check(side) && board.generateLegalMoves(side).empty()) {
        return 0;
    }
    std::vector<Move> moves = board.generateLegalMoves(side);
    moves = orderMoves(moves, board);
    int value = -INF;
    for (const Move& m : moves) {
        Board next = board;
        next.make_move(m);
        int nextValue = -negamax(next, depth-1, -beta, -alpha);
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

std::vector<Move> Search::orderMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(moves.size());
    
    for (const Move& m : moves) {
        int score = 0;
        Piece target = board.getPiece(m.to_row, m.to_col);
        
        if (m.type == MoveType::CASTLING) {
            score = 1000;
        } else if (m.type == MoveType::EN_PASSANT) {
            score = 400;
        } else if (!target.isNull()) {
            int victimValue = 0;
            switch (target.getType()) {
                case PieceType::PAWN: victimValue = 100; break;
                case PieceType::KNIGHT: victimValue = 320; break;
                case PieceType::BISHOP: victimValue = 330; break;
                case PieceType::ROOK: victimValue = 500; break;
                case PieceType::QUEEN: victimValue = 900; break;
                default: victimValue = 0; break;
            }
            score = 1000 + victimValue;
        } else if (m.type == MoveType::PROMOTION) {
            score = 900;
        }
        scored.emplace_back(score, m);
    }
    
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