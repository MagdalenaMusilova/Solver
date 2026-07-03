#include "../../include/Solvers/SQMSolver.h"
#include "../../include/PiecePlacement.h"

#include <algorithm>
#include <vector>


SQMSolver::SQMSolver(const Board &board) : board(board) {
    std::vector<int> allValues;
    allValues.reserve(board.size);
    for (int y = 0; y < board.height; ++y)
        for (int x = 0; x < board.width; ++x)
            allValues.push_back(board.cells[y][x]);
    std::sort(allValues.begin(), allValues.end());

    // trivialBound = sum of (board.size % 4) globally smallest values
    int trivialBoundSize = board.size % 4;
    trivialBound = 0;
    for (int i = 0; i < trivialBoundSize; ++i)
        trivialBound += allValues[i];

    sortedPrefixSum[0] = 0;
    for (int k = 1; k <= 3; ++k)
        sortedPrefixSum[k] = sortedPrefixSum[k - 1] + allValues[k - 1];

    bestState.value = INT_MAX;
}

bool SQMSolver::isDeadEnd(const BoardState &state) const {
    if (isSolved) return true;
    if (state.value >= bestState.value) return true;

    if (bestState.value - state.value <= sortedPrefixSum[3]) {
        int actualUnfilled = 0;
        for (int y = state.pos.y; y < board.height; ++y) {
            int x = (y == state.pos.y) ? state.pos.x : 0;
            for (; x < board.width; ++x)
                if (state.isEmptyAt(x, y)) actualUnfilled++;
        }
        if (state.value + sortedPrefixSum[actualUnfilled % 4] >= bestState.value) return true;
    }

    // are there enough cells left to balance the amount of Zs and Ts?
    int remaining = board.size - (state.pos.y * board.width + state.pos.x);
    if (remaining < (state.getTypeDifference() - 1) * QUATROMIN_SIZE) return true;

    return false;
}

bool SQMSolver::canBeLeftEmpty(const BoardState &state) const {
    // can a quatromin ever be placed backward here?
    // If not, leaving it empty is valid (no rule violation).
    // Based on my drawings on paper — works, but very ugly and doesn't make sense without the paper ":D
    bool res;
    const bool filledAbove = state.isFilledAt(state.pos + Coords(0, -1)) || state.pos.y < 1;
    const bool filledLeft = state.isFilledAt(state.pos + Coords(-1, 0)) || state.pos.x < 1;
    const bool filledLeftAbove = state.isFilledAt(state.pos + Coords(-1, -1)) || state.pos.y < 1 || state.pos.x < 1;
    const bool filledTwoLeft = state.isFilledAt(state.pos + Coords(-2, 0)) || state.pos.x < 2;
    const bool filledTwoLeftAbove = state.isFilledAt(state.pos + Coords(-2, -1)) || state.pos.y < 1 || state.pos.x < 2;
    const bool filledRightAbove = state.isFilledAt(state.pos + Coords(1, -1)) || state.pos.y < 1 || state.pos.x > board.width - 2;
    const bool filledTwoAbove = state.isFilledAt(state.pos + Coords(0, -2)) || state.pos.y < 2;
    const bool filledLeftTwoAbove = state.isFilledAt(state.pos + Coords(-1, -2)) || state.pos.y < 2 || state.pos.x < 1;
    const bool filledRightTwoAbove = state.isFilledAt(state.pos + Coords(1, -2)) || state.pos.y < 2 || state.pos.x > board.width - 2;

    if (filledAbove) {
        // can be only 3 or 5
        res = (!filledLeft && !filledLeftAbove) && // shared requirements
               (!filledTwoLeft || !filledTwoLeftAbove); // is 3 or 5
    } else {
        if (filledRightAbove) {
            // can be only 2 or 8
            res = !filledLeftAbove && // shared requirements
                   (!filledTwoAbove || !filledLeftTwoAbove); // is 2 or 8
        } else {
            res = !filledTwoAbove || // 1
                   !filledLeftAbove || // 4
                   !filledLeft || // 6
                   !filledRightTwoAbove; // 7
        }
    }
    return !res;
}

std::vector<BoardState> SQMSolver::getFirstXMoves(int x) {
    BoardState initialState(board.width);
    std::vector<BoardState> res;
    solveStateAndReturn(initialState, x, res);
    return res;
}


int SQMSolver::collectPossibleMoves(const BoardState &state, PiecePlacement res[]) const {
    int numCandidates = 0;

    for (const auto &quatromin : placableQuatromins) {
        for (const auto &shape : quatromin.possibleShapes) {
            if (board.canPlaceQuatromin(state, shape)) {
                int coverage = 0;
                for (int i = 0; i < QUATROMIN_SIZE; ++i) {
                    Coords abs = shape[i] + state.pos;
                    coverage += board.cells[abs.y][abs.x];
                }
                res[numCandidates++] = {&quatromin, &shape, coverage};
            }
        }
    }
    std::sort(res, res + numCandidates,
              [](const PiecePlacement &a, const PiecePlacement &b) { return a.coverage > b.coverage; });

    return numCandidates;
}


