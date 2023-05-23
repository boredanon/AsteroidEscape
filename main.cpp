#include <iostream>

#include "Piece.h"

int main()
{
    Piece p(SG_426A, CANONICAL);

    std::cout << p.print_piece() << std::endl;

    return 0;
}