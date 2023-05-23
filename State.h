#ifndef STATE_H_
#define STATE_H_

#include "Piece.h"

class State
{
    private:
        Piece* pieces;
    
    public:
        void init();

        ~State();

};

#endif /*STATE_H_*/