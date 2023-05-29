#include "Game.h"

Game::Game(std::string positions, std::string orientations)
{
    s = new State(positions, orientations);
}

std::vector<int> Game::reconstruct_path(State* goal)
{
    std::vector<int> p;

    auto t = goal;

    while(!std::equal_to<State*>{}(t, s))
    {
        p.push_back(visited_map[t].second);
        t = visited_map[t].first;
    }

    return p;
}

bool Game::BFS()
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


            //delete st;
            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            auto copy = new State(*st);
            copy->move(m);

            if(set.count(copy) <= 0){
                queue.push(copy);
                set.insert(copy);

                if(visited_map.find(copy) == visited_map.end()){
                    visited_map[copy] = std::make_pair(st, m.second);
                }
            }
            else
            {
                delete copy;
            }
        }
        //delete st;
        //return;
    }
    //delete copy;
    return false;
}

bool Game::Astar()
{
    std::cout << "Running A*..." << std::endl;


    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open;

    std::unordered_set<State*> closed;

    auto copy = new State(*s);
    open_s.insert(copy);
    open.push(copy);
    visited_map[s] = std::make_pair(s, -1);

    while(!open.empty())
    {
        auto st = open.top();
        open_s.erase(st);
        open.pop();

        //std::cout << st->print_board() << std::endl;
        if(st->is_goal()){
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;

            //delete st;
            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            auto copy = new State(*st);
            copy->move(m);
            //std::cout << copy->print_board() << std::endl;

            auto new_g = st->get_g() + 1;

            auto open_it = open_s.find(copy);
            auto closed_it = closed.find(copy);

            if(open_it != open_s.end()){
                if(copy->get_g() <= new_g){
                    continue;
                }
            }
            if(closed_it != closed.end()){
                
                if((copy->get_g() <= new_g)){
                    continue;
                }

                closed.erase(copy);
            }

            copy->g = new_g;
            copy->h = 0;
            if(visited_map.find(copy) == visited_map.end())
                visited_map[copy] = std::make_pair(st, m.second);
            
            open_s.insert(copy);
            open.push(copy);
        }
        closed.insert(st);
        //return;
    }
    return false;
}

Game::~Game()
{
    delete s;
}