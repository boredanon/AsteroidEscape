#include <iostream>
#include <fstream>
#include <algorithm>
#include "Game.h"

static void print_usage(const char* program)
{
    std::cerr <<
        "Usage: " << program << " <board_file> <algorithm> [heuristic] [LOOKBACK]\n"
        "  algorithm: BFS | ASTAR | IDASTAR | ALLSTAR | ALLPATHS | HUMAN | MUSE | REMUSE\n"
        "  heuristic: ZERO (default) | TWOONE | OBSTACLES0 | OBSTACLES1 | DISTANCE | ROWDISTANCE\n"
        "  LOOKBACK:  allow a piece to undo the previous move (off by default)\n"
        "Example: " << program << " Boards/starter/starter1 ASTAR DISTANCE\n";
}

static std::string to_upper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

int main(int argc, char** argv)
{
    if(argc < 3){
        std::cerr << "Not enough arguments." << std::endl;
        print_usage(argv[0]);
        return -1;
    }

    const std::string algorithms[] = {"BFS", "ASTAR", "IDASTAR", "ALLSTAR", "ALLPATHS", "HUMAN", "MUSE", "REMUSE"};
    std::string algorithm = to_upper(argv[2]);
    if(std::find(std::begin(algorithms), std::end(algorithms), algorithm) == std::end(algorithms)){
        std::cerr << "Unknown algorithm: " << argv[2] << std::endl;
        print_usage(argv[0]);
        return -1;
    }

    heuristic = ZERO;
    if(argc > 3){
        std::string h = to_upper(argv[3]);
        if(h == "ZERO") heuristic = ZERO;
        else if(h == "TWOONE") heuristic = TWOONE;
        else if(h == "OBSTACLES0") heuristic = OBSTACLES0;
        else if(h == "OBSTACLES1") heuristic = OBSTACLES1;
        else if(h == "DISTANCE") heuristic = DISTANCE;
        else if(h == "ROWDISTANCE") heuristic = ROWDISTANCE;
        else{
            std::cerr << "Unknown heuristic: " << argv[3] << std::endl;
            print_usage(argv[0]);
            return -1;
        }
    }

    if(argc > 4){
        if(to_upper(argv[4]) == "LOOKBACK"){
            CAN_RETURN=true;
        }
        else{
            std::cerr << "Unknown option: " << argv[4] << std::endl;
            print_usage(argv[0]);
            return -1;
        }
    }

    Game* g = nullptr;
    try
    {
        std::ifstream file(argv[1]);
        if(!file.is_open()){
            throw std::runtime_error(std::string("Cannot open board file: ") + argv[1]);
        }

        std::string positions, orientations;
        std::getline(file, positions);
        std::getline(file, orientations);

        g = new Game(positions, orientations);
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::cout << g->print_board() << std::endl;

    if(algorithm == "BFS"){
        g->BFS();
    }
    else if(algorithm == "ASTAR")
    {
        g->Astar();
    }
    else if(algorithm == "IDASTAR"){
        g->IDAstar();
    }
    else if(algorithm == "ALLSTAR"){
        g->Allstar();
    }
    else if(algorithm == "ALLPATHS"){
        g->BFS_allpaths();
    }
    else if(algorithm == "HUMAN"){
        g->human_player();
    }
    else if(algorithm == "MUSE"){
        REMUSE=false;
        g->Allstar();
        g->MUSE();
    }
    else if(algorithm == "REMUSE"){
        REMUSE=true;
        g->Allstar();
        g->MUSE();
    }
    delete g;
    return 0;
}
