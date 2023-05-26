#ifndef PIECE_H_
#define PIECE_H_

#include "Common.h"
#include <bit>  

class Piece
{ 
    private:
        uint64_t p;

        PIECE_TYPE p_type;
        ORIENTATION o;
    
    public:
        Piece();
        Piece(PIECE_TYPE pi, ORIENTATION po);
        uint64_t get_p() const;

        PIECE_TYPE get_piece_type() const;
        ORIENTATION get_piece_orientation() const;
        void set_piece(Piece* p);
        void set_piece(PIECE_TYPE pt, ORIENTATION po);
        std::string print_piece() const;
        std::string get_type() const;

        static std::string print_as_piece(uint64_t t);
};

#endif /*PIECE_H_*/