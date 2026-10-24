#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include "Game.h"

static void print_usage(const char* program)
{
    std::cerr <<
        "Usage: " << program << " <board_file> <algorithm> [heuristic] [LOOKBACK]\n"
        "       " << program << " --batch <boards_dir> <heuristic> <output.csv>\n"
        "  algorithm: BFS | ASTAR | IDASTAR | ALLSTAR | ALLPATHS | HUMAN | MUSE | REMUSE\n"
        "  heuristic: ZERO (default) | TWOONE | OBSTACLES0 | OBSTACLES1 | DISTANCE | ROWDISTANCE\n"
        "  LOOKBACK:  allow a piece to undo the previous move (off by default)\n"
        "  --batch:   solve every board under <boards_dir> with A* and write one CSV row of metrics per board\n"
        "Example: " << program << " Boards/starter/starter1 ASTAR DISTANCE\n";
}

static std::string to_upper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

static bool parse_heuristic(const std::string& name, HEURISTIC& out)
{
    std::string h = to_upper(name);
    if(h == "ZERO") out = ZERO;
    else if(h == "TWOONE") out = TWOONE;
    else if(h == "OBSTACLES0") out = OBSTACLES0;
    else if(h == "OBSTACLES1") out = OBSTACLES1;
    else if(h == "DISTANCE") out = DISTANCE;
    else if(h == "ROWDISTANCE") out = ROWDISTANCE;
    else return false;
    return true;
}

// ===== INSTRUMENTATION: batch mode =====
// Solves every board file under <dir> (recursively, in alphabetical order) with A* and writes one
// CSV row (';'-separated) of search, solution-path and structural metrics per board.
static int run_batch(const std::string& dir, const std::string& out_path)
{
    namespace fs = std::filesystem;
    if(!fs::is_directory(dir)){
        std::cerr << "Not a directory: " << dir << std::endl;
        return -1;
    }
    std::vector<fs::path> files;
    for(const auto& e : fs::recursive_directory_iterator(dir))
        if(e.is_regular_file()) files.push_back(e.path());
    std::sort(files.begin(), files.end());

    std::ofstream out(out_path);
    if(!out){
        std::cerr << "Cannot write: " << out_path << std::endl;
        return -1;
    }
    out << Game::metrics_csv_header() << "\n";
    std::ostringstream sink;
    for(const auto& f : files){
        std::ifstream in(f);
        std::string positions, orientations;
        if(!std::getline(in, positions) || !std::getline(in, orientations)){
            std::cerr << "Skipped (not a board): " << f << std::endl;
            continue;
        }
        try{
            Game g(positions, orientations);
            auto old = std::cout.rdbuf(sink.rdbuf());    // silence solver output
            bool ok = g.Astar();
            std::cout.rdbuf(old);
            sink.str("");
            if(ok) out << g.metrics_csv_row(f.filename().string()) << std::endl;
            else std::cerr << "No solution: " << f << std::endl;
        }
        catch(std::exception& e){
            std::cerr << "Skipped " << f << ": " << e.what() << std::endl;
            continue;
        }
        std::cerr << "done " << f.filename().string() << std::endl;
    }
    return 0;
}
// =======================================

int main(int argc, char** argv)
{
    if(argc >= 2 && std::string(argv[1]) == "--batch"){
        if(argc < 5){
            std::cerr << "Not enough arguments for --batch." << std::endl;
            print_usage(argv[0]);
            return -1;
        }
        if(!parse_heuristic(argv[3], heuristic)){
            std::cerr << "Unknown heuristic: " << argv[3] << std::endl;
            print_usage(argv[0]);
            return -1;
        }
        return run_batch(argv[2], argv[4]);
    }

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
    if(argc > 3 && !parse_heuristic(argv[3], heuristic)){
        std::cerr << "Unknown heuristic: " << argv[3] << std::endl;
        print_usage(argv[0]);
        return -1;
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
        if(g->Astar()) std::cout << g->metrics_report();   // INSTRUMENTATION: all metrics of this board
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
