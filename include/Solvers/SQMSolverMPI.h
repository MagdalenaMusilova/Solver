//
// Created by majda on 5/25/2026.
//

#ifndef SOLVER_SQMSOLVERMPI_H
#define SOLVER_SQMSOLVERMPI_H
#include "SQMSolver.h"


class SQMSolverMPI : public SQMSolver {
private:
    int bestValue = INT_MAX;

    bool tryUpdateBestValue(int newValue);

    void sendBoardState(BoardState state, int recievingRank, int tag) const;

    BoardState recieveBoardState(int tag, int& recievedBestValue);

    void masterSolve(int numOfSlaves);

    void slaveSolve(int rank);

public:
    SQMSolverMPI(const Board& board): SQMSolver(board){}

    void solveState(BoardState& curState) override;
};


#endif //SOLVER_SQMSOLVERMPI_H
