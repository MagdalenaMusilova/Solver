#ifndef SEQUENCE_BOARD_H
#define SEQUENCE_BOARD_H

#include <vector>

#include "Coords.h"
#include "PosedQuatromin.h"
#include "BoardState.h"

class Board {
public:
    int cells[MAXBOARDLENGHT][MAXBOARDLENGHT];
    int width, height, size;

    Board() = default;
    Board(int width, int height, const std::vector<int> &boardValues);

    bool isOutOfBoundsSimplified(const Coords &coords) const;

    bool isOutOfBounds(const Coords &coords) const;

    bool canPlaceQuatromin(const BoardState& state, QuatrominShape quatromin) const;
};


#endif //SEQUENCE_BOARD_H
