#ifndef SEQUENCE_BOARDFILLING_H
#define SEQUENCE_BOARDFILLING_H
#include <string>

inline constexpr int EMPTY_CELL = 0;


struct BoardFilling {
    char value[2];

    BoardFilling(char quatrominType, int quatrominIndex);

    inline void changeInto(char quatrominType, int quatrominIndex) {
        value[0] = quatrominType;
        value[1] = quatrominIndex;
    }

    inline bool isEmpty() const {
        return value[0] == EMPTY_CELL;
    }

    inline std::string toString() const {
        std::string res;
        res += value[0];
        res += std::to_string(static_cast<int>(value[1]));
        return res;
    }

    static BoardFilling EmptyCell;
};


#endif //SEQUENCE_BOARDFILLING_H
