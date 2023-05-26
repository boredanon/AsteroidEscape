#ifndef STATE_H_
#define STATE_H_

#include "Piece.h"
#include <vector>
#include <stdexcept>

class State
{
    private:
        Piece** pieces;
        int current_empty;
        bool check_collision(int i, uint64_t context, int ignore) const; //Ignore is used mid-movement.
        std::vector<std::pair<int, int>> get_adjacencies(int i) const;
    public:
        State();
        State(std::string pieces, std::string orientations);
        void init();
        void set_piece(int index, PIECE_TYPE pt, ORIENTATION po);
        Piece* get_piece(int index) const;
        Piece* get_empty() const;
        std::vector<std::pair<int,int>> get_legal_moves() const;
        bool is_legal() const;
        void move(std::pair<int,int> m);
        std::string print_pieces() const;
        std::string print_board() const;
        std::string print_board2() const;
        ~State();

};

#endif /*STATE_H_*/