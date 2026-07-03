#include "../include/BoardState.h"

BoardState::BoardState():value(INT_MAX) {

}

BoardState::BoardState(int boardWith) : boardWidth(boardWith), numOfZ(0), numOfT(0), value(0), pos(0, 0) {
}

BoardState::BoardState(const BoardState &org) : boardFillings(org.boardFillings), boardWidth(org.boardWidth), numOfZ(org.numOfZ), numOfT(org.numOfT),
                                                value(org.value), pos(org.pos) {
}

void BoardState::goToNextPos() {
    // go to the next empty cell
    do {
        if (pos.x == boardWidth - 1) {
            pos.x = 0;
            pos.y++;
        } else {
            pos.x++;
        }
    } while (isFilledAt(pos));
}

void BoardState::addValue(int additionalValue) {
    value += additionalValue;
    goToNextPos();
}

void BoardState::addQuatromin(const PosedQuatromin &quatromin) {
    int quatrominIndex;
    if (quatromin.type == 'Z') {
        numOfZ++;
        quatrominIndex = numOfZ;
    } else {
        numOfT++;
        quatrominIndex = numOfT;
    }

    //make sure that boardFillings is big enough to add quatromin. The last row affected by the quatromin should be fully added
    int neededBoardSize = (quatromin.absShape.back().y + 1) * boardWidth;
    boardFillings.reserve(neededBoardSize);
    for (int i = boardFillings.size(); i < neededBoardSize; ++i) {
        boardFillings.push_back(BoardFilling::EmptyCell);
    }

    for (auto coords : quatromin.absShape) {
        boardFillings[coords.y * boardWidth + coords.x].changeInto(quatromin.type, quatrominIndex);
    }
    goToNextPos();
}


bool BoardState::isFilledAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= boardWidth ||
        static_cast<size_t>(x + y * boardWidth) >= boardFillings.size()) {
        return false;
        }
    return !boardFillings[y * boardWidth + x].isEmpty();
}

BoardFilling BoardState::getFillingAt(int x, int y) const {
    int coords = y * boardWidth + x;
    if (static_cast<size_t>(coords) >= boardFillings.size()) {
        return BoardFilling::EmptyCell;
    }
    return boardFillings[coords];
}
