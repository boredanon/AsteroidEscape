#include <iostream>
#include <fstream>
#include "Game.h"

int main(int argc, char** argv)
{
    Game* g = nullptr;
    //State* s = nullptr;
    std::string algorithm;
    if(argc < 2){
        std::cout << "Not enough arguments." << std::endl;
        return -1;
    }else
    {
        if (std::string(argv[1]) != "r" && std::string(argv[1]) != "s") {
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
                else if(h == "DISTANCE"){
                    heuristic = DISTANCE;
                }
                else{
                    heuristic = ZERO;
                }
            }
            else{
                heuristic = ZERO;
            }

            if(argc > 4){
                std::string lookback(argv[4]);
                std::transform(lookback.begin(), lookback.end(), lookback.begin(), ::toupper);
                if(lookback == "LOOKBACK")
                {
                    CAN_RETURN=true;
                }
            }
        }
    }

    if (std::string(argv[1]) == "r") {
        try
        {
            std::ifstream configFile("config.txt");
            if (!configFile.is_open()) {
                throw std::runtime_error("File does not exist.");
            }

            if (argc == 3) {
                std::string line;

                std::getline(configFile, line);

                configFile.close();

                g = new Game(std::stoi(line), std::string(argv[2]));
            }
            else {
                std::string line;

                std::getline(configFile, line);

                configFile.close();

                g = new Game(std::stoi(line));
            }
        }
        catch(std::exception& e){
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }
    else if (std::string(argv[1]) == "s") {
        heuristic = TWOONE;
        g = new Game();
    }
    else {
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
    }

    delete g;
    //delete s;
    return 0;
}