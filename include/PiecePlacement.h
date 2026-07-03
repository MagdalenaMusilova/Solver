#ifndef SOLVERSEQUENTIAL_PIECEPLACEMENT_H
#define SOLVERSEQUENTIAL_PIECEPLACEMENT_H
#include "Constants.h"


class Quatromin;

struct PiecePlacement {
    const Quatromin* q;
    const QuatrominShape* shape;
    int coverage;
};


#endif //SOLVERSEQUENTIAL_PIECEPLACEMENT_H
