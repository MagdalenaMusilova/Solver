#include "../include/PosedQuatromin.h"

#include <array>


PosedQuatromin::PosedQuatromin(QuatrominShape relShape, const Coords &position,
                               const char type) : relBottomRight(relShape.back()), type(type) {
    for (int i = 0; i < QUATROMIN_SIZE; ++i) {
        absShape[i] = relShape[i] + position;
    }
}

Coords PosedQuatromin::getTopLeftPosFromBottomRight(const Coords &curPosition) const {
    return curPosition - relBottomRight;
}
