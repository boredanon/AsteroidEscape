
#include "Game.h"

Game::Game() {
    std::string path = "Boards";

    std::ifstream configFile2("config2.txt");
    if (!configFile2.is_open()) {
        throw std::runtime_error("File does not exist.");
    }

    std::string line;
    std::getline(configFile2, line);
    int mode = std::stoi(line);

    configFile2.close();

    if (mode == 0) {

        std::vector<std::string> finishedPuzzles;
        std::ifstream finishedPuzzlesFile("finishedPuzzles");

        if (!finishedPuzzlesFile.is_open()) {
            throw std::runtime_error("File does not exist.");
        }

        while (std::getline(finishedPuzzlesFile, line)) {
            finishedPuzzles.push_back(line);
        }

        finishedPuzzlesFile.close();

        for (const auto& folderPath : std::filesystem::directory_iterator(path)) {
            if (folderPath.path() == path + "/solved" || folderPath.path() == path + "/solved2") continue;

            for (const auto& filePath : std::filesystem::directory_iterator(folderPath.path())) {

                //if (filePath.path().filename() != "starter1") continue;
                bool alreadyFinished = false;
                int index = -1;
                for (int i = 0; i < finishedPuzzles.size(); ++i) {
                    if (filePath.path().filename() == finishedPuzzles[i]) {
                        alreadyFinished = true;
                        index = i;
                    }
                }
                if (alreadyFinished) {
                    finishedPuzzles.erase(finishedPuzzles.begin() + index);
                    continue;
                }

                std::ifstream file(filePath.path());
                if (file.is_open()){
                    std::string positions, orientations;
                    std::getline(file, positions);
                    std::getline(file, orientations);

                    file.close();

                    s = new State(positions, orientations);

                    Astar2(&s);

                    std::string finalPath = "Boards/solved2/" + filePath.path().parent_path().filename().string() + "/" + filePath.path().filename().string();
                    
                    generateRandomPuzzle6(finalPath);

                    delete s;
                    s = nullptr;

                    std::ofstream finishedPuzzlesFile2("finishedPuzzles", std::ios::out | std::ios::app);

                    if (!finishedPuzzlesFile2.is_open()) {
                        throw std::runtime_error("File does not exist.");
                    }

                    finishedPuzzlesFile2 << filePath.path().filename().string() + "\n";

                    finishedPuzzlesFile2.close();
                }
            }
        }
    }
    else if (mode == 1) {
        srand(time(nullptr));

        std::ifstream configFile("config.txt");
        if (!configFile.is_open()) {
            throw std::runtime_error("File does not exist.");
        }

        std::string line;
        std::getline(configFile, line);
        std::getline(configFile, line);
        int puzzleAmount = std::stoi(line);

        configFile.close();

        for (int i = 1; i <= puzzleAmount; ++i) {

            // std::ofstream randomPuzzlesFile("randomPuzzle" + std::to_string(i) + ".txt");
            
            // if (!randomPuzzlesFile.is_open()) {
            //     throw std::runtime_error("File does not exist.");
            // }

            // randomPuzzlesFile.close();

            s = new State();
            s->generateRandom3();

            generateRandomPuzzle6("RandomPuzzles/randomPuzzle" + std::to_string(i) + ".txt");

            delete s;
            s = nullptr;
        }
    }
}

Game::Game(int mode, std::string file_name) {
    srand(time(nullptr));
    
    int puzzleAmount = 2;
    int maxSolutionLength = 20;
    int maxIterations = 1000;
    std::ifstream configFile("config.txt");
    if (configFile.is_open()) {
        std::string line;

        std::getline(configFile, line);
        std::getline(configFile, line);
        puzzleAmount = std::stoi(line);

        std::getline(configFile, line);
        maxSolutionLength = std::stoi(line);

        std::getline(configFile, line);
        maxIterations = std::stoi(line);

        configFile.close();
    }
    
    if (file_name == "") {
        for (int i = 0; i < puzzleAmount; ++i) {
        
            s = new State();

            if (mode == 0) {
                generateRandomPuzzle(maxSolutionLength);
            }
            else if (mode == 1) {
                if (!generateRandomPuzzle2(maxSolutionLength, maxIterations)) {
                    break;
                }
            }
            else if (mode == 2) {
                generateRandomPuzzle3(maxSolutionLength, maxIterations);
            }
        }
    }
    else {
        try
        {
            std::ifstream file(file_name);

            if(!file.is_open()){
                throw std::runtime_error("File does not exist.");
            }

            std::string positions, orientations;
            std::getline(file, positions);
            std::getline(file, orientations);

            //0 1 2 3 4 5 6 7 8
            s = new State(positions, orientations);
            //s = new State(positions, orientations);

            auto start = std::chrono::high_resolution_clock::now();
            
            generateRandomPuzzle4(maxSolutionLength, maxIterations);

            auto end = std::chrono::high_resolution_clock::now();
            
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout << "\n\nTotal time: " << diff/1000.0 << "s" << std::endl;
        }
        catch(std::exception& e){
            std::cerr << e.what() << std::endl;
            return;
        }
    }

    delete s;
    s = nullptr;
}

// Game::Game(int numberOfMoves) {
//     srand(time(nullptr));

//     for (int n = 0; n < 100; ++n) {
//         std::cout << "\n\nPuzzle number " << n << ":\n";
//         s = new State(numberOfMoves);
//         Astar();
//         delete s;
//         s = nullptr;
//     }
// }

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


