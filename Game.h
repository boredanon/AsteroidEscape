#ifndef GAME_H_
#define GAME_H_

#include "State.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>

class Game
{
    State* s;
    std::unordered_map<State*, std::pair<State*, int>> visited_map;
    std::vector<int> reconstruct_path(State* goal);
    public:
        Game(std::string positions, std::string orientations);
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