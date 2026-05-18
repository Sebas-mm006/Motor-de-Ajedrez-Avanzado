#include "../include/Board.hpp"
#include "../include/Search.hpp"
#include "../include/Piece.hpp"
#include "../include/Move.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/**
 * @brief Modo demo: muestra un movimiento y sale
 */
void runDemoMode() {
    Board board;
    board.initStartPosition();
    cout << "Initial board:" << endl;
    board.printBoard();
    cout << "Turn: " << (board.getTurn() == PieceColor::WHITE ? "White" : "Black") << endl;

    Search search;
    int depth = 3;
    Move best = search.iterativeDeepening(board, depth);
    cout << "Best move at depth " << depth << ": " << best << endl;

    board.make_move(best);
    cout << "After move:" << endl;
    board.printBoard();
    cout << "Turn: " << (board.getTurn() == PieceColor::WHITE ? "White" : "Black") << endl;
}

/**
 * @brief Modo backend: espera comandos vía stdin y responde vía stdout
 * Formato de comandos (JSON simple):
 *   {"cmd": "init"} - inicializa tablero
 *   {"cmd": "move", "move": "e2e4"} - hace un movimiento
 *   {"cmd": "getmove", "depth": 2} - obtiene mejor movimiento
 *   {"cmd": "board"} - obtiene estado actual del tablero
 *   {"cmd": "quit"} - termina
 * 
 * Respuestas:
 *   {"status": "ok", "data": {...}}
 *   {"status": "error", "message": "..."}
 */
void runBackendMode() {
    Board board;
    board.initStartPosition();
    Search search;
    
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        if (line.find("\"cmd\": \"init\"") != string::npos) {
            board.initStartPosition();
            cout << "{\"status\": \"ok\"}" << endl;
            cout.flush();
        }
        else if (line.find("\"cmd\": \"board\"") != string::npos) {
            // Convert board to a simple string representation
            stringstream ss;
            ss << "{\"status\": \"ok\", \"board\": [";
            bool first = true;
            for (int row = 0; row < BOARD_SIZE; ++row) {
                for (int col = 0; col < BOARD_SIZE; ++col) {
                    if (!first) ss << ",";
                    first = false;
                    Piece p = board.getPiece(row, col);
                    if (p.isNull()) {
                        ss << "null";
                    } else {
                        string typeStr, colorStr;
                        switch (p.getType()) {
                            case PieceType::PAWN: typeStr = "pawn"; break;
                            case PieceType::KNIGHT: typeStr = "knight"; break;
                            case PieceType::BISHOP: typeStr = "bishop"; break;
                            case PieceType::ROOK: typeStr = "rook"; break;
                            case PieceType::QUEEN: typeStr = "queen"; break;
                            case PieceType::KING: typeStr = "king"; break;
                            default: typeStr = "unknown";
                        }
                        colorStr = (p.getColor() == PieceColor::WHITE) ? "white" : "black";
                        ss << "{\"type\": \"" << typeStr << "\", \"color\": \"" << colorStr << "\", \"row\": " << row << ", \"col\": " << col << "}";
                    }
                }
            }
            ss << "], \"turn\": \"" << (board.getTurn() == PieceColor::WHITE ? "white" : "black") << "\"}" << endl;
            cout << ss.str();
            cout.flush();
        }
        else if (line.find("\"cmd\": \"move\"") != string::npos) {
            size_t moveStart = line.find("\"move\": \"");
            if (moveStart != string::npos) {
                moveStart += 9;
                size_t moveEnd = line.find("\"", moveStart);
                if (moveEnd != string::npos) {
                    string moveStr = line.substr(moveStart, moveEnd - moveStart);
                    if (moveStr.length() == 4) {
                        int from_col = moveStr[0] - 'a';
                        int from_row = '8' - moveStr[1];
                        int to_col = moveStr[2] - 'a';
                        int to_row = '8' - moveStr[3];
                        
                        Move move(from_row, from_col, to_row, to_col);
                        board.make_move(move);
                        cout << "{\"status\": \"ok\"}" << endl;
                        cout.flush();
                        continue;
                    }
                }
            }
            cout << "{\"status\": \"error\", \"message\": \"Invalid move format\"}" << endl;
            cout.flush();
        }
        else if (line.find("\"cmd\": \"getmove\"") != string::npos) {
            // Extract depth
            size_t depthStart = line.find("\"depth\": ");
            int depth = 2; // default
            if (depthStart != string::npos) {
                depthStart += 9; // length of "\"depth\": "
                string depthStr = line.substr(depthStart);
                // Find end of number
                size_t depthEnd = depthStr.find_first_of(",}");
                if (depthEnd != string::npos) {
                    depthStr = depthStr.substr(0, depthEnd);
                    try {
                        depth = stoi(depthStr);
                    } catch (...) {
                        depth = 2;
                    }
                }
            }
            
            PieceColor side = board.getTurn();
            vector<Move> moves = board.generateLegalMoves(side);
            if (moves.empty()) {
                cout << "{\"status\": \"ok\", \"move\": null}" << endl;
                cout.flush();
                continue;
            }
            
            Move best = search.searchBestMove(board, depth);
            if (best.isNull()) {
                cout << "{\"status\": \"ok\", \"move\": null}" << endl;
                cout.flush();
                continue;
            }
            
            // Convert move to algebraic notation
            char from_file = 'a' + best.from_col;
            char from_rank = '1' + best.from_row;
            char to_file = 'a' + best.to_col;
            char to_rank = '1' + best.to_row;
            string moveStr = string(1, from_file) + from_rank + to_file + to_rank;
            
            cout << "{\"status\": \"ok\", \"move\": \"" << moveStr << "\"}" << endl;
            cout.flush();
        }
        else if (line.find("\"cmd\": \"quit\"") != string::npos) {
            cout << "{\"status\": \"ok\"}" << endl;
            cout.flush();
            break;
        }
        else {
            cout << "{\"status\": \"error\", \"message\": \"Unknown command\"}" << endl;
            cout.flush();
        }
    }
}

int main(int argc, char* argv[]) {
    // Check if we should run in backend mode
    bool backendMode = false;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "backend") {
            backendMode = true;
            break;
        }
    }
    
    if (backendMode) {
        runBackendMode();
    } else {
        runDemoMode();
    }
    
    return 0;
}