std::vector<int> Game::reconstruct_path3(State* goal, bool reverse)
{
    std::vector<int> p;

    auto t = goal;

   // while(!std::equal_to<State*>{}(t, s))

    if (reverse) {
        while(visited_map[t].second != -1)
        {
            p.push_back(opposite((ADJACENCY_DIRECTIONS)visited_map[t].second));
            t = visited_map[t].first;
        }
        p.push_back(DOWN);
    }
    else {
        while(visited_map[t].second != -1)
        {
            p.push_back(visited_map[t].second);
            t = visited_map[t].first;
        }
        p.push_back(UP);
    }

    return p;
}

std::vector<int> Game::reconstruct_path4(State* goal, bool reverse, std::unordered_map<State*, std::pair<State*, int>> map)
{
    std::vector<int> p;

    auto t = goal;

   // while(!std::equal_to<State*>{}(t, s))

    if (reverse) {
        while(map[t].second != -1)
        {
            p.push_back(opposite((ADJACENCY_DIRECTIONS)map[t].second));
            t = map[t].first;
        }
        p.push_back(DOWN);
    }
    else {
        while(map[t].second != -1)
        {
            p.push_back(map[t].second);
            t = map[t].first;
        }
        p.push_back(UP);
    }

    return p;
}

std::vector<std::pair<State*, int>> Game::reconstruct_path_states(State* goal)
{
    std::vector<std::pair<State*, int>> p;

    auto t = goal;

   // while(!std::equal_to<State*>{}(t, s))

    while(visited_map[t].second != -1)
    {
        p.push_back(std::make_pair<State*, int>(new State(*(visited_map[t].first)), (int)visited_map[t].second));
        t = visited_map[t].first;
    }

    //std::reverse(p.begin(), p.end());

    return p;
}



std::string Game::print_board() const
{
    return s->print_board() + "\n\n" + s->print_board2() + "\n\n";
}

void Game::savePuzzleInFile(State* puzzle, std::vector<int>& path, std::string filepath) {
    std::ofstream puzzleFile;
    if (filepath == "") {
        puzzleFile.open("puzzles.txt", std::ios::out | std::ios::app);
    }
    else {
        puzzleFile.open(filepath, std::ios::out | std::ios::app);
    }

    if (puzzleFile.is_open()) {

        puzzleFile << puzzle->print_pieces_compact() << "\n\n";
        
        puzzleFile << puzzle->print_board() << "\n";
        puzzleFile << puzzle->print_board2() << "\n";

        puzzleFile << "Solution: ";
        for(int i = path.size()-1; i >= 0; i--){
            puzzleFile << print_direction(path[i]) << " ";
        }
        puzzleFile << "\n";
        puzzleFile << "Solution length: " << path.size() << "\n\n";

        puzzleFile.close();
    }
}

void Game::savePuzzleInFile2(State* puzzle, std::vector<int>& path, std::string filepath) {
    std::ofstream puzzleFile;
    if (filepath == "") {
        puzzleFile.open("puzzles.txt", std::ios::out | std::ios::app);
    }
    else {
        puzzleFile.open(filepath, std::ios::out | std::ios::app);
    }

    if (puzzleFile.is_open()) {

        puzzleFile << puzzle->print_pieces_compact() << "\n\n";

        puzzleFile << "Solution: ";
        for(int i = path.size()-1; i >= 0; i--){
            puzzleFile << print_direction(path[i]) << " ";
        }
        puzzleFile << "\n";
        // puzzleFile << "Solution length: " << path.size() << "\n\n";
        puzzleFile << "Solution length: " << puzzle->get_h() << "\n\n";

        puzzleFile.close();
    }
}

