//
// Created by majda on 5/25/2026.
//

#ifndef SOLVER_SQMSOLVERSEQ_H
#define SOLVER_SQMSOLVERSEQ_H

#include "SQMSolver.h"

class SQMSolverSeq : public SQMSolver {
public:
    SQMSolverSeq(const Board& board): SQMSolver(board){}
    void solveState(BoardState& curState) override;
};


#endif //SOLVER_SQMSOLVERSEQ_H
