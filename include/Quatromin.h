#ifndef SEQUENCE_QUATROMIN_H
#define SEQUENCE_QUATROMIN_H
#include <vector>
#include "Constants.h"

class Quatromin {
private:
    static QuatrominShape getRotatedShape(const QuatrominShape &shape);
    static QuatrominShape getFlippedShape(const QuatrominShape &shape);
    static QuatrominShape getNormalizedShape(const QuatrominShape &shape);
public:
    const char type;
    std::vector<QuatrominShape> possibleShapes;

    static const Quatromin T;
    static const Quatromin Z;


    Quatromin(char type, const QuatrominShape& shape);
};


#endif //SEQUENCE_QUATROMIN_H