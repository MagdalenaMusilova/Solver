//
// Created by majda on 5/25/2026.
//

#include "../../include/Solvers/SQMSolverTaskParallel.h"

void SQMSolverTaskParallel::solveState(BoardState &curState) {
#pragma omp parallel
    {
#pragma omp single
        solveStateOMP(curState, MAX_DEPTH);
    }
}
