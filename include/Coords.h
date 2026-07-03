#ifndef SEQUENCE_COORDS_H
#define SEQUENCE_COORDS_H
#include <cstdint>

struct Coords {
    std::int8_t x;
    std::int8_t y;

    Coords():x(0), y(0){}
    Coords(std::int8_t x, std::int8_t y):x(x), y(y){}

    bool operator<(const Coords& other) const;
    Coords operator+(const Coords& other) const;
    Coords operator-(const Coords& other) const;
};


#endif //SEQUENCE_COORDS_H