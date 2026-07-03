#ifndef SEQUENCE_CONSTANTS_H
#define SEQUENCE_CONSTANTS_H
#include "Coords.h"
#include "BoardFilling.h"

#include <array>
#include <climits>  // for INT_MAX


inline constexpr int QUATROMIN_SIZE = 4;
inline constexpr int MINBOARDLENGHT = 3;
inline constexpr int MAXBOARDLENGHT = 20;
inline constexpr int MAXBOARDSIZE (MAXBOARDLENGHT * MAXBOARDLENGHT);
inline constexpr int MAXTHREADS = 64;

inline constexpr int NUM_OF_FIRST_MOVES = 5;
inline constexpr int BUFFER_SIZE = sizeof(int)  //bestValue
                                   + sizeof(std::uint8_t)   // coordinate x
                                   + sizeof(std::uint8_t)   // coordinate y
                                   + sizeof(int)    // value
                                   + sizeof(std::uint8_t)   // numOfT
                                   + sizeof(std::uint8_t)   // numOfZ
                                   + sizeof(int)    // boardBufferSize
                                   + MAXBOARDSIZE * sizeof(BoardFilling);  // boardBuffer

inline constexpr int TAG_TASK = 0;
inline constexpr int TAG_RESULT = 1;
inline constexpr int TAG_TERMINATE = 2;
inline constexpr int TAG_FINAL_RESULT = 3;

inline constexpr int NOT_TERMINATED = -1;

inline constexpr int MAX_DEPTH = 5;

using QuatrominShape = std::array<Coords, QUATROMIN_SIZE>;

#endif //SEQUENCE_CONSTANTS_H
