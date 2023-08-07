#ifndef GAME_H_
#define GAME_H_

#include "State.h"
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <stack>
#include <vector>
#include <chrono>

class Game
{
    State* s;
    std::unordered_map<State*, std::pair<State*, int>> visited_map;
    std::multimap<State*, std::pair<State*, int>> visited_multimap;

    std::unordered_map<State*, std::vector<std::pair<State*, int>>> parents;

    std::stack<int> _path;
    std::unordered_set<State*> _path_set; 
    int nodes;
    std::vector<int> reconstruct_path(State* goal);
    std::stack<int> reconstruct_path2(State* goal, std::stack<int>& path);
    void reconstruct_multipath(State* goal);

    bool allBFS();

    int doneIDAstar;

    int searchIDAstar(State* s, int g, int bound);
    public:
        Game(std::string positions, std::string orientations);
        std::string print_board() const;
        bool BFS();
        bool BFS_allpaths();
        bool Astar();
        bool Allstar();
        bool IDAstar();
        bool human_player();
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