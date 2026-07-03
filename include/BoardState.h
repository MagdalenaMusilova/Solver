#ifndef SEQUENCE_BOARDSTATE_H
#define SEQUENCE_BOARDSTATE_H
#include <bitset>
#include <memory>
#include <vector>

#include "BoardFilling.h"
#include "PosedQuatromin.h"


class BoardState {
private:
    std::vector<BoardFilling> boardFillings;
    std::uint8_t boardWidth;

    void goToNextPos();
public:
    std::uint8_t numOfZ;
    std::uint8_t numOfT;
    int value;
    Coords pos;

    BoardState();
    BoardState(int boardWith);
    BoardState(const BoardState& org);
    BoardState& operator=(const BoardState& other) = default;

    void addValue(int additionalValue);

    void addQuatromin(const PosedQuatromin& quatromin);

    inline int getTypeDifference() const {
        return abs(numOfZ - numOfT);
    }

    int feedBoardIntoBuffer(std::uint8_t* buffer) const;

    bool isFilledAt(int x, int y) const;

    inline bool isFilledAt(Coords coords) const {
        return isFilledAt(coords.x, coords.y);
    }

    inline bool isEmptyAt(int x, int y) const {
        return !isFilledAt(x, y);
    }

    inline bool isEmptyAt(Coords coords) const {
        return isEmptyAt(coords.x, coords.y);
    }

    BoardFilling getFillingAt(int x, int y) const;
};


#endif //SEQUENCE_BOARDSTATE_H
