//
// Created by majda on 5/25/2026.
//

#ifndef SOLVER_SQMSOLVERDATAPARALLEL_H
#define SOLVER_SQMSOLVERDATAPARALLEL_H
#include "SQMSolver.h"


class SQMSolverDataParallel : public SQMSolver {
public:
    SQMSolverDataParallel(const Board& board): SQMSolver(board){}

    void solveState(BoardState& curState) override;
};


#endif //SOLVER_SQMSOLVERDATAPARALLEL_H
