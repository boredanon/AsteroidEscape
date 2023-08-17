
#include "Game.h"

Game::Game(std::string positions, std::string orientations)
{
    s = new State(positions, orientations);
}

std::vector<int> Game::reconstruct_path(State* goal)
{
    std::vector<int> p;

    auto t = goal;

   // while(!std::equal_to<State*>{}(t, s))

    while(visited_map[t].second != -1)
    {
        p.push_back(visited_map[t].second);
        t = visited_map[t].first;
    }

    return p;
}


std::stack<std::pair<int,double>> Game::reconstruct_path2(State* g, std::stack<std::pair<int,double>>& path)
{
    if(g->g == 0) {
        //std::cout << std::endl;
        //std::cout << std::endl;
        return path;
    }
    auto t = g;
    auto parent = parents[t];

    std::cout << g->g << std::endl;
    for(auto i = parent.begin(); i != parent.end(); i++)
    {
        //std::cout << print_direction(i->second) << " ";
        std::vector<double> dummy;
        //std::cout << print_direction(i->second) << " ";
        path.push({i->second,i->first->calculate_entropy(dummy)});
        reconstruct_path2(i->first, path);
        //std::cout << std::endl;
    }

    return path;
}


std::string Game::print_board() const
{
    return s->print_board() + "\n\n" + s->print_board2() + "\n\n";
}

