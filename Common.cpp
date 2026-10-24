#include "Common.h"
#include <limits>

int GOAL_POSITION = 10;
int PIECES_AMOUNT = 9;
int HEIGHT = 3;
int WIDTH = 3;
bool CAN_RETURN=false;
bool REMUSE=false;
HEURISTIC heuristic;
uint64_t INSIDE=0x3c3c3c3c0000;
uint64_t RIGHT_COLUMN=0xf0f0f0f0f0f0f0f;
uint64_t BOTTOM_ROW = 0xffffffff;

uint64_t SLIDING_LIMITS[4] =
{
    //UP
    0xff00000000000000,
    //DOWN
    0xff,
    //RIGHT
    0x101010101010101,
    //LEFT
    0x8080808080808080
};


//CANONICAL, NINETY, ONE_EIGHTY, TWO_SEVENTY
uint64_t PRECALC_PIECES[7][4] = {
    {0                      , 0                       , 0                       , 0                       }, // Empty
    {0xffff18180000         , 0x3030303c3c303030      , 0x1818ffff0000          , 0xc0c0c3c3c0c0c0c       }, // SG_426A
    {0x3c3c3c00000000       , 0x707070700000          , 0x3c3c3c00000000 >> 24  , 0x707070700000  >> 3}, // SG_426B
    //{0x3c3c3c3c00000000 >> 2, 0xf0f0f0f000000000      , 0xf0f0f0f000000000 >> 32, 0xf0f0f0f000000000 >> 36}, // SG_426C
    {/*0xe0e0e00000000*/0x60e0e00000000        , /*0x70707000000000*/0x60707000000000        , /*0x70707000000000 >> 24*/ 0x70703000, 0xe0e0c00/*0x70707000000000 >> 27*/  },
    {0xc0c00000000          , 0xc0c00000000 << 2      , 0xc0c00000000 >> 14     , 0xc0c00000000 >> 16     }, // SG_426DEF
    {0xc0c30300000          , 0x30300c0c0000          , 0xc0c30300000           , 0x30300c0c0000          }, // SG_426G
    {0x3c3c00000000 >> 16   , 0xc0c0c0c0000           , 0x3c3c00000000          , 0xc0c0c0c0000 << 2      } // SG_426H
};

//Directions where there is adjacency by piece position. Bit is marked for the directions
//TOP LEFT DIAGONAL, TOP, TOP RIGHT DIAGONAL, LEFT, RIGHT, BOTTOM LEFT DIAGONAL, BOTTOM, BOTTOM RIGHT DIAGONAL
uint8_t ADJACENCY[12] = {
    0xb, // Piece 0
    0x1f,  // Piece 1
    0x16, // Piece 2
    0x6b, // Piece 3
    0xff, // Piece 4
    0xd6, // Piece 5
    0x68, // Piece 6
    0xf8, // Piece 7
    0xd0, // Piece 8
    0x0, // This isn't really a part of the board.
    0x40, // Goal piece // Only up.
    0x0
};

uint64_t COMBINED[9] = { //Basically INSIDE|ADJACENTS
    0x3f3f3f3f3f3f, // Piece 1
    0xffffffffffff, // Piece 2
    0xfcfcfcfcfcfc, // Piece 3
    0x3f3f3f3f3f3f3f3f, // Piece 4
    0xffffffffffffffff, // Piece 5
    0xfcfcfcfcfcfcfcfc, // Piece 6
    0x3f3f3f3f3f3f0000, // Piece 7
    0xffffffffffff0000, // Piece 8
    0xfcfcfcfcfcfc0000 // Piece 9
};

std::string get_binary64(uint64_t b)
{
    std::bitset<64> bs(b);
    return bs.to_string();
}

ADJACENCY_DIRECTIONS opposite(ADJACENCY_DIRECTIONS ad)
{
    switch(ad)
    {
        case UP: return DOWN; break;
        case DOWN: return UP; break;
        case LEFT: return RIGHT; break;
        case RIGHT: return LEFT; break;
        case UP_LEFT: return DOWN_RIGHT; break;
        case UP_RIGHT: return DOWN_LEFT; break;
        case DOWN_LEFT: return UP_RIGHT; break;
        case DOWN_RIGHT: return UP_LEFT; break;
        default: return UP; break;
    }
}

std::string print_direction(int dir)
{
    std::string s="";
    switch(dir)
    {
        case UP: s+="UP"; break;
        case DOWN: s+="DOWN"; break;
        case LEFT: s+="LEFT"; break;
        case RIGHT: s+="RIGHT"; break;
        case UP_LEFT: s+= "UP_LEFT"; break;
        case UP_RIGHT: s+= "UP_RIGHT"; break;
        case DOWN_LEFT: s+= "DOWN_LEFT"; break;
        case DOWN_RIGHT: s+= "DOWN_RIGHT"; break;
        default : s+="UNIDENTIFIED"; break;
    }
    return s;
}

std::string heuristic_name(HEURISTIC h)
{
    switch(h)
    {
        case ZERO: return "ZERO";
        case TWOONE: return "TWOONE";
        case OBSTACLES0: return "OBSTACLES0";
        case OBSTACLES1: return "OBSTACLES1";
        case DISTANCE: return "DISTANCE";
        case ROWDISTANCE: return "ROWDISTANCE";
        default: return "UNKNOWN";
    }
}

int get_int_prompt(std::string msg)
{
    int prompt = 0;
    std::cout << msg;
    std::cout << ">> ";
    while (!(std::cin >> prompt))
    {
        if(std::cin.eof()){
            return -1; // No more input.
        }
        std::cin.clear(); // Discard the non-numeric input and ask again.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ">> ";
    }

    return prompt;
}