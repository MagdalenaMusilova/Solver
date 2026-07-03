#ifndef SEQUENCE_SQMSOLVER_H
#define SEQUENCE_SQMSOLVER_H
#include "../Board.h"
#include "../Quatromin.h"
#include "../SQMSolution.h"
#include "../PiecePlacement.h"



class SQMSolver {
protected:
    const Board board;
    const Quatromin placableQuatromins[2] = { Quatromin::T, Quatromin::Z};
    BoardState bestState;
    bool isSolved = false;
    int trivialBound;
    int sortedPrefixSum[4]; // sortedPrefixSum[k] = sum of k globally smallest cell values

    bool isDeadEnd(const BoardState& state) const;

    bool canBeLeftEmpty(const BoardState& state) const;

    std::vector<BoardState> getFirstXMoves(int x);

    int collectPossibleMoves(const BoardState &state, PiecePlacement res[]) const;

    int countRemainingFreeCells(const BoardState &state) const;

    void tryUpdateBestState(const BoardState &state);

    /**
     * Makes X amount of moves and then returns the resulting board states.
     */
    void solveStateAndReturn(BoardState &curState, int remainingDepth, std::vector<BoardState> &res);

    void solveStateOMP(BoardState &curState, const int remainingDepth);

    void solveStateSEQ(BoardState &curState);

    virtual void solveState(BoardState& curState) = 0;
public:
    SQMSolver(const Board& board);

    SQMSolution solve();
};


#endif //SEQUENCE_SQMSOLVER_H