// Generates a random puzzle
bool Game::generateRandomPuzzle(int maxSolutionLength) {

    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();

    s->generateRandom3();

    std::unordered_set<State*> set;
    std::queue<State*> queue;

    State* c = new State(*s);
    set.insert(c); 
    queue.push(c);

    visited_map[c] = std::make_pair(s, -1);
    
    auto start = std::chrono::high_resolution_clock::now();

    State* st;
    while (!queue.empty()) {
        st = queue.front();
        queue.pop();
        set.erase(st);

        //std::cout << "G: " << st->get_g() << std::endl;

        if (st->get_g() >= maxSolutionLength - 1) {
            auto end = std::chrono::high_resolution_clock::now();
            std::vector<int> path = reconstruct_path3(st);
            
            std::cout << "Reorganization: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }

            path = reconstruct_path3(st, true);

            std::cout << "\n\nSolution: ";
            for(int i = 0; i < path.size(); i++){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
            //std::cout << "Queue size: " << queue.size() << std::endl;
            //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;

            delete s;
            s = new State(*st);
            s->g = 0;

            std::cout << "Final Configuration:" << std::endl;
            std::cout << print_board() << std::endl;
            
            for(auto it = visited_map.begin(); it != visited_map.end(); it++){
                delete it->first;
            }
            visited_map.clear();

            for(auto it = parents.begin(); it != parents.end(); it++){
                delete it->first;
            }
            parents.clear();

            Astar();

            return true;
        }

        auto moves = st->get_legal_moves_without_goal();
        //std::cout << "Moves count: " << moves.size() << std::endl;
        for (const auto& m : moves) {
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
        //std::cout << std::endl;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::vector<int> path = reconstruct_path3(st);
            
    std::cout << "Reorganization: ";
    for(int i = path.size()-1; i >= 0; i--){
        std::cout << print_direction(path[i]) << " ";
    }

    path = reconstruct_path3(st, true);

    std::cout << "\n\nSolution: ";
    for(int i = 0; i < path.size(); i++){
        std::cout << print_direction(path[i]) << " ";
    }
    std::cout << std::endl << std::endl;
    std::cout << "Length: " << path.size() << std::endl;
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
    //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
    //std::cout << "Queue size: " << queue.size() << std::endl;
    //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
    
    delete s;
    s = new State(*st);
    s->g = 0;

    std::cout << "Final configuration:" << std::endl;
    std::cout << print_board() << std::endl;
    
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();

    std::cout << std::endl;

    Astar();

    return true;
}

// Generates random puzzles until it finds one with a specific solution length
// maxSolutionLength: the length of the searched solution
// maxIterations: the maximum number of iterations that this algorithm will do to restart the generation before aborting
bool Game::generateRandomPuzzle2(int maxSolutionLength, int maxIterations) {

    int iterationCount = 0;

    while (true) {

        bool aStarMaxLengthReached = true;
                
        for(auto it = visited_map.begin(); it != visited_map.end(); it++){
            delete it->first;
        }
        visited_map.clear();

        for(auto it = parents.begin(); it != parents.end(); it++){
            delete it->first;
        }
        parents.clear();

        s->generateRandom3();

        std::unordered_set<State*> set;
        std::queue<State*> queue;

        State* c = new State(*s);

        visited_map[c] = std::make_pair(s, -1);
        
        auto moves = c->get_legal_moves_without_goal();
        for (const auto& m : moves) {
            if (c->get_piece(m.first)->get_piece_type() == SG_426A) {
                auto copy = new State(*c);
                copy->move(m);
                nodes++;
                visited_map[copy] = std::make_pair(c, m.second);
                
                if(set.count(copy) <= 0){
                    queue.push(copy);
                    set.insert(copy);
                }
                break;
            }
        }
        
        auto start = std::chrono::high_resolution_clock::now();

        State* st;
        while (!queue.empty()) {
            st = queue.front();
            queue.pop();
            set.erase(st);

            //std::cout << "G: " << st->get_g() << std::endl;

            if (st->get_g() >= maxSolutionLength - 1) {
                auto end = std::chrono::high_resolution_clock::now();
                std::vector<int> path = reconstruct_path3(st);
                
                std::cout << "Reorganization: ";
                for(int i = path.size()-1; i >= 0; i--){
                    std::cout << print_direction(path[i]) << " ";
                }

                path = reconstruct_path3(st, true);

                std::cout << "\n\nSolution: ";
                for(int i = 0; i < path.size(); i++){
                    std::cout << print_direction(path[i]) << " ";
                }
                std::cout << std::endl << std::endl;
                std::cout << "Length: " << path.size() << std::endl;
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
                //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
                //std::cout << "Queue size: " << queue.size() << std::endl;
                //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;

                delete s;
                s = new State(*st);
                s->g = 0;

                std::cout << "Final Configuration:" << std::endl;
                std::cout << print_board() << std::endl;
                
                for(auto it = visited_map.begin(); it != visited_map.end(); it++){
                    delete it->first;
                }
                visited_map.clear();

                for(auto it = parents.begin(); it != parents.end(); it++){
                    delete it->first;
                }
                parents.clear();

                Astar2();

                if (aStarPath.size() - 1 != maxSolutionLength - 1) {
                    std::cout << "Did not reach the max solution length. Resetting generation." << std::endl;

                    iterationCount++;

                    if (iterationCount >= maxIterations) {
                        std::cout << "Max iterations reached. Aborting proccess" << std::endl;
                        return false;
                    }

                    aStarMaxLengthReached = false;

                    break;
                }

                // std::ofstream puzzleFile("puzzles.txt", std::ios::out | std::ios::app);
                // if (puzzleFile.is_open()) {
                    
                //     puzzleFile << print_board() << "\n";

                //     puzzleFile << "Solution: ";
                //     for(int i = aStarPath.size()-1; i >= 0; i--){
                //         puzzleFile << print_direction(aStarPath[i]) << " ";
                //     }
                //     puzzleFile << "\n";
                //     puzzleFile << "Solution length: " << aStarPath.size() << "\n\n";

                //     puzzleFile.close();
                // }
                savePuzzleInFile(s, aStarPath);

                return true;
            }

            auto moves = st->get_legal_moves_without_goal();
            //std::cout << "Moves count: " << moves.size() << std::endl;
            for (const auto& m : moves) {
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
            //std::cout << std::endl;
        }

        if (!aStarMaxLengthReached) {
            continue;
        }

        if (st->get_g() != maxSolutionLength - 1) {
            std::cout << "Did not reach the max length. Resetting generation." << std::endl;

            iterationCount++;

            if (iterationCount >= maxIterations) {
                std::cout << "Max iterations reached. Aborting proccess" << std::endl;
                return false;
            }

            continue;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::vector<int> path = reconstruct_path3(st);
                
        std::cout << "Reorganization: ";
        for(int i = path.size()-1; i >= 0; i--){
            std::cout << print_direction(path[i]) << " ";
        }

        path = reconstruct_path3(st, true);

        std::cout << "\n\nSolution: ";
        for(int i = 0; i < path.size(); i++){
            std::cout << print_direction(path[i]) << " ";
        }
        std::cout << std::endl << std::endl;
        std::cout << "Length: " << path.size() << std::endl;
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
        //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
        //std::cout << "Queue size: " << queue.size() << std::endl;
        //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
        
        delete s;
        s = new State(*st);
        s->g = 0;

        std::cout << "Final configuration:" << std::endl;
        std::cout << print_board() << std::endl;
        
        for(auto it = visited_map.begin(); it != visited_map.end(); it++){
            delete it->first;
        }
        visited_map.clear();

        for(auto it = parents.begin(); it != parents.end(); it++){
            delete it->first;
        }
        parents.clear();

        std::cout << std::endl;

        Astar2();

        if (aStarPath.size() - 1 != maxSolutionLength - 1) {
            std::cout << "Did not reach the max solution length. Resetting generation." << std::endl;

            iterationCount++;

            if (iterationCount >= maxIterations) {
                std::cout << "Max iterations reached. Aborting proccess" << std::endl;
                return false;
            }

            continue;
        }

        break;
    }

    return true;
}

// Generates a random puzzle and explore their solution space
// maxSolutionLength: the length of the searched solution. If -1 then it will explore the whole solution space
bool Game::generateRandomPuzzle3(int maxSolutionLength, int maxIterations) {

    int iterationCount = 0;

    // while (true) {

        bool aStarMaxLengthReached = true;
                
        for(auto it = visited_map.begin(); it != visited_map.end(); it++){
            delete it->first;
        }
        visited_map.clear();

        for(auto it = parents.begin(); it != parents.end(); it++){
            delete it->first;
        }
        parents.clear();
        
        std::unordered_map<State*, std::pair<State*, int>> visited_map_generation;
        std::unordered_map<State*, int> aStarGs;

        s->generateRandom3();

        std::unordered_set<State*> set;
        std::queue<State*> queue;

        State* c = new State(*s);
        set.insert(c); 
        queue.push(c);

        visited_map_generation[c] = std::make_pair(s, -1);
        
        auto start = std::chrono::high_resolution_clock::now();

        State* st;
        State* maxSolutionLengthState = nullptr;
        while (!queue.empty()) {
            st = queue.front();
            queue.pop();
            set.erase(st);

            if (maxSolutionLengthState == nullptr) {
                maxSolutionLengthState = st;
            }
            else {
                if (aStarGs[maxSolutionLengthState] < aStarGs[st]) {
                    maxSolutionLengthState = st;

                    if (maxSolutionLength != -1) {
                        if (aStarGs[maxSolutionLengthState] >= maxSolutionLength) {
                            auto end = std::chrono::high_resolution_clock::now();
                            std::vector<int> path = reconstruct_path4(maxSolutionLengthState, false, visited_map_generation);
                            
                            std::cout << "Reorganization: ";
                            for(int i = path.size()-1; i >= 0; i--){
                                std::cout << print_direction(path[i]) << " ";
                            }

                            path = reconstruct_path4(maxSolutionLengthState, true, visited_map_generation);

                            std::cout << "\n\nSolution: ";
                            for(int i = 0; i < path.size(); i++){
                                std::cout << print_direction(path[i]) << " ";
                            }
                            std::cout << std::endl << std::endl;
                            std::cout << "Length: " << path.size() << std::endl;
                            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                            std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
                            //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
                            //std::cout << "Queue size: " << queue.size() << std::endl;
                            //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;

                            delete s;
                            s = new State(*maxSolutionLengthState);
                            s->g = 0;

                            std::cout << "Final Configuration:" << std::endl;
                            std::cout << print_board() << std::endl;
                            
                            for(auto it = visited_map.begin(); it != visited_map.end(); it++){
                                delete it->first;
                            }
                            visited_map.clear();

                            for(auto it = parents.begin(); it != parents.end(); it++){
                                delete it->first;
                            }
                            parents.clear();

                            Astar();

                            // if (aStarPath.size() - 1 != maxSolutionLength - 1) {
                            //     std::cout << "Did not reach the max solution length. Resetting generation." << std::endl;

                            //     iterationCount++;

                            //     if (iterationCount >= maxIterations) {
                            //         std::cout << "Max iterations reached. Aborting proccess" << std::endl;
                            //         return false;
                            //     }

                            //     aStarMaxLengthReached = false;

                            //     break;
                            // }

                            // std::ofstream puzzleFile("puzzles.txt", std::ios::out | std::ios::app);
                            // if (puzzleFile.is_open()) {
                                
                            //     puzzleFile << print_board() << "\n";

                            //     puzzleFile << "Solution: ";
                            //     for(int i = aStarPath.size()-1; i >= 0; i--){
                            //         puzzleFile << print_direction(aStarPath[i]) << " ";
                            //     }
                            //     puzzleFile << "\n";
                            //     puzzleFile << "Solution length: " << aStarPath.size() << "\n\n";

                            //     puzzleFile.close();
                            // }
                            
                            savePuzzleInFile(s, aStarPath);
                
                            for(auto it = visited_map_generation.begin(); it != visited_map_generation.end(); it++){
                                delete it->first;
                            }
                            visited_map_generation.clear();

                            return true;
                        }
                    }
                }
            }

            auto moves = st->get_legal_moves_without_goal();
            //std::cout << "Moves count: " << moves.size() << std::endl;
            for (const auto& m : moves) {
                auto copy = new State(*st);
                copy->move(m);

                if(visited_map_generation.find(copy) == visited_map_generation.end()){
                    visited_map_generation[copy] = std::make_pair(st, m.second);
                }
                else
                {   
                    delete copy;
                    continue;
                }

                std::cout << "Mid Configuration:" << std::endl;
                std::cout << copy->print_board() << std::endl;
                std::cout << copy->print_board2() << std::endl;
        
                for(auto it = visited_map.begin(); it != visited_map.end(); it++){
                    delete it->first;
                }
                visited_map.clear();

                for(auto it = parents.begin(); it != parents.end(); it++){
                    delete it->first;
                }
                parents.clear();

                Astar2(copy);
                aStarGs[copy] = aStarPath.size();
                //copy->g = aStarPath.size();
                
                std::cout << "Mid G:\t\t" << copy->get_g() + 1 << std::endl;
                std::cout << "Mid Astar g:\t" << aStarGs[copy] << std::endl;

                //if (copy->get_g() < maxSolutionLengthState->get_g()) continue;
                
                if(set.count(copy) <= 0){
                    queue.push(copy);
                    set.insert(copy);
                }
            }
            //std::cout << std::endl;
            std::this_thread::sleep_for (std::chrono::milliseconds(2));
        }

        // if (!aStarMaxLengthReached) {
        //     continue;
        // }

        // if (st->get_g() != maxSolutionLength - 1) {
        //     std::cout << "Did not reach the max length. Resetting generation." << std::endl;

        //     iterationCount++;

        //     if (iterationCount >= maxIterations) {
        //         std::cout << "Max iterations reached. Aborting proccess" << std::endl;
        //         return false;
        //     }

        //     continue;
        // }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::vector<int> path = reconstruct_path4(st, false, visited_map_generation);
                
        std::cout << "Reorganization: ";
        for(int i = path.size()-1; i >= 0; i--){
            std::cout << print_direction(path[i]) << " ";
        }

        path = reconstruct_path4(st, true, visited_map_generation);

        std::cout << "\n\nSolution: ";
        for(int i = 0; i < path.size(); i++){
            std::cout << print_direction(path[i]) << " ";
        }
        std::cout << std::endl << std::endl;
        std::cout << "Length: " << path.size() << std::endl;
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
        //std::cout << "Nodes evaluated (every iterations): " << nodes << std::endl;
        //std::cout << "Queue size: " << queue.size() << std::endl;
        //std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
        
        delete s;
        s = new State(*maxSolutionLengthState);
        s->g = 0;

        std::cout << "Final configuration:" << std::endl;
        std::cout << print_board() << std::endl;
        
        for(auto it = visited_map.begin(); it != visited_map.end(); it++){
            delete it->first;
        }
        visited_map.clear();

        for(auto it = parents.begin(); it != parents.end(); it++){
            delete it->first;
        }
        parents.clear();

        std::cout << std::endl;

        Astar();

        // if (aStarPath.size() - 1 != maxSolutionLength - 1) {
        //     std::cout << "Did not reach the max solution length. Resetting generation." << std::endl;

        //     iterationCount++;

        //     if (iterationCount >= maxIterations) {
        //         std::cout << "Max iterations reached. Aborting proccess" << std::endl;
        //         return false;
        //     }

        //     continue;
        // }

        savePuzzleInFile(s, aStarPath);
                
        for(auto it = visited_map_generation.begin(); it != visited_map_generation.end(); it++){
            delete it->first;
        }
        visited_map_generation.clear();

    //     break;
    // }

    return true;
}

// Search the solution space of a given puzzle from a file
// maxSolutionLength: the length of the searched solution. If -1 then it will explore the whole solution space
bool Game::generateRandomPuzzle4(int maxSolutionLength, int maxIterations) {
            
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();
    
    // std::unordered_map<State*, std::pair<State*, int>> visited_map_generation;
    std::unordered_map<State*, std::unordered_map<State*, int>> visited_undirected_map;
    std::unordered_set<State*> visited_set;

    std::unordered_set<State*> set;
    std::priority_queue<State*, std::vector<State*>, Comparator> queue;
    std::vector<State*> finalStates;

    State* c = new State(*s);
    c->h = 1;
    set.insert(c); 
    queue.push(c);

    // visited_map_generation[c] = std::make_pair(s, -1);
    visited_set.insert(c);
    finalStates.push_back(c);
    
    auto start = std::chrono::high_resolution_clock::now();

    // int rest = 0;

    State* st;
    State* maxSolutionLengthState = nullptr;
    while (!queue.empty()) {
        st = queue.top();
        queue.pop();
        set.erase(st);

        // if (maxSolutionLengthState == nullptr) {
        //     maxSolutionLengthState = st;
        // }
        // else {
        //     if (maxSolutionLengthState->get_h() < st->get_h()) {
        //         maxSolutionLengthState = st;

        //         if (maxSolutionLength != -1) {
        //             if (maxSolutionLengthState->get_h() >= maxSolutionLength) {
        //                 auto end = std::chrono::high_resolution_clock::now();

        //                 delete s;
        //                 s = new State(*maxSolutionLengthState);
        //                 s->g = 0;

        //                 std::cout << "Final Configuration:" << std::endl;
        //                 std::cout << print_board() << std::endl;
                        
        //                 for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        //                     delete it->first;
        //                 }
        //                 visited_map.clear();

        //                 for(auto it = parents.begin(); it != parents.end(); it++){
        //                     delete it->first;
        //                 }
        //                 parents.clear();

        //                 Astar();

        //                 for (auto it : visited_set) {
        //                     delete it;
        //                 }
        //                 visited_set.clear();

        //                 return true;
        //             }
        //         }
        //     }
        // }

        auto moves = st->get_legal_moves_without_goal();
        for (const auto& m : moves) {
            auto copy = new State(*st);
            copy->move(m);
            // copy->g = 0;
            
            if (maxSolutionLength == -1) {
                std::cout << "Parent Configuration:" << std::endl;
                std::cout << st->print_board() << std::endl;
                std::cout << st->print_board2() << std::endl;
                std::cout << "Mid Configuration:" << std::endl;
                std::cout << copy->print_board() << std::endl;
                std::cout << copy->print_board2() << std::endl;
                std::cout << "\n\n";
            }

            if(visited_set.count(copy) <= 0){
                visited_set.insert(copy);
            }
            else
            {
                auto it = *visited_set.find(copy);

                visited_undirected_map[st][it] = m.second;
                visited_undirected_map[it][st] = opposite((ADJACENCY_DIRECTIONS)m.second);

                delete copy;
                continue;
            }
            
            copy->h = 100000;

            if (copy->get_current_ship() == 7 && copy->can_slide(7, 10, UP)) {
                copy->h = 1;
                finalStates.push_back(copy);
            }

            visited_undirected_map[st][copy] = m.second;
            visited_undirected_map[copy][st] = opposite((ADJACENCY_DIRECTIONS)m.second);
            
            if(set.count(copy) <= 0){
                queue.push(copy);
                set.insert(copy);
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "\n\nTime BFS: " << diff/1000.0 << "s" << std::endl;
    
    std::priority_queue<State*, std::vector<State*>, Comparator3> queue3;
    std::queue<State*> queue4;
    std::unordered_map<State*, std::vector<int>> paths;

    for (auto& st : finalStates) {
        queue3.push(st);
        paths[st].push_back(DOWN);
    }

    std::vector<State*> maxSolutionLengthStates;

    while (!queue3.empty()) {
        auto st = queue3.top();
        queue3.pop();
        queue4.push(st);

        for (auto& st2 : visited_undirected_map[st]) {

            if (paths.find(st2.first) != paths.end()) {
                if (paths[st].size() + 1 < paths[st2.first].size()) {
                    st2.first->h = st->get_h() + 1;
                    paths[st2.first].clear();

                    for (auto& m : paths[st]) {
                        paths[st2.first].push_back(m);
                    }
                    paths[st2.first].push_back(visited_undirected_map[st2.first][st]);
                }
            }
            else {
                st2.first->h = st->get_h() + 1;

                if (maxSolutionLength != -1) {
                    if (st2.first->get_h() == maxSolutionLength) {
                        maxSolutionLengthStates.push_back(st2.first);
                    }
                }

                queue3.push(st2.first);

                for (auto& m : paths[st]) {
                    paths[st2.first].push_back(m);
                }
                paths[st2.first].push_back(visited_undirected_map[st2.first][st]);
            }
        }
    }

    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "\n\nTotal time: " << diff/1000.0 << "s" << std::endl;

    for (auto& st : maxSolutionLengthStates) {
        std::cout << st->print_pieces_compact() + "\n\n";

        std::cout << st->print_board2() << "\n";

        std::cout << "Solution: ";
        for (int i = paths[st].size() - 1; i >= 0; --i) {
            std::cout << print_direction(paths[st][i]) + " ";
        }
        std::cout << "\n";

        std::cout << "Solution length: " + std::to_string(st->get_h());
        std::cout << "\n\n";
    }

    // delete s;
    // s = new State(*maxSolutionLengthState);
    // s->g = 0;

    // std::cout << "Final configuration:" << std::endl;
    // std::cout << s->print_pieces_compact() << std::endl;
    // std::cout << print_board() << std::endl;
    
    // for(auto it = visited_map.begin(); it != visited_map.end(); it++){
    //     delete it->first;
    // }
    // visited_map.clear();

    // for(auto it = parents.begin(); it != parents.end(); it++){
    //     delete it->first;
    // }
    // parents.clear();

    // std::cout << std::endl;

    // Astar();

    // savePuzzleInFile(s, aStarPath);

    // for(auto it = visited_map_generation.begin(); it != visited_map_generation.end(); it++){
    //     delete it->first;
    // }
    // visited_map_generation.clear();

    for (auto& it : visited_set) {
        delete it;
    }
    visited_set.clear();

    return true;
}

// Solves the booklet puzzles and explore their search space
bool Game::generateRandomPuzzle5(std::string filename) {
            
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();
    
    std::unordered_map<State*, std::pair<State*, int>> visited_map_generation;
    //std::unordered_set<State*> visited_set;

    std::unordered_set<State*> set;
    std::priority_queue<State*, std::vector<State*>, Comparator2> queue;

    State* c = new State(*s);
    set.insert(c); 
    queue.push(c);

    visited_map_generation[c] = std::make_pair(s, -1);
    //visited_set.insert(c);
    
    auto start = std::chrono::high_resolution_clock::now();

    Astar2(s);
    savePuzzleInFile2(s, aStarPath, "Boards/solved/" + filename);

    State* st;
    State* maxSolutionLengthState = nullptr;
    while (!queue.empty()) {
        st = queue.top();
        queue.pop();
        set.erase(st);

        if (maxSolutionLengthState == nullptr) {
            maxSolutionLengthState = st;
        }
        else {
            if (maxSolutionLengthState->get_h() < st->get_h()) {
                maxSolutionLengthState = st;
            }
        }

        auto moves = st->get_legal_moves_without_goal();
        for (const auto& m : moves) {
            auto copy = new State(*st);
            copy->move(m);

            // if(visited_set.count(copy) <= 0){
            //     visited_set.insert(copy);
            // }
            if(visited_map_generation.find(copy) == visited_map_generation.end()){
                visited_map_generation[copy] = std::make_pair(st, m.second);
            }
            else
            {   
                delete copy;
                continue;
            }

            // if (st->get_current_empty() == 0 || st->get_current_empty() == 2 || st->get_current_empty() == 6 || st->get_current_empty() == 8) {
            //     copy->h = st->h + 1;
            // }
            // else {
                for(auto it = visited_map.begin(); it != visited_map.end(); it++){
                    delete it->first;
                }
                visited_map.clear();

                for(auto it = parents.begin(); it != parents.end(); it++){
                    delete it->first;
                }
                parents.clear();

                //std::vector<std::pair<State*, int>> path = Astar3(copy, visited_map_generation);
                Astar2(copy);
                copy->h = aStarPath.size();

                // for (int i = 0; i < path.size(); ++i) {
                //     //if(visited_map_generation.find(path[i + 1].first) == visited_map_generation.end()){
                //     if(visited_set.count(path[i].first) <= 0){
                //         //visited_map_generation[path[i + 1].first] = std::make_pair(path[i].first, path[i].second);
                        
                //         path[i].first->h = i + 1;

                //         visited_set.insert(path[i].first);
            
                //         if(set.count(path[i].first) <= 0){
                //             queue.push(path[i].first);
                //             set.insert(path[i].first);
                //         }
                //     }
                //     else
                //     {   
                //         //std::cout << "\t\tVISITADOOOOOOOOOOOOOOOOOO" << std::endl;
                //         delete path[i].first;
                //         continue;
                //     }
                // }

                savePuzzleInFile2(copy, aStarPath, "Boards/solved/" + filename);
            // }
            
            if(set.count(copy) <= 0){
                queue.push(copy);
                set.insert(copy);
            }
        }
        //std::this_thread::sleep_for (std::chrono::milliseconds(2));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::vector<int> path = reconstruct_path4(st, false, visited_map_generation);
            
    delete s;
    s = new State(*maxSolutionLengthState);
    s->h = 0;
    
    for(auto it = visited_map.begin(); it != visited_map.end(); it++){
        delete it->first;
    }
    visited_map.clear();

    for(auto it = parents.begin(); it != parents.end(); it++){
        delete it->first;
    }
    parents.clear();
            
    for(auto it = visited_map_generation.begin(); it != visited_map_generation.end(); it++){
        delete it->first;
    }
    visited_map_generation.clear();

    // for (auto it : visited_set) {
    //     delete it;
    // }
    // visited_set.clear();

    return true;
}

// Solves the booklet puzzles and explore their search space
bool Game::generateRandomPuzzle6(std::string filePath) {
    
    std::unordered_map<State*, std::unordered_map<State*, int>> visited_undirected_map;
    std::unordered_set<State*> visited_set;

    std::unordered_set<State*> set;
    std::queue<State*> queue;
    std::vector<State*> finalStates;

    State* c = new State(*s);
    set.insert(c); 
    queue.push(c);

    visited_set.insert(c);
    c->h = 1;
    finalStates.push_back(c);
    
    auto start = std::chrono::high_resolution_clock::now();
          
    while (!queue.empty()) {
        State* st = queue.front();
        queue.pop();
        set.erase(st);

        auto moves = st->get_legal_moves_without_goal();
        for (const auto& m : moves) {
            auto copy = new State(*st);
            copy->move(m);

            // std::cout << "Parent: \t" << st->print_pieces_type_compact() << std::endl << std::endl;
            // std::cout << "Mid: \t" << copy->print_pieces_type_compact() << std::endl;
            
            // std::ofstream logFile("log.txt", std::ios::out | std::ios::app);

            // if (logFile.is_open()) {
            //     logFile << "Mid: \t" << copy->print_pieces_type_compact() << copy->print_board2() << "\n" << "\n";

            //     logFile.close();
            // }

            if(visited_set.count(copy) <= 0){
                visited_set.insert(copy);
            }
            else {
                auto it = *visited_set.find(copy);

                visited_undirected_map[st][it] = m.second;
                visited_undirected_map[it][st] = opposite((ADJACENCY_DIRECTIONS)m.second);

                delete copy;
                continue;
            }
            
            copy->h = 100000;

            if (copy->get_current_ship() == 7 && copy->can_slide(7, 10, UP)) {
                copy->h = 1;
                finalStates.push_back(copy);
            }

            visited_undirected_map[st][copy] = m.second;
            visited_undirected_map[copy][st] = opposite((ADJACENCY_DIRECTIONS)m.second);

            if(set.count(copy) <= 0){
                queue.push(copy);
                set.insert(copy);
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "\n\nTime BFS: " << diff/1000.0 << "s" << std::endl;

    // for (auto& it : visited_undirected_map) 
    // {
    //     std::ofstream logFile("log.txt", std::ios::out | std::ios::app);

    //     if (logFile.is_open()) {
    //         logFile << "\n Adjacency list of vertex "<< it.first->print_pieces_type_compact() << "\n head "; 
    //         for (auto x : it.second) 
    //             logFile << "-> " << x.first->print_pieces_type_compact(); 
    //         logFile << "\n";

    //         logFile.close();
    //     }
    // }
    
    std::priority_queue<State*, std::vector<State*>, Comparator3> queue3;
    std::queue<State*> queue4;
    std::unordered_map<State*, std::vector<int>> paths;

    for (auto& st : finalStates) {
        queue3.push(st);
        paths[st].push_back(DOWN);
    }

    while (!queue3.empty()) {
        auto st = queue3.top();
        queue3.pop();
        queue4.push(st);

        for (auto& st2 : visited_undirected_map[st]) {

            if (paths.find(st2.first) != paths.end()) {
                if (paths[st].size() + 1 < paths[st2.first].size()) {
                    st2.first->h = st->get_h() + 1;
                    paths[st2.first].clear();

                    for (auto& m : paths[st]) {
                        paths[st2.first].push_back(m);
                    }
                    paths[st2.first].push_back(visited_undirected_map[st2.first][st]);
                }
            }
            else {
                st2.first->h = st->get_h() + 1;
                queue3.push(st2.first);

                for (auto& m : paths[st]) {
                    paths[st2.first].push_back(m);
                }
                paths[st2.first].push_back(visited_undirected_map[st2.first][st]);
            }
        }
    }

    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "\n\nTotal time: " << diff/1000.0 << "s" << std::endl;

    std::string fileContent = "";
    while (!queue4.empty()) {
        auto st = queue4.front();
        queue4.pop();

        fileContent += st->print_pieces_compact() + "\n\n";

        fileContent += "Solution: ";
        for (int i = paths[st].size() - 1; i >= 0; --i) {
            fileContent += print_direction(paths[st][i]) + " ";
        }
        fileContent += "\n";

        fileContent += "Solution length: " + std::to_string(st->get_h());
        // std::cout << st->get_h() << std::endl;
        fileContent += "\n\n";

        // savePuzzleInFile2(st, paths[st], filePath);
    }

    std::ofstream file(filePath);

    file << fileContent;

    file.close();
            
    for(auto& it : visited_set){
        delete it;
    }
    visited_set.clear();

    return true;
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
            aStarPath = reconstruct_path(st);

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

bool Game::Astar2(State** saveState)
{
    std::cout << "Running A*..." << std::endl;


    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open;

    std::unordered_set<State*> closed;

    nodes = 0;

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
            aStarPath = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            // std::cout << "Closed list nodes: " << closed.size() << std::endl;
            // auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            // std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            // std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
            // std::cout << "All iterations: " << nodes << std::endl;

            //delete st;
            //delete copy;

            if (saveState) {
                delete *saveState;
                *saveState = new State(*(visited_map[st].first));
                (*saveState)->g = 0;
            }

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

bool Game::Astar2(State* state)
{
    std::cout << "Running A*..." << std::endl;


    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open;

    std::unordered_set<State*> closed;

    nodes = 0;

    auto c = state == nullptr ? new State(*s) : new State(*state);
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
            aStarPath = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            // std::cout << "Closed list nodes: " << closed.size() << std::endl;
            // auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            // std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            // std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
            // std::cout << "All iterations: " << nodes << std::endl;

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

std::vector<std::pair<State*, int>> Game::Astar3(State* state, std::unordered_map<State*, std::pair<State*, int>>& visited_map_generation)
{
    std::cout << "Running A*..." << std::endl;


    std::unordered_set<State*> open_s;
    std::priority_queue<State*, std::vector<State*>, Comparator> open;

    std::unordered_set<State*> closed;

    nodes = 0;

    auto c = state == nullptr ? new State(*s) : new State(*state);
    open_s.insert(c);
    open.push(c);
    visited_map[c] = std::make_pair(s, -1);
    auto start = std::chrono::high_resolution_clock::now();
    while(!open.empty())
    {
        auto st = open.top();
        open_s.erase(st);
        open.pop();

        // if (st != c) {
        //     if (visited_map_generation.find(st) != visited_map_generation.end()) {
        //         std::vector<std::pair<State*, int>> path = reconstruct_path_states(st);
                
        //     }
        // }

        //std::cout << st->print_board() << std::endl;
        if(st->is_goal()){
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Goal found!" << std::endl;
            std::vector<int> path = reconstruct_path(st);
            aStarPath = reconstruct_path(st);

            std::cout << "Solution: ";
            for(int i = path.size()-1; i >= 0; i--){
                std::cout << print_direction(path[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Length: " << path.size() << std::endl;
            // std::cout << "Closed list nodes: " << closed.size() << std::endl;
            // auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            // std::cout << "Time: " << diff/1000.0 << "s" << std::endl;
            // std::cout << "Nodes per second: " << (double)nodes/(diff/1000.0) << std::endl;
            // std::cout << "All iterations: " << nodes << std::endl;

            //delete st;
            //delete copy;
            
            // std::vector<std::pair<State*, int>> pathStates = reconstruct_path_states(st);

            // for (int i = 1; i < path.size(); ++i) {
            //     if (visited_map_generation.find(pathStates[i].first) == visited_map_generation.end()) {
            //         visited_map_generation;
            //         pathStates[i].first->g = i;
            //     }
            //     else {
            //         delete pathStates[i].first;
            //         continue;
            //     }
            // }

            return reconstruct_path_states(st);
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
    return std::vector<std::pair<State*, int>>();
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