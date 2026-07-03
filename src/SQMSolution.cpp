#include "../include/SQMSolution.h"

SQMSolution::SQMSolution(const BoardState &boardState, const Board &board) : value(boardState.value) {
    filledInBoard.reserve(board.height);

    for (int y = 0; y < board.height; ++y) {
        auto &row = filledInBoard.emplace_back();
        row.reserve(board.width);

        for (int x = 0; x < board.width; ++x) {
            const auto &filling = boardState.getFillingAt(x, y);
            row.emplace_back(filling.isEmpty() ? std::to_string(board.cells[y][x]) : filling.toString());
        }
    }
}
