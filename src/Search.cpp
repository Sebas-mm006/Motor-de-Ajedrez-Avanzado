#include "./../include/Search.hpp"
#include <algorithm>
#include <limits>

const int INF = 1000000;

Search::Search() {}

Move Search::searchBestMove(const Board& board, int depth) {
    // We'll use negamax to search for the best move.
    // The side to move is board.getTurn()
    PieceColor side = board.getTurn();
    std::vector<Move> moves = board.generateLegalMoves(side);
    if (moves.empty()) {
        return Move(); // null move (no legal moves)
    }
    // Order moves to improve pruning
    moves = orderMoves(moves, board);
    Move bestMove;
    int bestValue = -INF;
    int alpha = -INF;
    int beta = INF;
    for (const Move& m : moves) {
        Board next = board;
        next.make_move(m);
        // The value of the next position from the opponent's perspective is negamax(next, depth-1, -beta, -alpha)
        // Then our value is the negative of that.
        int value = -negamax(next, depth-1, -beta, -alpha);
        if (value > bestValue) {
            bestValue = value;
            bestMove = m;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            break; // beta cutoff
        }
    }
    return bestMove;
}

int Search::negamax(Board board, int depth, int alpha, int beta) {
    if (depth == 0) {
        // Evaluate the position from the perspective of the side to move.
        int eval = eval_.evaluate(board); // white-positive
        if (board.getTurn() == PieceColor::WHITE) {
            return eval;
        } else {
            return -eval;
        }
    }
    // Check for game over
    PieceColor side = board.getTurn();
    if (board.is_mate(side)) {
        // The side to move is checkmated -> lose
        return -INF + depth; // slightly prefer sooner mates? Actually we return a large negative.
        // We'll just return -INF for simplicity.
    }
    if (!board.is_in_check(side) && board.generateLegalMoves(side).empty()) {
        // Stalemate
        return 0;
    }
    std::vector<Move> moves = board.generateLegalMoves(side);
    moves = orderMoves(moves, board);
    int value = -INF;
    for (const Move& m : moves) {
        Board next = board;
        next.make_move(m);
        // Recursion: the value of the position after the move from the opponent's perspective is negamax(next, depth-1, -beta, -alpha)
        // Then our value is the negative of that.
        int nextValue = -negamax(next, depth-1, -beta, -alpha);
        if (nextValue > value) {
            value = nextValue;
        }
        if (value > alpha) {
            alpha = value;
        }
        if (alpha >= beta) {
            break; // beta cutoff
        }
    }
    return value;
}

std::vector<Move> Search::orderMoves(const std::vector<Move>& moves, const Board& board) const {
    // Simple ordering: captures first, then others.
    // We'll separate captures and non-captures.
    std::vector<Move> captures;
    std::vector<Move> nonCaptures;
    for (const Move& m : moves) {
        Piece captured = board.getPiece(m.to_row, m.to_col);
        if (!captured.isNull()) {
            captures.push_back(m);
        } else {
            nonCaptures.push_back(m);
        }
    }
    // Combine: captures first
    std::vector<Move> ordered;
    ordered.reserve(moves.size());
    ordered.insert(ordered.end(), captures.begin(), captures.end());
    ordered.insert(ordered.end(), nonCaptures.begin(), nonCaptures.end());
    return ordered;
}
