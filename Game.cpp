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

std::string Game::print_board() const
{
    return s->print_pieces() + "\n\n" + s->print_board() + "\n\n" + s->print_board2();
}

bool Game::BFS()
{
    std::cout << "Running BFS..." << std::endl;
    
    std::unordered_set<State*> set;
    std::queue<State*> queue;

    State* c = new State(*s);
    set.insert(c); 
    queue.push(c);
    int count=0;

    visited_map[c] = std::make_pair(s, -1);
    while(!queue.empty())
    {
        auto st = queue.front();
        queue.pop();
        set.erase(st);
        count++;
        if(st->is_goal())
        {
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            std::cout << "Nodes visited: " << count << std::endl;

            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            auto copy = new State(*st);
            copy->move(m);
            
            if(visited_map.find(copy) == visited_map.end()){
                visited_map[copy] = std::make_pair(st, m.second);
            }
            else
            {
                delete copy;
                continue;
            }

            if(set.count(copy) <= 0){
                queue.push(copy);
                set.insert(copy);
            }
        }
    }
    std::cout << "No solution found." << std::endl;
    return false;
}

bool Game::Astar()
{
    std::cout << "Running A*..." << std::endl;


    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open;

    std::unordered_set<State*> closed;

    auto c = new State(*s);
    open_s.insert(c);
    open.push(c);
    visited_map[c] = std::make_pair(s, -1);

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
            std::cout << "Length: " << path.size() << std::endl;
            std::cout << "Open list nodes: " << open.size() << std::endl;
            std::cout << "Closed list nodes: " << closed.size() << std::endl;

            //delete st;
            //delete copy;
            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            auto copy = new State(*st);
            copy->move(m);
            //std::cout << copy->print_board() << std::endl;
            if(visited_map.find(copy) == visited_map.end()){
                visited_map[copy] = std::make_pair(st, m.second);
            }
            else{
                delete copy;
                continue;
            }



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
            copy->h = copy->get_h();
            
            open_s.insert(copy);
            open.push(copy);
        }
        closed.insert(st);
        //return;
    }
    //delete copy;
    std::cout << "No solution found." << std::endl;
    return false;
}

bool Game::IDAstar()
{
    std::cout << "Running IDA*..." << std::endl;

    auto bound = s->get_h();

    auto c = new State(*s);
    visited_map[c] = std::make_pair(s, -1);

    int t=-1;
    doneIDAstar=false;
    while(bound < 1000000)
    {
        t = searchIDAstar(c, 0, bound);
        
        if(doneIDAstar){
            std::cout << "Found solution." << std::endl;
            std::vector<int> path = reconstruct_path(goal);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;


            std::cout << visited_map.size() << std::endl;
            return true;
        }
        bound = t;
    }
    std::cout << "No solution found." << std::endl;
    return false;
}

int Game::searchIDAstar(State* s, int g, int bound)
{
    //std::cout << "Calling search..." << std::endl;
    if(s->is_goal()){
        goal=new State(*s);
        //std::vector<int> path = reconstruct_path(s);
        doneIDAstar=true;
        return 0;
    }

    auto moves = s->get_legal_moves();

    int min = 10000000;
    for(const auto& m : moves)
    {
        auto copy = new State(*s);
        copy->move(m); 


        auto h = copy->get_h();
        int t;

        if(copy->get_f() <= bound)
            t = 1 + searchIDAstar(copy, g+1, bound);
        else{
            t = 1 + h;
        }

        if(visited_map.find(copy) == visited_map.end()){
            visited_map[copy] = std::make_pair(s, m.second);
        }

        if(doneIDAstar){
            return t;
        }      

        if(t < min){
            min = t;
        }
    }
    return min;
}


Game::~Game()
{
    
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    if(s != nullptr) {
        delete s;
    }

    if(goal != nullptr){
        delete goal;
    }
}