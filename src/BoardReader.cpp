#include "../include/BoardReader.h"

#include <fstream>
#include <iostream>

bool BoardReader::ReadBoard(const std::string& filename, Board & outBoard) {
    std::ifstream file(filename);
    if (!file)
        return false;

    int height, width;
    std::vector<int> board;

    file >> height >> width;

    if (!file || width <= 0 || height <= 0)
        return false;

    board.clear();
    board.reserve(width * height);

    int value;
    while (file >> value)
    {
        board.push_back(value);
    }

    // Optional safety check
    if (board.size() != static_cast<size_t>(width * height))
    {
        std::cerr << "Error: Expected " << width * height
                  << " values but got " << board.size() << "\n";
        return false;
    }

    outBoard = Board( width, height, board);
    return true;
}
