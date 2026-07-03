#ifndef SEQUENCE_POSEDQUATROMIN_H
#define SEQUENCE_POSEDQUATROMIN_H

#include "Constants.h"

struct PosedQuatromin {
    QuatrominShape absShape;
    const Coords relBottomRight;
    const char type;

    PosedQuatromin(QuatrominShape relShape, const Coords &position, char type);

    Coords getTopLeftPosFromBottomRight(const Coords &curPosition) const;

};


#endif //SEQUENCE_POSEDQUATROMIN_H
