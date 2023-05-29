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
        void BFS();
        void Astar();
        void IDAstar();
        ~Game();

};

#endif /*GAME_H_*/