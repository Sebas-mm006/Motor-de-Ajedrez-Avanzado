#include "./../include/Pawn.hpp"
#include "./../include/Board.hpp"
#include <cctype>

Pawn::Pawn(PieceColor color) : Piece(PieceType::PAWN, color) {}

/**
 * @brief Verifica si esta pieza es nula
 */
bool Pawn::isNull() const {
    return false;
}

/**
 * @brief Convierte el peón a representación ASCII
 */
char Pawn::toChar() const {
    char pieceChar = 'P';
    if (getColor() == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}

/**
 * @brief Crea una copia del peón
 */
std::unique_ptr<Piece> Pawn::clone() const {
    return std::make_unique<Pawn>(getColor());
}

/**
 * @brief Genera los movimientos pseudo-legales del peón
 */
void Pawn::generateMoves(int row, int col, const Board& board, std::vector<Move>& moves) const {
    PieceColor color = getColor();
    // La dirección depende del color: blancas suben (-1), negras bajan (+1)
    int direction = (color == PieceColor::WHITE) ? -1 : 1;
    // Fila inicial para mover dos casillas
    int startRow = (color == PieceColor::WHITE) ? 6 : 1;

    // Movimiento un paso hacia adelante
    int oneRow = row + direction;
    if (board.isSquareInside(oneRow, col) && board.getPiece(oneRow, col).isNull()) {
        if (oneRow == 0 || oneRow == 7) {
            // Promoción - ofrecer todos los tipos de piezas
            moves.emplace_back(row, col, oneRow, col, PieceType::QUEEN, color, MoveType::PROMOTION);
            moves.emplace_back(row, col, oneRow, col, PieceType::ROOK, color, MoveType::PROMOTION);
            moves.emplace_back(row, col, oneRow, col, PieceType::BISHOP, color, MoveType::PROMOTION);
            moves.emplace_back(row, col, oneRow, col, PieceType::KNIGHT, color, MoveType::PROMOTION);
        } else {
            moves.emplace_back(row, col, oneRow, col);
        }
        // Movimiento dos pasos desde la fila inicial
        int twoRow = row + 2 * direction;
        if (row == startRow && board.isSquareInside(twoRow, col) && board.getPiece(twoRow, col).isNull()) {
            moves.emplace_back(row, col, twoRow, col);
        }
    }

    // Capturas diagonales
    for (int dc : {-1, 1}) {
        int capRow = row + direction;
        int capCol = col + dc;
        if (board.isSquareInside(capRow, capCol)) {
            const Piece& target = board.getPiece(capRow, capCol);
            if (!target.isNull() && target.getColor() != color) {
                if (capRow == 0 || capRow == 7) {
                    // Captura con promoción - ofrecer todos los tipos
                    moves.emplace_back(row, col, capRow, capCol, PieceType::QUEEN, color, MoveType::PROMOTION);
                    moves.emplace_back(row, col, capRow, capCol, PieceType::ROOK, color, MoveType::PROMOTION);
                    moves.emplace_back(row, col, capRow, capCol, PieceType::BISHOP, color, MoveType::PROMOTION);
                    moves.emplace_back(row, col, capRow, capCol, PieceType::KNIGHT, color, MoveType::PROMOTION);
                } else {
                    moves.emplace_back(row, col, capRow, capCol);
                }
            } else if (capRow * 8 + capCol == board.getEnPassantSq()) {
                // Captura al paso (en passant)
                moves.emplace_back(row, col, capRow, capCol, PieceType::NONE, PieceColor::NONE, MoveType::EN_PASSANT);
            }
        }
    }
}