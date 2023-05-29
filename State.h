#ifndef STATE_H_
#define STATE_H_

#include "Piece.h"
#include <vector>
#include <stdexcept>
#include <functional>

class State
{
    private:
        Piece** pieces;
        int current_empty;
        bool check_collision(int i, uint64_t context, int ignore) const; //Ignore is used mid-movement.
        std::vector<std::pair<int, int>> get_adjacencies(int i) const;
    public:
        int g;
        int h=0;
        State();
        State(const State& s);
        State(std::string pieces, std::string orientations);
        void init();
        void set_piece(int index, PIECE_TYPE pt, ORIENTATION po);
        Piece* get_piece(int index) const;
        Piece* get_empty() const;
        int get_f() const;
        int get_h() const;
        int get_g() const;
        std::vector<std::pair<int,int>> get_legal_moves() const;
        bool is_legal() const;
        bool can_slide(int from, int to, int direction) const;
        void move(std::pair<int,int> m);
        bool is_goal() const;
        std::string print_pieces() const;
        std::string print_board() const;
        std::string print_board2() const;
        ~State();

};

namespace std{
    template<> struct hash<State>{
        size_t operator()(const State& s) const
        {
            uint64_t p[12];
            for(int i = 0; i < 12; i++)
            {
                p[i]=s.get_piece(i)->get_p();
            }

            auto h = std::hash<uint32_t>{}(p[0]);
            for(int i = 1; i < 12; i++)
            {
                h^=(std::hash<uint32_t>{}(p[i]) << i);
            }
            return h;
        }
    };


    template<> struct hash<State*>{
        size_t operator()(const State* s) const
        {
            uint64_t p[12];
            for(int i = 0; i < 12; i++)
            {
                p[i]=s->get_piece(i)->get_p();
            }

            auto h = std::hash<uint32_t>{}(p[0]);
            for(int i = 1; i < 12; i++)
            {
                h^=(std::hash<uint32_t>{}(p[i]) << i);
            }
            return h;
        }
    };

    template <> struct equal_to<State>{
        bool operator()(const State & x, const State & y) const
        {
            for(int i = 0; i < 12; i++){
                auto x_piece = x.get_piece(i);
                auto y_piece = y.get_piece(i);

                if(x_piece->get_piece_type() != y_piece->get_piece_type()
                || x_piece->get_piece_orientation() != y_piece->get_piece_orientation()){
                    return false;
                }
            }
            return true;
        }
    };

    template <> struct equal_to<State*>{
        bool operator()(const State * x, const State * y) const
        {
            for(int i = 0; i < 12; i++){
                auto x_piece = x->get_piece(i);
                auto y_piece = y->get_piece(i);

                if(x_piece->get_piece_type() != y_piece->get_piece_type()
                || x_piece->get_piece_orientation() != y_piece->get_piece_orientation()){
                    return false;
                }
            }
            return true;
        }
    };
}

#endif /*STATE_H_*/