#ifndef BOARD_HPP
#define BOARD_HPP

#include "Piece.hpp"
#include "Move.hpp"
#include <array>
#include <vector>
#include <iostream>

constexpr int BOARD_SIZE = 8;

struct CastlingRights {
    bool white_kingside = true;
    bool white_queenside = true;
    bool black_kingside = true;
    bool black_queenside = true;
    
    bool any() const { return white_kingside || white_queenside || black_kingside || black_queenside; }
};

struct UndoInfo {
    Piece captured_piece;
    Piece moved_piece;
    PieceType promoted_type;
    CastlingRights castling_rights;
    int en_passant_sq;
};

/**
 * @brief Representa el tablero de ajedrez y el estado del juego
 */
class Board {
public:
    Board();                                                // Constructor por defecto
    void initStartPosition();                               // Inicializa la posición inicial de ajedrez
    
    /**
     * @brief Ejecuta un movimiento en el tablero (asume que es legal)
     * @param move Movimiento a realizar
     */
    void make_move(const Move& move);
    
    /**
     * @brief Deshace el último movimiento realizado
     * @param move Movimiento que se deshace
     */
    void undo_move(const Move& move);
    
    // Obtiene la pieza en una posición específica
    Piece getPiece(int row, int col) const;
    // Establece una pieza en una posición específica (uso interno)
    void setPiece(int row, int col, const Piece& piece);
    
    // Obtiene cuyo turno es para mover
    PieceColor getTurn() const;
    
    /**
     * @brief Genera todos los movimientos legales para un color dado
     * @param color Color cuyas moves se generan
     * @return Vector de movimientos legales
     */
    std::vector<Move> generateLegalMoves(PieceColor color) const;
    
    /**
     * @brief Verifica si un color está en jaque mate
     * @param color Color a verificar
     * @return true si está en jaque mate, false en caso contrario
     */
    bool is_mate(PieceColor color) const;
    
    /**
     * @brief Verifica si un color está en jaque
     * @param color Color a verificar
     * @return true si está en jaque, false en caso contrario
     */
    bool is_in_check(PieceColor color) const;
    
    // Imprime el tablero en la consola (para depuración)
    void printBoard() const;
    
    // Obtiene el square de en passant (fila * 8 + col, o -1 si no hay)
    int getEnPassantSq() const { return en_passant_sq_; }
    
    // Obtiene los derechos de enroque
    CastlingRights getCastlingRights() const { return castling_rights_; }
    
private:
    std::array<std::array<Piece, BOARD_SIZE>, BOARD_SIZE> board_;  // Matriz 8x8 de piezas
    PieceColor turn_;                                              // Indica cuyo turno es (BLANCO o NEGRO)
    CastlingRights castling_rights_;                               // Derechos de enroque
    int en_passant_sq_;                                            // Square de en passant (-1 si no hay)
    mutable std::vector<UndoInfo> history_;                        // Historial para undo
    
    // Métodos privados auxiliares
    bool isSquareInside(int row, int col) const;                   // Verifica si una posición está dentro del tablero
    
    /**
     * @brief Genera movimientos pseudo-legales (ignorando jaque) para una pieza en (row, col)
     * @param row Fila de la pieza
     * @param col Columna de la pieza
     * @param moves Vector donde se almacenan los movimientos generados
     */
    void generatePieceMoves(int row, int col, std::vector<Move>& moves) const;
    
    /**
     * @brief Verifica si un movimiento dejaría al rey del mismo color en jaque
     * @param move Movimiento a verificar
     * @param color Color del jugador que realiza el movimiento
     * @return true si después del movimiento el rey estaría en jaque, false en caso contrario
     */
    bool wouldMoveBeInCheck(const Move& move, PieceColor color) const;
    
    /**
     * @brief Encuentra la posición del rey de un color dado
     * @param color Color cuyo rey se busca
     * @return Pará (fila, columna) del rey, o (-1,-1) si no se encuentra
     */
    std::pair<int, int> findKing(PieceColor color) const;
    
    /**
     * @brief Actualiza los derechos de enroque después de un movimiento
     */
    void updateCastlingRights(const Move& move, const Piece& piece);
};

#endif // BOARD_HPP