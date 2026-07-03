#ifndef SEQUENCE_SQMSOLUTION_H
#define SEQUENCE_SQMSOLUTION_H
#include <string>
#include <vector>

#include "BoardState.h"
#include "Board.h"


struct  SQMSolution {
    std::vector<std::vector<std::string>> filledInBoard;
    int value;

    SQMSolution() : value(0) {}
    SQMSolution(const BoardState& boardState, const Board& board);
};


#endif //SEQUENCE_SQMSOLUTION_H