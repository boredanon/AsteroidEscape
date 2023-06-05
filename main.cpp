#include <iostream>
#include <fstream>
#include "Game.h"

int main(int argc, char** argv)
{
    Game* g = nullptr;
    //State* s = nullptr;
    std::string algorithm;
    if(argc < 3){
        std::cout << "Not enough arguments." << std::endl;
        return -1;
    }

    try
    {
        auto file_name = argv[1];
        algorithm = std::string(argv[2]);
        std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), ::toupper);
        std::ifstream file(file_name);

        if(!file.is_open()){
            throw std::runtime_error("File does not exist.");
        }

        std::string positions, orientations;
        std::getline(file, positions);
        std::getline(file, orientations);

        //0 1 2 3 4 5 6 7 8
        g = new Game(positions, orientations);
        //s = new State(positions, orientations);
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::cout << g->print_board() << std::endl;
    //auto moves = s->get_legal_moves();
    /*for(const auto& m : moves)
    {
        std::cout << "Piece " << m.first << " in direction " << m.second << std::endl;
    }*/

    if(argc > 3){
        std::string h(argv[3]);
        std::transform(h.begin(), h.end(), h.begin(), ::toupper);
        if(h == "ZERO"){
            heuristic = ZERO;
        }
        else if(h == "TWOONE"){
            heuristic = TWOONE;
        }
        else if(h == "OBSTACLES0"){
            heuristic = OBSTACLES0;
        }
        else if(h == "OBSTACLES1"){
            heuristic = OBSTACLES1;
        }
    }else{
        heuristic = ZERO;
    }

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
    
    delete g;
    //delete s;
    return 0;
}