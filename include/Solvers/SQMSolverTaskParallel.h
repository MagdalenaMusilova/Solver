//
// Created by majda on 5/25/2026.
//

#ifndef SOLVER_SQMSOLVERTASKPARALLEL_H
#define SOLVER_SQMSOLVERTASKPARALLEL_H
#include "SQMSolver.h"


class SQMSolverTaskParallel : public SQMSolver {
public:
    SQMSolverTaskParallel(const Board& board): SQMSolver(board){}
    void solveState(BoardState& curState) override;
};


#endif //SOLVER_SQMSOLVERTASKPARALLEL_H
