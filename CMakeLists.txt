cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess
    src/main.cpp
    src/Board.cpp
    src/Piece.cpp
    src/Search.cpp
    src/Eval.cpp
)

target_include_directories(chess PRIVATE include)