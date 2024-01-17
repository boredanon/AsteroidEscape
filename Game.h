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
#include <thread>
#include <filesystem>

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
    std::vector<int> reconstruct_path3(State* goal, bool reverse = false);
    std::vector<int> reconstruct_path4(State* goal, bool reverse = false, std::unordered_map<State*, std::pair<State*, int>> visited_map = std::unordered_map<State*, std::pair<State*, int>>());
    std::vector<std::pair<State*, int>> reconstruct_path_states(State* goal);

    int doneIDAstar;
    int doneMUSE;
    double minimum=10000.0;
    int searchIDAstar(State* s, int g, int bound);
    double minimum_path_entropy(State* s, int g);
    int minimum_solution_length=-1;

    std::vector<int> aStarPath;
    bool generateRandomPuzzle(int maxSolutionLength);
    bool generateRandomPuzzle2(int maxSolutionLength, int maxIterations);
    bool generateRandomPuzzle3(int maxSolutionLength, int maxIterations);
    bool generateRandomPuzzle4(int maxSolutionLength, int maxIterations);
    bool generateRandomPuzzle5(std::string filename);
    bool generateRandomPuzzle6(std::string filename);

    void savePuzzleInFile(State* puzzle, std::vector<int>& path, std::string filePath = "");
    void savePuzzleInFile2(State* puzzle, std::vector<int>& path, std::string filePath = "");

    public:
        Game();
        Game(int mode, std::string file_name = "");
        //Game(int numberOfMoves);
        Game(std::string positions, std::string orientations);
        std::string print_board() const;
        bool BFS();
        bool BFS_allpaths();
        bool Astar();
        bool Astar2(State** saveState = nullptr);
        bool Astar2(State* state);
        std::vector<std::pair<State*, int>> Astar3(State* state, std::unordered_map<State*, std::pair<State*, int>>& visited_map_generation);
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
struct Comparator2
{
    bool operator()(const State* a, const State* b) const
    {
        return a->get_f() < b->get_f();
    }
};

struct Comparator3
{
    bool operator()(const State* a, const State* b) const
    {
        return a->get_h() > b->get_h();
    }
};

#endif /*GAME_H_*/