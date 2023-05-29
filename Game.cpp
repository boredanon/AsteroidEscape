#include "Game.h"

Game::Game(std::string positions, std::string orientations)
{
    s = new State(positions, orientations);
}

std::vector<int> Game::reconstruct_path(State* goal)
{
    std::vector<int> p;

    auto t = goal;

    while(visited_map[t].first != nullptr)
    {
        p.push_back(visited_map[t].second);
        t = visited_map[t].first;
    }

    return p;
}

void Game::BFS()
{
    std::cout << "Running BFS..." << std::endl;
    
    std::unordered_set<State*> set;
    std::queue<State*> queue;

    State* copy = new State(*s);
    set.insert(copy); 
    queue.push(copy);

    visited_map[s] = std::make_pair(s, -1);
    while(!queue.empty())
    {
        auto st = queue.front();
        queue.pop();
        set.erase(st);

        if(st->is_goal())
        {
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;

            delete st;
            break;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            auto copy = new State(*st);
            copy->move(m);

            if(set.count(copy) <= 0){ // Is not visited.
                queue.push(copy);
                set.insert(copy);

                visited_map[copy] = std::make_pair(st, m.second);
            }
            else
            {
                delete copy;
            }
        }
        //delete st;
        //return;
    }
    delete copy;
}

Game::~Game()
{
    delete s;
}