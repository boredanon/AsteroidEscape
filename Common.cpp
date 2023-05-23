#include "Common.h"

uint64_t INSIDE=0x3c3c3c3c0000;
uint64_t V_BORDER=0x242424240000;
uint64_t H_BORDER=0x180000180000;

//CANONICAL, NINETY, ONE_EIGHTY, TWO_SEVENTY
uint64_t PRECALC_PIECES[7][4] = {
    {0                      , 0                       , 0                       , 0                       }, // Empty
    {0xffff3c180000         , 0x3030383c3c383030      , 0x183cffff0000          , 0xc0c1c3c3c1c0c0c       }, // SG_426A
    {0x3c3c3c3c00000000     , 0x3c3c3c3c00000000 >> 14, 0x3c3c3c3c00000000 >> 32, 0x3c3c3c3c00000000 >> 18}, // SG_426B
    {0x3c3c3c3c00000000 >> 2, 0xf0f0f0f000000000      , 0xf0f0f0f000000000 >> 32, 0xf0f0f0f000000000 >> 36}, // SG_426C
    {0xc0c00000000          , 0xc0c00000000 << 2      , 0xc0c00000000 >> 14     , 0xc0c00000000 >> 16     }, // SG_426DEF
    {0xc0c30300000          , 0x30300c0c0000          , 0xc0c30300000           , 0x30300c0c0000          }, // SG_426G
    {0x3c3c00000000 >> 16   , 0xc0c0c0c0000           , 0x3c3c00000000          , 0xc0c0c0c0000 << 2      } // SG_426H
};

std::string get_binary64(uint64_t b)
{
    std::bitset<64> bs(b);
    return bs.to_string();
}