int SQMSolver::countRemainingFreeCells(const BoardState &state) const {
    if (state.pos.y >= board.height) return 0;
    int res = 0;
    for (int x = state.pos.x; x < board.width; ++x) {
        if (state.isEmptyAt(x, state.pos.y)) {
            res += board.cells[state.pos.y][x];
        }
    }
    return res;
}

void SQMSolver::tryUpdateBestState(const BoardState &state) {
    if (state.value < bestState.value) {
        bestState = state;
        if (state.value == trivialBound) {
            isSolved = true;
        }
    }
}

void SQMSolver::solveStateAndReturn(BoardState &curState, int remainingDepth, std::vector<BoardState> &res) {
    if (remainingDepth == 0) {
        res.push_back(curState);
        return;
    }

    // due to how the quatromins are defined, there's no way to place them in the last row -> finish
    if (curState.pos.y >= board.height - 1) {
        if (curState.getTypeDifference() > 1) {
            return;
        }
        int remainingValues = countRemainingFreeCells(curState);
        curState.addValue(remainingValues);
        tryUpdateBestState(curState);
        return;
    }

    // Collect valid piece placements sorted by total covered value (descending).
    // Trying high-coverage placements first finds better solutions earlier.
    // Fixed-size stack array: max 2 types * 4 unique shapes each = 8 candidates.
    PiecePlacement candidates[16];
    int numOfCandidates = collectPossibleMoves(curState, candidates);

    for (int i = 0; i < numOfCandidates; ++i) {
        BoardState newState = curState;
        newState.addQuatromin(PosedQuatromin(*candidates[i].shape, curState.pos, candidates[i].q->type));
        if (!isDeadEnd(newState)) {
            solveStateAndReturn(newState, remainingDepth - 1, res);
        }
        if (isSolved) return;
    }

    // Try leaving the cell empty after piece placements
    if (canBeLeftEmpty(curState)) {
        int cellValue = board.cells[curState.pos.y][curState.pos.x];
        BoardState newState = curState;
        newState.addValue(cellValue);
        if (!isDeadEnd(newState)) {
            solveStateAndReturn(newState, remainingDepth - 1, res);
        }
    }
}

void SQMSolver::solveStateOMP(BoardState &curState, int remainingDepth) {
    if (remainingDepth == 0) {
        solveStateSEQ(curState);
        return;
    }

    // due to how the quatromins are defined, there's no way to place them in the last row -> finish
    if (curState.pos.y >= board.height - 1) {
        if (curState.getTypeDifference() > 1) {
            return;
        }
        int remainingValues = countRemainingFreeCells(curState);
        curState.addValue(remainingValues);
        tryUpdateBestState(curState);
        return;
    }

    // Collect valid piece placements sorted by total covered value (descending).
    // Trying high-coverage placements first finds better solutions earlier.
    // Fixed-size stack array: max 2 types * 4 unique shapes each = 8 candidates.
    PiecePlacement candidates[16];
    int numOfCandidates = collectPossibleMoves(curState, candidates);

    for (int i = 0; i < numOfCandidates; ++i) {
        BoardState newState = curState;
        PosedQuatromin posedQuatromin(*candidates[i].shape, curState.pos, candidates[i].q->type);
        newState.addQuatromin(posedQuatromin);
        if (!isDeadEnd(newState)) {
#pragma omp task firstprivate(newState)
            solveStateOMP(newState, remainingDepth - 1);
        }
        if (isSolved) return;
    }

    // Try leaving the cell empty after piece placements
    if (canBeLeftEmpty(curState)) {
        int cellValue = board.cells[curState.pos.y][curState.pos.x];
        BoardState newState = curState;
        newState.addValue(cellValue);
        if (!isDeadEnd(newState)) {
            solveStateOMP(newState, remainingDepth - 1);
        }
    }
}

void SQMSolver::solveStateSEQ(BoardState &curState) {
    // due to how the quatromins are defined, there's no way to place them in the last row -> finish
    if (curState.pos.y >= board.height - 1) {
        if (curState.getTypeDifference() > 1) {
            return;
        }
        int remainingValues = countRemainingFreeCells(curState);
        curState.addValue(remainingValues);
        tryUpdateBestState(curState);
        return;
    }

    // Collect valid piece placements sorted by total covered value (descending).
    // Trying high-coverage placements first finds better solutions earlier.
    // Fixed-size stack array: max 2 types * 4 unique shapes each = 8 candidates.
    PiecePlacement candidates[16];
    int numOfCandidates = collectPossibleMoves(curState, candidates);

    for (int i = 0; i < numOfCandidates; ++i) {
        BoardState newState = curState;
        newState.addQuatromin(PosedQuatromin(*candidates[i].shape, curState.pos, candidates[i].q->type));
        if (!isDeadEnd(newState)) {
            solveStateSEQ(newState);
        }
        if (isSolved) return;
    }

    // Try leaving the cell empty after piece placements
    if (canBeLeftEmpty(curState)) {
        int cellValue = board.cells[curState.pos.y][curState.pos.x];
        BoardState newState = curState;
        newState.addValue(cellValue);
        if (!isDeadEnd(newState)) {
            solveStateSEQ(newState);
        }
    }
}

SQMSolution SQMSolver::solve() {
    BoardState initState(board.width);
    solveState(initState);
    return { bestState, board };
}
