#include "State.h"

State::State()
{
    pieces = new Piece*[9];
    for(int i = 0; i < 9; i++){
        pieces[i] = new Piece();
    }
}

State::State(std::string ps, std::string orientations)
{
    pieces = new Piece*[9];
    current_empty=-1;
    for(unsigned int i = 0; i < 9; i++)
    {
        std::pair<int,int> piece;
        switch(ps[i])
        {
            case '-':
                piece.first = (int)EMPTY;
                current_empty=4;
            break;
            case 'A':
                piece.first = (int)SG_426A;
            break;
            case 'B':
                piece.first = (int)SG_426B;
            break;
            case 'C':
                piece.first = (int)SG_426C;
            break;
            case 'D':
                piece.first = (int)SG_426DEF;
            break;
            case 'G':
                piece.first = (int)SG_426G;
            break;
            case 'H':
                piece.first = (int)SG_426H;
            break;
        }

        switch(orientations[i])
        {
            case '0':
                piece.second = (int)CANONICAL;
            break;
            case '1':
                piece.second = (int)NINETY;
            break;
            case '2':
                piece.second = (int)ONE_EIGHTY;
            break;
            case '3':
                piece.second = (int)TWO_SEVENTY;
            break;
        }

        pieces[i] = new Piece((PIECE_TYPE)piece.first, (ORIENTATION)piece.second);
    }

    if(!is_legal()){

        for(int i = 0; i < 9; i++){
            delete pieces[i];
        }
        delete [] pieces;
        throw std::runtime_error("Illegal board.");
    }
}

Piece* State::get_empty() const{
    return pieces[current_empty];
}

Piece* State::get_piece(int index) const{
    return pieces[index];
}

void State::set_piece(int index, PIECE_TYPE pt, ORIENTATION po)
{
    pieces[index]->set_piece(pt, po); 
}

std::string State::print_pieces() const
{
    std::string s;
    for(int i = 0; i < 9; i++)
    {
        s+=pieces[i]->print_piece();
        
        s+="\n\n";
    }
    return s;
}

std::string State::print_board() const
{
    std::string s;

    for(int i = 0; i < 9; i++)
    {
        s+=pieces[i]->get_type() + " | ";
        if(i==2 || i==5 || i == 8){
            s+="\n";
        }
    }

    return s;
}

std::vector<std::pair<int, int>> State::get_adjacencies(int i) const
{
    std::vector<std::pair<int,int>> a;
    auto p = ADJACENCY[i];
    while(p)
    {
        auto c = 8 - __builtin_ffs(p);
        uint8_t d = 0x80 >> c;
        switch(d)
        {
            case 1:
                a.push_back(std::make_pair(i+4, (int)DOWN_RIGHT));
            break;
            case 2:
                a.push_back(std::make_pair(i+3, (int)DOWN));
            break;
            case 4:
                a.push_back(std::make_pair(i+2, (int)DOWN_LEFT));
            break;
            case 8:
                a.push_back(std::make_pair(i+1, (int)RIGHT));
            break;
            case 16:
                a.push_back(std::make_pair(i-1, (int)LEFT));
            break;
            case 32:
                a.push_back(std::make_pair(i-2, (int)UP_RIGHT));
            break;
            case 64:
                a.push_back(std::make_pair(i-3, (int)UP));
            break;
            case 128:
                a.push_back(std::make_pair(i-4, (int)UP_LEFT));
            break;
        }
        p^=d;
    }

    return a;
}

bool State::check_collision(int i, uint64_t context) const
{
    uint64_t collision_mask=0;
    auto adj = get_adjacencies(i);

    for(const auto& a : adj)
    {
        auto piece = pieces[a.first]->get_p();
        ADJACENCY_DIRECTIONS dir = (ADJACENCY_DIRECTIONS)a.second;


        switch(dir)
        {
            case UP:
                collision_mask = ((piece&BOTTOM_ROW) << 32);
            break;
            case DOWN:
                collision_mask = ((piece&~BOTTOM_ROW) >> 32);
            break;
            case LEFT:
                collision_mask = ((piece&RIGHT_COLUMN) << 4);
            break;
            case RIGHT:
                collision_mask = ((piece&~RIGHT_COLUMN) >> 4);
            break;
            case UP_LEFT:
                collision_mask = ((piece&~(~BOTTOM_ROW|~RIGHT_COLUMN)) << 36);
            break;
            case DOWN_LEFT:
                collision_mask = ((piece&~(BOTTOM_ROW|~RIGHT_COLUMN)) >> 28);
            break;
            case UP_RIGHT:
                collision_mask = ((piece&~(~BOTTOM_ROW|RIGHT_COLUMN)) << 28);
            break;
            case DOWN_RIGHT:
                collision_mask = ((piece&~(BOTTOM_ROW|RIGHT_COLUMN)) >> 36);
            break;
        }

        if(collision_mask&context){
            return true;
        }
    }
    return false;
}

