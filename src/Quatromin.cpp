#include "../include/Quatromin.h"

#include <algorithm>
#include <set>

Quatromin::Quatromin(const char type, const QuatrominShape &shape) : type(type) {
    std::set<QuatrominShape> uniqueShapes = std::set<QuatrominShape>();
    QuatrominShape nextShape = getNormalizedShape(shape);
    for (int rotation = 0; rotation < 4; ++rotation) {
        uniqueShapes.insert(nextShape);
        uniqueShapes.insert(getFlippedShape(nextShape));
        nextShape = getRotatedShape(nextShape);
    }
    possibleShapes = std::vector<QuatrominShape>(uniqueShapes.begin(), uniqueShapes.end());
}

QuatrominShape Quatromin::getFlippedShape(const QuatrominShape &shape) {
    QuatrominShape newShape = shape;
    for (Coords &coords: newShape) {
        coords.x = -coords.x;
    }
    return getNormalizedShape(newShape);
}

QuatrominShape Quatromin::getRotatedShape(const QuatrominShape &shape) {
    QuatrominShape newShape = QuatrominShape();
    for (int i = 0; i < QUATROMIN_SIZE; ++i) {
        newShape[i] = {
            (int8_t)-shape[i].y,
            shape[i].x
        };
    }
    return getNormalizedShape(newShape);
}

QuatrominShape Quatromin::getNormalizedShape(const QuatrominShape &shape) {
    auto topLeft = *std::min_element(shape.begin(), shape.end());

    QuatrominShape newShape = shape;
    for (Coords &coords: newShape) {
        coords.x -= topLeft.x;
        coords.y -= topLeft.y;
    }
    std::sort(newShape.begin(), newShape.end());
    return newShape;
}


const Quatromin Quatromin::T = Quatromin('T', {
                                             Coords(0, 0),
                                             {1, 0},
                                             {2, 0},
                                             {1, 1}
                                         });
const Quatromin Quatromin::Z = Quatromin('Z', {
                                             Coords(0, 0),
                                             {1, 0},
                                             {1, 1},
                                             {2, 1}
                                         });
