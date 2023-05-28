#include <iostream>

#include "State.h"

int main()
{
    State* s = nullptr;
    try
    {
        //0 1 2 3 4 5 6 7 8
        s = new State("DDDDDDA-D", "000000000");
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }

    delete s;
    return 0;
}