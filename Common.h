#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <bitset>
#include <cstdint>

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

enum HEURISTIC
{
    ZERO,
    TWOONE,
    OBSTACLES0,
    OBSTACLES1,
    DISTANCE,
    ROWDISTANCE
};

extern HEURISTIC heuristic;
extern uint8_t ADJACENCY[12];
extern uint64_t COMBINED[9];
extern uint64_t PRECALC_PIECES[7][4];
extern uint64_t INSIDE; // OUTSIDE = ~INSIDE.

extern uint64_t BOTTOM_ROW;
extern uint64_t RIGHT_COLUMN;
extern uint64_t SLIDING_LIMITS[4];
extern int GOAL_POSITION;
extern int PIECES_AMOUNT;
extern int HEIGHT;
extern int WIDTH;
extern bool CAN_RETURN;
extern bool REMUSE;


extern std::string get_binary64(uint64_t b);
extern ADJACENCY_DIRECTIONS opposite(ADJACENCY_DIRECTIONS ad);
extern std::string print_direction(int dir);
extern std::string heuristic_name(HEURISTIC h);
extern int get_int_prompt(std::string msg);
#endif /*COMMON_H_*/