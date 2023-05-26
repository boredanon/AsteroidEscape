#include <iostream>

#include "State.h"

int main()
{
    State* s = nullptr;
    try
    {
        //0 1 2 3 4 5 6 7 8
        s = new State("DADB-DDDD", "100000000");
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::cout << s->print_pieces() << std::endl;
    std::cout << s->print_board() << std::endl;
    auto moves = s->get_legal_moves();

    s->move(moves[0]);

    std::cout << s->print_board() << std::endl;


    delete s;
    return 0;
}