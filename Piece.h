#ifndef PIECE_H_
#define PIECE_H_

#include "Common.h"
#include <bit>  

class Piece
{ 
    private:
        uint64_t p;
    public:
        Piece(PIECE_TYPE pi, ORIENTATION po);
        std::string print_piece() const;
};

#endif /*PIECE_H_*/