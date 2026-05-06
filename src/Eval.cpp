#include "./../include/Eval.hpp"

Eval::Eval() {}

int Eval::evaluate(const Board& board) const {
    int score = 0;
    // Iterate over all squares
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece p = board.getPiece(row, col);
            if (p.isNull()) continue;
            int value = pieceValue(p.getType());
            int bonus = pieceSquareBonus(p.getType(), p.getColor(), row, col);
            int total = value + bonus;
            if (p.getColor() == PieceColor::WHITE) {
                score += total;
            } else {
                score -= total;
            }
        }
    }
    return score;
}

int Eval::pieceValue(PieceType type) const {
    switch (type) {
        case PieceType::PAWN: return pawnValue;
        case PieceType::KNIGHT: return knightValue;
        case PieceType::BISHOP: return bishopValue;
        case PieceType::ROOK: return rookValue;
        case PieceType::QUEEN: return queenValue;
        case PieceType::KING: return kingValue;
        default: return 0;
    }
}

int Eval::pieceSquareBonus(PieceType type, PieceColor color, int row, int col) const {
    // For simplicity, we'll use the same table for both colors but flip the row for black.
    int r = (color == PieceColor::WHITE) ? row : (BOARD_SIZE - 1 - row);
    int c = col;
    switch (type) {
        case PieceType::PAWN: return pawnTable[r][c];
        case PieceType::KNIGHT: return knightTable[r][c];
        case PieceType::BISHOP: return bishopTable[r][c];
        case PieceType::ROOK: return rookTable[r][c];
        case PieceType::QUEEN: return queenTable[r][c];
        case PieceType::KING: return kingTable[r][c];
        default: return 0;
    }
}

// Define piece-square tables (simplified, values in centipawns)
const int Eval::pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

const int Eval::knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int Eval::bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int Eval::rookTable[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    {  5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {  0,  0,  0,  5,  5,  0,  0,  0}
};

const int Eval::queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int Eval::kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};
