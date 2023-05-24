#include <iostream>

#include "State.h"

int main()
{
    State* s = new State();
    s->init();
    s->set_piece(4, SG_426A, CANONICAL);
    s->set_piece(1, SG_426DEF, CANONICAL);
    s->set_piece(2, SG_426C, ONE_EIGHTY);

    std::cout << s->print_pieces() << std::endl;
    std::cout << s->print_board() << std::endl;
    
    auto test = s->is_legal();

    if(test)
    {
        std::cout << "Legal!" << std::endl;
    }
    else
    {
        std::cout << "Illegal..." << std::endl;
    }
    
    delete s;
    return 0;
}