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
        char get_letter() const;        // Board-file letter: A, B, C, D (D/E/F), G, H; '-' if empty.
        std::string get_name() const;   // Human-readable name of the piece.
        int get_degrees() const;        // Orientation in degrees (0, 90, 180, 270).
        bool has_pixel(int y, int x) const; // Pixel (row y, column x) of the 8x8 bitmap.

        static std::string print_as_piece(uint64_t t);
};

#endif /*PIECE_H_*/