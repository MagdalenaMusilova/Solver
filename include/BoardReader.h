#ifndef SEQUENCE_BOARDREADER_H
#define SEQUENCE_BOARDREADER_H
#include <string>

#include "Board.h"


class BoardReader {
    public:
    static bool ReadBoard(const std::string &filename, Board &outBoard);
};


#endif //SEQUENCE_BOARDREADER_H