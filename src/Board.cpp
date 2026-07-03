#include "../include/Board.h"
#include <stdexcept>

Board::Board(const int width, const int height, const std::vector<int> &boardValues) : width(width), height(height),
    size(height * width) {
    if (width < MINBOARDLENGHT ||
        height < MINBOARDLENGHT ||
        width > MAXBOARDLENGHT ||
        height > MAXBOARDLENGHT ||
        boardValues.size() != static_cast<size_t>(size)) {
        throw std::invalid_argument("Invalid board size");
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x] = boardValues[y * width + x];
        }
    }
}


bool Board::isOutOfBoundsSimplified(const Coords &coords) const {
    return coords.y >= height;
}

bool Board::isOutOfBounds(const Coords &coords) const {
    return !(coords.x >= 0 && coords.x < width && coords.y >= 0 && coords.y < height);
}

bool Board::canPlaceQuatromin(const BoardState& state, QuatrominShape quatromin) const {
    for (int i = 0; i < QUATROMIN_SIZE; ++i) {
        Coords absCoords = quatromin[i] + state.pos;
        if (isOutOfBounds(absCoords) || state.isFilledAt(absCoords)) {
            return false;
            }
    }
    return true;
}