bool Game::BFS()
{
    std::cout << "Running BFS..." << std::endl;
    


    std::unordered_set<State*> set;
    std::queue<State*> queue;

    State* c = new State(*s);
    set.insert(c); 
    queue.push(c);
    nodes=0;

    auto start = std::chrono::high_resolution_clock::now();
    visited_map[c] = std::make_pair(s, -1);


    while(!queue.empty())
    {
        auto st = queue.front();
        queue.pop();
        set.erase(st);
        if(st->is_goal())
        {

            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
            std::cout << "Queue size: " << queue.size() << std::endl;
            std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;

            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {   
            auto copy = new State(*st);
            copy->move(m);
            nodes++;
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

bool Game::BFS_allpaths()
{
    std::cout << "Running BFS (All min. paths)" << std::endl;
    std::unordered_set<State*> set;
    std::queue<State*> queue;
    std::vector<double> entropies;

    State* c = new State(*s);
    set.insert(c);
    queue.push(c);
    parents[c].push_back({s, -1});
    nodes=0;


    auto start = std::chrono::high_resolution_clock::now();
    auto solution_start = std::chrono::high_resolution_clock::now();
    while(!queue.empty())
    {
        auto st = queue.front();
        queue.pop();
        
        if(st->is_goal())
        {   
            if(minimum_solution_length == -1)
            {
                double solution_entropy=0.0;
                auto solution_end = std::chrono::high_resolution_clock::now();
                std::cout << "Solution found!" << std::endl;
                minimum_solution_length = st->g;
                std::cout << "Path: ";
                std::stack<std::pair<int,double>> path;
                //path = reconstruct_path2(st, path);
                while(!path.empty())
                {
                    auto move = path.top();
                    path.pop();
                    std::cout << print_direction(move.first) << " ";
                    solution_entropy+=move.second;
                }
                std::cout << std::endl;
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(solution_end-solution_start).count();
                std::cout << "Length: " << st->g << std::endl;
                std::cout << "Entropy for this solution: " << solution_entropy << std::endl;
                std::cout << "Time for this solution: " << (diff/1000.0) << "s" << std::endl;
                std::cout << "Nodes evaluated for this solution: " << nodes << std::endl;
                std::cout << std::endl;

                entropies.push_back(solution_entropy);
                solution_start = std::chrono::high_resolution_clock::now();
            }
            else
            {
                if(st->g > minimum_solution_length)
                {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << "All min. paths found." << std::endl;
                    std::cout << "Total nodes evaluated:" << nodes << std::endl;
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                    std::cout << "Total time elapsed: " << diff/1000.0 << "s." << std::endl;
                    return true;
                }
                else
                {
                    double solution_entropy=0.0;
                    auto solution_end = std::chrono::high_resolution_clock::now();
                    std::cout << "Solution found! " << std::endl;
                    std::cout << "Path: ";
                    std::stack<std::pair<int,double>> path;
                    //path = reconstruct_path2(st, path);

                    while(!path.empty())
                    {
                        auto move = path.top();
                        path.pop();
                        std::cout << print_direction(move.first) << " ";
                        solution_entropy+=move.second;
                    }
                    std::cout << std::endl;

                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(solution_end-solution_start).count();
                    std::cout << "Length: " << st->g << std::endl;
                    std::cout << "Entropy for this solution: " << solution_entropy << std::endl;
                    std::cout << "Time for this solution: " << (diff/1000.0) << "s" << std::endl;
                    std::cout << "Nodes evaluated for this solution: " << nodes << std::endl;
                    std::cout << std::endl;

                    entropies.push_back(solution_entropy);
                    solution_start = std::chrono::high_resolution_clock::now();
                }
            }
            //return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto &m : moves)
        {
            auto copy = new State(*st);
            copy->move(m); // We can use the g value here. The method move updates its value.
            nodes++;

            if(set.count(copy) <= 0) // Not visited.
            {
                queue.push(copy);
                set.insert(copy);
                parents[copy].push_back({st, m.second});
            }else{
                auto p = parents[copy];
                auto g = st->g;
                bool to_add = true;
                for(auto pr = p.begin(); pr != p.end(); pr++)
                {
                    if(pr->first->g != g){
                        to_add = false;
                        break;
                    }
                }

                if(to_add)
                {
                    parents[copy].push_back({st,m.second});
                }else{
                    delete copy;
                }
            }
        }
    }
    std::cout << "No solution found." << std::endl;
    return false;
}

bool Game::Allstar()
{
    std::cout << "Running All*..." << std::endl;

    std::vector<double> entropies;
    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open_q;

    std::unordered_set<State*> closed;
    
    auto c = new State(*s);
    open_s.insert(c);
    open_q.push(c);
    nodes = 0;

    parents[c].push_back({s, -1});

    auto start = std::chrono::high_resolution_clock::now();
    auto solution_start = std::chrono::high_resolution_clock::now();
    while(!open_q.empty())
    {
        auto st = open_q.top();
        open_s.erase(st);
        open_q.pop();

        if(st->is_goal())
        {
            if(minimum_solution_length == -1)
            {
                double solution_entropy = 0.0;
                auto solution_end = std::chrono::high_resolution_clock::now();
                minimum_solution_length = st->g;
                std::cout << "Goal found!" << std::endl;
                std::stack<std::pair<int,double>> path;
                //path = reconstruct_path2(st, path);
                std::cout << "Path: ";
                /*while(!path.empty())
                {
                    auto move = path.top();
                    path.pop();
                    std::cout << print_direction(move.first) << " ";
                    solution_entropy+=move.second;
                }*/
                std::cout << std::endl;
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(solution_end-solution_start).count();
                std::cout << "Entropy for this solution: " << solution_entropy << std::endl;
                std::cout << "Length: " << st->g << std::endl;
                std::cout << "Time for this solution: " << (diff/1000.0) << "s" << std::endl;
                std::cout << "Total nodes evaluated so far: " << closed.size() << std::endl;
                std::cout << std::endl;
                entropies.push_back(solution_entropy);
                solution_start = std::chrono::high_resolution_clock::now();

            }
            else
            {
                if(st->g > minimum_solution_length)
                {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << "All min. paths found." << std::endl;
                    std::cout << "Closed list size:" << closed.size() << std::endl;
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                    std::cout << "Total time elapsed: " << diff/1000.0 << "s." << std::endl;
                    return true;
                }
                //else
                if(st->g <= minimum_solution_length)
                {   
                    double solution_entropy = 0.0;
                    auto solution_end = std::chrono::high_resolution_clock::now(); 
                    std::cout << "Goal found!" << std::endl;
                    std::stack<std::pair<int,double>> path;
                    //path = reconstruct_path2(st, path);
                    std::cout << "Path: ";
                    /*while(!path.empty())
                    {
                        auto move = path.top();
                        path.pop();
                        std::cout << print_direction(move.first) << " ";
                        solution_entropy+=move.second;
                    }*/
                    std::cout << std::endl;

                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(solution_end-solution_start).count();
                    std::cout << "Length: " << st->g << std::endl;
                    std::cout << "Entropy for this solution: " << solution_entropy << std::endl;
                    std::cout << "Time for this solution: " << (diff/1000.0) << "s" << std::endl;
                    std::cout << "Total nodes evaluated so far: " << closed.size() << std::endl;
                    std::cout << std::endl;
                    entropies.push_back(solution_entropy);
                    solution_start = std::chrono::high_resolution_clock::now();
                }
            }

        }

        auto moves = st->get_legal_moves();

        for(const auto&m : moves)
        {
            nodes++;
            auto copy = new State(*st);
            copy->move(m);

            auto new_g = st->get_g() + 1;

            auto open_it = open_s.find(copy);
            auto closed_it = closed.find(copy);

            if(open_it != open_s.end())
            {
                if(copy->get_g() < new_g)
                {
                    delete copy;
                    continue;
                }
                else if(copy->get_g() == new_g)
                {
                    copy->g = new_g;
                    copy->h = copy->get_h();

                    open_s.insert(copy);
                    open_q.push(copy);
                    parents[copy].push_back({st, m.second});
                    continue;
                }
            }

            if(closed_it != closed.end())
            {
                if(copy->get_g() <= new_g)
                {
                    delete copy;
                    continue;
                }
                closed.erase(copy);
            }

            //Not visited.
            copy->g = new_g;
            copy->h = copy->get_h();

            open_s.insert(copy);
            open_q.push(copy);
            parents[copy].push_back({st, m.second});
        }
        closed.insert(st);
    }


    std::cout << "No solution found..." << std::endl;
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
    auto start = std::chrono::high_resolution_clock::now();
    while(!open.empty())
    {
        auto st = open.top();
        open_s.erase(st);
        open.pop();

        //std::cout << st->print_board() << std::endl;
        if(st->is_goal()){
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            std::cout << "Closed list nodes: " << closed.size() << std::endl;
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
            std::cout << "All iterations: " << nodes << std::endl;

            //delete st;
            //delete copy;
            return true;
        }

        auto moves = st->get_legal_moves();
        for(const auto& m: moves)
        {
            nodes++;
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
    //bound = 20;
    int t;
    doneIDAstar=false;
    auto start = std::chrono::high_resolution_clock::now();
    _path_set.insert(s);
    int total_nodes=0;
    while(bound < 1000)
    {
        nodes=0;
        t = searchIDAstar(s, 0, bound);

       // std::cout << "Bound: " << bound;
        //std::cout << " Nodes visited " << nodes << std::endl;
        total_nodes+=nodes;
        //total_repeated+=repeated_nodes;
        //auto bound_end = std::chrono::high_resolution_clock::now();

        //auto bound_diff = std::chrono::duration_cast<std::chrono::milliseconds>(bound_end-bound_start).count();
        //std::cout << "Bound " << bound <<"  bound time: " << bound_diff/1000.0 << "ms" << std::endl;

        if(doneIDAstar){
            auto end = std::chrono::high_resolution_clock::now();
            //std::cout << "Bound: " << bound << std::endl;
            std::cout << "Goal found!" << std::endl;

            std::cout << "Solution: ";
            auto l = _path.size();
            std::stack<int> aux;
            while(!_path.empty())
            {
                auto d = _path.top();
                _path.pop();
                aux.push(d);
            }
            while(!aux.empty())
            {
                auto d = aux.top();
                aux.pop();
                std::cout << print_direction(d) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << l << std::endl;
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            std::cout << "Nodes expanded at bound " << bound << ": " << nodes << std::endl;
            std::cout << "Total nodes expanded (considering repeats): " << total_nodes << std::endl;
            std::cout << "Nodes per second: " << (double)total_nodes/(diff/1000.0) << std::endl; 
            return true;
        }
        bound = t;
    }
    std::cout << "No solution found." << std::endl;
    return false;
}

int Game::searchIDAstar(State* s, int g, int bound)
{
    nodes++;
    //std::cout << "Calling search..." << std::endl;

    if(s->is_goal()){
        doneIDAstar=true;
        return 0;
    }

    auto moves = s->get_legal_moves();
    int min = 10000000;
    for(const auto& m : moves)
    {
        auto copy = new State(*s);
        copy->move(m); //This calculates h and adds one to g

        if(_path_set.find(copy) == _path_set.end()){

            _path.push(m.second);
            _path_set.insert(copy);

            int t;
            if(copy->get_f() <= bound){
                t = 1 + searchIDAstar(copy, g+1, bound);
            }
            else{
                t = 1 + copy->get_h();
            }
            if(doneIDAstar){
                delete copy;
                return t;
            }

            if(t < min){
                min = t;
            }
            _path.pop();
            _path_set.erase(copy);
        }
        delete copy;
    }
    return min;
}

bool Game::human_player(){
    std::cout << "Human player..." << std::endl;

    do{
        std::cout << print_board() << std::endl;
        auto moves = s->get_legal_moves();

        std::cout << "Available moves: (";
        for(unsigned int i = 0; i < moves.size(); i++)
        {
            auto m = moves[i];
            std::cout << " " << i << ": Piece " << m.first << " to " << print_direction(m.second);
            std::cout << " ; ";
        }
        std::cout << " " << moves.size() << ": Give up )"<< std::endl;

        unsigned int pick=0;
        do{
            pick = ::get_int_prompt("Pick your move ");

            if(pick > moves.size()){
                std::cout << "Invalid move." << std::endl;
            }

        }while(pick > moves.size());

        if(pick == moves.size()){
            break;
        }

        s->move(moves[pick]);
        _path.push(moves[pick].second);
    }while(!s->is_goal());

    if(s->is_goal()){
        std::cout << "Found solution." << std::endl;
    }else{
        std::cout << "Did not find solution." << std::endl;
    }

    std::cout << "Moves: ";
    std::stack<int> aux;
    while(!_path.empty())
    {
        auto d = _path.top();
        _path.pop();
        aux.push(d);
    }
    while(!aux.empty())
    {
        auto d = aux.top();
        aux.pop();
        std::cout << print_direction(d) << " ";
    }
    std::cout << std::endl;


    return true;
}


void Game::MUSE()
{
    if(!REMUSE){
        std::cout << "Running MUSE..." << std::endl;
    }
    else
        std::cout << "Running ReMUSE..." << std::endl;

    std::vector<double> d;
    std::cout << "Score: " << minimum_path_entropy(s, 0) << std::endl;
}

double Game::minimum_path_entropy(State* s, int g)
{
    if(minimum_solution_length != -1 && g > minimum_solution_length){
        return 10000.0;
    }

    if(s->is_goal()) // Solved.
    {
        if(minimum_solution_length == -1)
        {
            minimum_solution_length = g;
        }
        else
        {
            if(g <= minimum_solution_length)
            {
                minimum_solution_length = g;
                auto t = _path;
                while(!t.empty())
                {
                    auto p = t.top();

                    std::cout << print_direction(p) << " ";
                    t.pop();
                }
                std::cout << std::endl;
                std::cout << std::endl;

            }
        }
        return 0.0;
    }

    auto moves = s->get_legal_moves();
    
    if(moves.size() == 0) // No solution. Return infinite entropy.
    {
        return 10000.0;
    }

    std::vector<double> child_entropies;

    
    for(const auto &m : moves)
    {
        auto child = new State(*s);
        child->move(m);

        if(_path_set.find(child) == _path_set.end()){
            _path_set.insert(child);
            _path.push(m.second);
            child_entropies.push_back(minimum_path_entropy(child, g+1));
            _path_set.erase(child);
            _path.pop();
        }
        delete child;
    }
    
    bool all_zeros = std::all_of(child_entropies.begin(), child_entropies.end(), [](double i){return i == 0.0;});
    /*if(all_zeros){
        return s->calculate_entropy(child_entropies);
    }*/

    if(child_entropies.size() == 0){
        return 10000.0;
    }

    auto immediate = s->calculate_entropy(child_entropies);
    auto min = *std::min_element(child_entropies.begin(), child_entropies.end());

    return immediate+min;
}

Game::~Game()
{
    
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();


    /*for(auto it = visited_multimap.begin(); it != visited_multimap.end(); it++){
        delete it->first;
    }
    visited_multimap.clear();-*/

    if(s != nullptr) {
        delete s;
    }
}