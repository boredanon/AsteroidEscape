#include "State.h"

void State::init()
{
    pieces = new Piece*[9];
    for(int i = 0; i < 9; i++){
        pieces[i] = new Piece();
    }
}

Piece* State::get_piece(int index){
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

bool State::is_legal() const
{
    for(int i = 0; i < 9; i++)
    {
        uint64_t context=pieces[i]->get_p();
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
                    collision_mask = ((piece&(BOTTOM_ROW&RIGHT_COLUMN)) << 36);
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
                return false;
            }
        }
    }
    return true;
}

std::string State::print_adjacencies() const
{
    std::string s;
    for(int i = 0; i < 9; i++)
    {
        auto p = ADJACENCY[i];
        s+="Piece " + std::to_string(i) + " ";
        while (p)
        {
            auto c = 8 - __builtin_ffs(p);
            uint8_t d = 0x80 >> c;
            s+="adjacency with piece ";
            switch(d){
                case 1:
                    s+=std::to_string(i+4);
                    break;
                case 2:
                    s+=std::to_string(i+3);
                    break;
                case 4:
                    s+=std::to_string(i+2);
                    break;
                case 8:
                    s+=std::to_string(i+1);
                    break;
                case 16:
                    s+=std::to_string(i-1);
                    break;
                case 32:
                    s+=std::to_string(i-2);
                    break;
                case 64:
                    s+=std::to_string(i-3);
                    break;
                case 128:
                    s+=std::to_string(i-4);
                    break;
            }
            p^=d;
            s+=" ";
        }
        s+="\n";
    }
    return s;
}

State::~State()
{
    for(int i = 0; i < 9; i++){
        delete pieces[i];
    }
    delete [] pieces;
    std::cout << "Delete state." << std::endl;
}