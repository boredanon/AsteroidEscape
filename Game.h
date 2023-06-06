#ifndef GAME_H_
#define GAME_H_

#include "State.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <vector>

class Game
{
    State* s;
    std::unordered_map<State*, std::pair<State*, int>> visited_map;
    
    std::stack<int> _path;
    std::vector<int> reconstruct_path(State* goal);

    int doneIDAstar;

    int searchIDAstar(State* s, int g, int bound);
    public:
        Game(std::string positions, std::string orientations);
        std::string print_board() const;
        bool BFS();
        bool Astar();
        bool IDAstar();
        ~Game();

};


struct Comparator
{
    bool operator()(const State* a, const State* b) const
    {
        return a->get_f() > b->get_f();
    }
};

#endif /*GAME_H_*/