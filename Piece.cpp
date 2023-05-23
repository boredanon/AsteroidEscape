#include "Piece.h"

Piece::Piece(PIECE_TYPE pt, ORIENTATION po)
{
    p = PRECALC_PIECES[pt][po];
}

std::string Piece::print_piece() const
{
    auto bs = get_binary64(p);
    for(auto i = 1; i < 8; i++)
    {
        bs.insert(8*i + (i-1), "\n");
    }

    return bs;
} 