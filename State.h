#ifndef STATE_H_
#define STATE_H_

#include "Piece.h"
#include <vector>

class State
{
    private:
        Piece** pieces;

        std::vector<std::pair<int, int>> get_adjacencies(int i) const;
    public:
        void init();
        void set_piece(int index, PIECE_TYPE pt, ORIENTATION po);
        Piece* get_piece(int index);
        bool is_legal() const;
        std::string print_pieces() const;
        std::string print_board() const;
        std::string print_adjacencies() const;
        ~State();

};

#endif /*STATE_H_*/