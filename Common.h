#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <bitset>

enum PIECE_TYPE
{
    EMPTY, // Empty. 0
    SG_426A, // Ship 1
    SG_426B, // Big asteroid 2
    SG_426C, // Big asteroid corner 3
    SG_426DEF, // Small asteroid top corner (This includes D,E,F) 4
    SG_426G, // Small diagonal asteroids 5
    SG_426H, // Small aligned asteroids 6
};

enum ORIENTATION
{
    CANONICAL, // 0 
    NINETY, // 1
    ONE_EIGHTY, // 2
    TWO_SEVENTY // 3
};

enum ADJACENCY_DIRECTIONS
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    UP_LEFT,
    DOWN_LEFT,
    UP_RIGHT,
    DOWN_RIGHT
};

extern uint8_t ADJACENCY[9];
extern uint64_t PRECALC_PIECES[7][4];
extern uint64_t INSIDE; // OUTSIDE = ~INSIDE.

extern uint64_t BOTTOM_ROW;
extern uint64_t RIGHT_COLUMN;

extern std::string get_binary64(uint64_t b);
#endif /*COMMON_H_*/