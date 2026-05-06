#include "./../include/Piece.hpp"
#include <cctype>

/**
 * @brief Constructor por defecto - crea una pieza nula
 */
Piece::Piece() : type_(PieceType::NONE), color_(PieceColor::NONE) {}

/**
 * @brief Constructor con parámetros
 * @param type Tipo de pieza
 * @param color Color de la pieza
 */
Piece::Piece(PieceType type, PieceColor color) : type_(type), color_(color) {}

/**
 * @brief Obtiene el tipo de pieza
 * @return Tipo de pieza
 */
PieceType Piece::getType() const {
    return type_;
}

/**
 * @brief Obtiene el color de la pieza
 * @return Color de la pieza
 */
PieceColor Piece::getColor() const {
    return color_;
}

/**
 * @brief Establece el tipo de pieza
 * @param type Nuevo tipo de pieza
 */
void Piece::setType(PieceType type) {
    type_ = type;
}

/**
 * @brief Establece el color de la pieza
 * @param color Nuevo color de la pieza
 */
void Piece::setColor(PieceColor color) {
    color_ = color;
}

/**
 * @brief Verifica si la pieza es nula (no representa una pieza real)
 * @return true si es nula, false en caso contrario
 */
bool Piece::isNull() const {
    return type_ == PieceType::NONE && color_ == PieceColor::NONE;
}

/**
 * @brief Convierte la pieza a su representación ASCII
 * @return Carácter que representa la pieza (mayúscula para blanco, minúscula para negro)
 */
char Piece::toChar() const {
    if (isNull()) return ' ';
    char pieceChar = '?';
    switch (type_) {
        case PieceType::PAWN: pieceChar = 'P'; break;
        case PieceType::KNIGHT: pieceChar = 'N'; break;
        case PieceType::BISHOP: pieceChar = 'B'; break;
        case PieceType::ROOK: pieceChar = 'R'; break;
        case PieceType::QUEEN: pieceChar = 'Q'; break;
        case PieceType::KING: pieceChar = 'K'; break;
        default: pieceChar = '?';
    }
    if (color_ == PieceColor::BLACK) {
        pieceChar = std::tolower(static_cast<unsigned char>(pieceChar));
    }
    return pieceChar;
}