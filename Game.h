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
#include <fstream>

class Game
{
    State* s;
    std::unordered_map<State*, std::pair<State*, int>> visited_map;
    std::unordered_map<State*, std::vector<std::pair<State*, int>>> parents;
    std::unordered_set<State*> visited;
    std::stack<int> _path;
    std::stack<double> entropy_path;
    std::unordered_set<State*> _path_set; 

    

    int nodes;
    std::vector<int> reconstruct_path(State* goal);
    std::stack<std::pair<int,double>> reconstruct_path2(State* goal, std::stack<std::pair<int,double>>& path);

    int doneIDAstar;
    int doneMUSE;
    double minimum=10000.0;
    int searchIDAstar(State* s, int g, int bound);
    double minimum_path_entropy(State* s, int g);
    int minimum_solution_length=-1;

    public:
        Game(std::string positions, std::string orientations);
        std::string print_board() const;
        bool BFS();
        bool BFS_allpaths();
        bool Astar();
        bool Allstar();
        bool IDAstar();
        bool human_player();
        void MUSE();
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