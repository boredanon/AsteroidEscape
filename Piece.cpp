#include "Piece.h"

Piece::Piece() : p_type((PIECE_TYPE)0), o((ORIENTATION)0)
{
    p = PRECALC_PIECES[0][0];
}

Piece::Piece(PIECE_TYPE pt, ORIENTATION po) : p_type(pt), o(po)
{
    p = PRECALC_PIECES[pt][po];
}

uint64_t Piece::get_p() const
{
    return p;
}

PIECE_TYPE Piece::get_piece_type() const
{
    return p_type;
}

ORIENTATION Piece::get_piece_orientation() const
{
    return o;
}

void Piece::set_piece(PIECE_TYPE pt, ORIENTATION po)
{
    p_type = pt;
    o = po;

    p = PRECALC_PIECES[pt][po];
}

void Piece::set_piece(Piece* new_p)
{
    p_type = new_p->p_type;
    o = new_p->o;
    p = PRECALC_PIECES[p_type][o];
}

std::string Piece::print_as_piece(uint64_t t)
{
    auto bs = get_binary64(t);
    for(auto i = 1; i < 8; i++)
    {
        bs.insert(8*i + (i-1), "\n");
    }
    return bs + "\n";
}

std::string Piece::print_piece() const
{
    auto bs = get_binary64(p);
    for(auto i = 1; i < 8; i++)
    {
        bs.insert(8*i + (i-1), "\n");
    }

    return bs + "\n";
} 

std::string Piece::get_type() const
{
    std::string s;
    switch (p_type)
    {
        case SG_426A:
            s+="A";
            break;
        case SG_426B:
            s+="B";
            break;
        case SG_426C:
            s+="C";
            break;
        case SG_426DEF:
            s+="D";
            break;
        case SG_426G:
            s+="G";
            break;
        case SG_426H:
            s+="H";
            break;
        default:
            s+="E";
            break;
    }
    s+="-";
    switch(o)
    {
        case CANONICAL:
            s+="000";
            break;
        case NINETY:
            s+="090";
            break;
        case ONE_EIGHTY:
            s+="180";
            break;
        case TWO_SEVENTY:
            s+="270";
            break;
    }

    return s;
}