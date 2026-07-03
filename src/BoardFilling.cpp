#include "../include/BoardFilling.h"

BoardFilling::BoardFilling(char quatrominType, int quatrominIndex) {
    value[0] = quatrominType;
    value[1] = quatrominIndex;
}

BoardFilling BoardFilling::EmptyCell(EMPTY_CELL, EMPTY_CELL);