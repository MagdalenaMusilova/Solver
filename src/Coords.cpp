#include "../include/Coords.h"

bool Coords::operator<(const Coords &other) const {
    if (y != other.y) return y < other.y;
    return x < other.x;
}

Coords Coords::operator+(const Coords &other) const {
    return Coords(x + other.x, y + other.y);
}

Coords Coords::operator-(const Coords &other) const {
    return Coords(x - other.x, y - other.y);
}