bool State::is_legal() const
{
    if(current_empty==-1){
        return false;
    }

    for(int i = 0; i < 9; i++)
    {
        uint64_t context=pieces[i]->get_p();
        if(check_collision(i, context)){
            return false;
        }

    }

    return true;
}

std::vector<std::pair<int,int>> State::get_legal_moves() const
{
    std::vector<std::pair<int,int>> v;
    std::cout << "Current empty piece is: " << current_empty << std::endl;
    auto adj = get_adjacencies(current_empty);

    for(const auto& a : adj)
    {
        if(a.second != 1){ // Diagonal moves are not allowed.
            continue;
        }
        //UP,DOWN,RIGHT,LEFT

        auto p_adj = get_adjacencies(a.first);
        auto p_bits = pieces[a.first]->get_p();
        auto e_bits = pieces[current_empty]->get_p(); // 0

        switch(a.second)
        {
            case UP:
                e_bits = (p_bits&BOTTOM_ROW)<<32;
            break;
            case DOWN:
                e_bits = (p_bits&~BOTTOM_ROW) >> 32;
            break;
            case RIGHT:
            break;
            case LEFT:
            break;
        }




        std::cout << Piece::print_as_piece(p_bits) << std::endl;
        for(int i = 0; i < 4; i++){
            std::cout << "Piece:" << a.first << ",";
           // bool move_p = __builtin_popcountll(e_bits) != __builtin_popcountll(pieces[a.first]->get_p());
            //bool move_p = (p_bits&INSIDE);
            bool move_p = false;
            switch(a.second)
            {
                case UP: // Means it moves DOWN.
                    std::cout << "UP" << std::endl;
                    p_bits = (p_bits&~SLIDING_LIMITS[DOWN]) >> 8;
                    e_bits = ((p_bits&BOTTOM_ROW)<<32)|((e_bits&~SLIDING_LIMITS[DOWN]) >> 8);

                break;
                case DOWN: // Means it moves UP.
                    std::cout << "DOWN" << std::endl;
                    p_bits = (p_bits&~SLIDING_LIMITS[UP]) << 8;
                    //if(move_p){
                        e_bits = (((p_bits&~BOTTOM_ROW) >> 32)|((e_bits&~SLIDING_LIMITS[UP]) << 8));
                    //}
                    //else
                    //{
                    //    e_bits = (e_bits&~SLIDING_LIMITS[UP]) << 8;
                    //}
                break;
                case RIGHT: // Means it moves LEFT.
                    std::cout << "RIGHT" << std::endl;
                    p_bits = (p_bits&~SLIDING_LIMITS[LEFT]) << 1;
                    if(move_p){
                        e_bits = (p_bits&~RIGHT_COLUMN) >> 4;
                    }
                    else
                    {
                        e_bits = (e_bits&~SLIDING_LIMITS[LEFT]) << 1;
                    }


                break;
                case LEFT: //Means it moves RIGHT.
                    std::cout << "LEFT" << std::endl;
                    p_bits = (p_bits&~SLIDING_LIMITS[RIGHT]) >> 1;
                    if(move_p){
                        e_bits = (p_bits&RIGHT_COLUMN) << 4;
                    }
                    else
                    {
                        e_bits = (e_bits&~SLIDING_LIMITS[RIGHT]) >> 1;
                    }
                break;

            }
            std::cout << Piece::print_as_piece(p_bits) << std::endl;
            std::cout << Piece::print_as_piece(e_bits) << std::endl;
        }
        std::cout << "-----" << std::endl;
    }
    return v;
}


State::~State()
{
    for(int i = 0; i < 9; i++){
        delete pieces[i];
    }
    delete [] pieces;
    std::cout << "Delete state." << std::endl;
}