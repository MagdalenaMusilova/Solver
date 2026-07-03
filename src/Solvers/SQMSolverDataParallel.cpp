//
// Created by majda on 5/25/2026.
//

#include "../../include/Solvers/SQMSolverDataParallel.h"

void SQMSolverDataParallel::solveState(BoardState &curState) {
    auto firstMoves = getFirstXMoves(MAX_DEPTH);

#pragma omp parallel for schedule(dynamic, 4)
    for (size_t i = 0; i < firstMoves.size(); i++) {
        solveStateSEQ(firstMoves[i]);
    }
}
