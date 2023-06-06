#include "State.h"

State::State()
{
    pieces = new Piece*[12];
    g=0;
    for(int i = 0; i < 12; i++){
        pieces[i] = new Piece();
    }
}

State::State(std::string ps, std::string orientations)
{
    pieces = new Piece*[12];
    current_empty=-1;
    int empty_amounts = 0;
    g=0;

    int i = 0;
    for(i = 0; i < 9; i++)
    {
        std::pair<int,int> piece;
        switch(ps[i])
        {
            case '-':
                piece.first = (int)EMPTY;
                current_empty=i;
                empty_amounts++;
            break;
            case 'A':
                piece.first = (int)SG_426A;
                current_ship=i;
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

    for(; i < 12; i++){
        pieces[i] = new Piece(EMPTY, CANONICAL); //Goal piece is Piece[10].
    }

    if(!is_legal() || empty_amounts>1 || empty_amounts == 0){

        for(int i = 0; i <  12; i++){
            delete pieces[i];
        }
        delete [] pieces;
        throw std::runtime_error("Illegal board.");
    }

    h = calculateH();
}

int State::calculateH(){
    int calculated_h = 0;

    if(heuristic==ZERO){
        return calculated_h;
    }
    //return 0;
    // Solved board doesn't need calculations.
    if(pieces[GOAL_POSITION]->get_piece_type() == SG_426A)
    {
        return 0;
    }

    //If it is on the correct column, it is AT LEAST 1 move to reach goal.
    if(current_ship%3 == 1){
        calculated_h+=1;
    }else{ // If not it is at least 2 moves to reach goal.
        calculated_h+=2;
    }

    if(current_ship/3 == 2){
        calculated_h+=1;
    }
    else{
        calculated_h+=2;
    }

    if(heuristic==TWOONE){
        return calculated_h;
    }

    auto adj = get_adjacencies(GOAL_POSITION);
    for(const auto& a : adj)
    {
        if(a.second > 3 || !pieces[a.first]->get_p()){
            continue;
        }

        if(pieces[a.first]->get_piece_type() != SG_426A){
            calculated_h+=1;
        }
    }

    if(heuristic == OBSTACLES0){
        return calculated_h;
    }

    for(const auto& a : adj)
    {
        if(a.second > 3 || !pieces[a.first]->get_p()){
            continue;
        }

        auto adj_2 = get_adjacencies(a.first);
        for(const auto& a_2 : adj_2)
        {
            if(a_2.first == a.first || a_2.second > 3 || !pieces[a_2.first]->get_p()){
                continue;
            }

            if(pieces[a_2.first]->get_piece_type() != SG_426A){
                calculated_h += 1;
            }
        }
    }


    return calculated_h;
}

State::State(const State& s)
{
    pieces = new Piece*[12];
    g = s.g;
    h = s.h;
    current_empty = s.current_empty;
    current_ship = s.current_ship;
    for(int i = 0; i < 12; i++)
    {
        pieces[i] = new Piece(s.pieces[i]->get_piece_type(), s.pieces[i]->get_piece_orientation());
    }
}

int State::get_f() const
{
    return g + h;
}

int State::get_h() const // Heuristic
{
    return h;
}

int State::get_g() const
{
    return g;
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

std::string State::print_board2() const
{
    std::string full_board;
    std::string context_pieces[9];
    for(int p = 0; p < 9; p++)
    {
        auto piece = pieces[p]->get_p();
        auto piece_adjacencies = get_adjacencies(p);

        for(const auto& p_a : piece_adjacencies)
        {
            auto piece_a = pieces[p_a.first]->get_p();
            switch(p_a.second)
            {
                case UP:
                    piece |= ((piece_a&BOTTOM_ROW) << 32);
                    break;
                case DOWN:
                    piece |= ((piece_a&~BOTTOM_ROW) >> 32);
                    break;
                case LEFT:
                    piece |= ((piece_a&RIGHT_COLUMN) << 4);
                    break;
                case RIGHT:
                    piece |= ((piece_a&~RIGHT_COLUMN) >> 4);
                    break;
                case UP_LEFT:
                    piece |= ((piece_a&~(~BOTTOM_ROW|~RIGHT_COLUMN)) << 36);
                    break;
                case DOWN_LEFT:
                    piece |= ((piece_a&~(BOTTOM_ROW|~RIGHT_COLUMN)) >> 28);
                    break;
                case UP_RIGHT:
                    piece |= ((piece_a&~(~BOTTOM_ROW|RIGHT_COLUMN)) << 28);
                    break;
                case DOWN_RIGHT:
                    piece |= ((piece_a&~(BOTTOM_ROW|RIGHT_COLUMN)) >> 36);
                    break;
            }
        }
        context_pieces[p] = get_binary64(piece);
    }

    auto i = get_binary64(INSIDE);

    for(int x = 0; x < 9; x++){
        for(unsigned int y = 0; y < context_pieces[x].size(); y++){
            if(context_pieces[x][y] == '0'){
                if(i[y] == '1'){
                    context_pieces[x][y] = '.';
                }
                if(i[y] == '0'){
                    context_pieces[x][y] = ' ';
                }
            }
        }
    }


    auto size=4*HEIGHT+4;
    std::string b[size];
    std::string c[size];
    int horizontal_offsets[3] = {0, 2, 4};
    int vertical_offsets[3] = {0, 16, 32};
    
    for(int j = 0; j < size; j++){
        for(int i = 0; i < size;i++){

            auto row = j/(8 - 2);
            auto col = i/(8 - 2);

            auto piece_index = col + row*3;
            auto string_index=(((j%(8-2)*8) + vertical_offsets[piece_index/HEIGHT]) + i%(8-2) + horizontal_offsets[piece_index%WIDTH]);

            c[j]+=std::to_string(piece_index) + "(" + std::to_string(string_index) + ") ";
            
            b[j] += context_pieces[piece_index][string_index];

        }
        full_board+=b[j] + "\n";
    }


    for(int d = 0; d < size; d++){
        std::cout << c[d] + " " << std::endl;
    }
    return full_board;
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
                a.push_back(std::make_pair(i+WIDTH+1, (int)DOWN_RIGHT));
            break;
            case 2:
                a.push_back(std::make_pair(i+WIDTH, (int)DOWN));
            break;
            case 4:
                a.push_back(std::make_pair(i+WIDTH-1, (int)DOWN_LEFT));
            break;
            case 8:
                a.push_back(std::make_pair(i+1, (int)RIGHT));
            break;
            case 16:
                a.push_back(std::make_pair(i-1, (int)LEFT));
            break;
            case 32:
                a.push_back(std::make_pair(i-WIDTH+1, (int)UP_RIGHT));
            break;
            case 64:
                a.push_back(std::make_pair(i-WIDTH, (int)UP));
            break;
            case 128:
                a.push_back(std::make_pair(i-WIDTH-1, (int)UP_LEFT));
            break;
        }
        p^=d;
    }

    return a;
}

void State::move(std::pair<int, int> m)
{
    auto dir = m.second;
    int target=-1;
    switch(dir)
    {
        case UP:
            target=m.first-3;
        break;
        case DOWN:
            target=m.first+3;
        break;
        case RIGHT:
            target=m.first+1;
        break;
        case LEFT:
            target=m.first-1;
        break;
    }

    PIECE_TYPE new_type = pieces[m.first]->get_piece_type();
    ORIENTATION new_o = pieces[m.first]->get_piece_orientation();
    
    if(new_type == SG_426A){
        current_ship = target;
    }

    pieces[target]->set_piece(new_type, new_o);
    pieces[m.first]->set_piece(EMPTY, CANONICAL);
    current_empty=m.first;
    g++;
    h=calculateH();
}

bool State::check_collision(int i, uint64_t context, int ignore) const
{
    uint64_t collision_mask=0;
    auto adj = get_adjacencies(i);

    for(const auto& a : adj)
    {
        if(a.first == ignore){
            continue;
        }

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
            /*std::cout << "Piece " << i << " is colliding with piece " << a.first << " in direction " << a.second << std::endl;

            std::cout << Piece::print_as_piece(context) << std::endl;
            std::cout << Piece::print_as_piece(collision_mask) << std::endl;

            std::cout << "Collision:\n" << Piece::print_as_piece(collision_mask|context) << std::endl;*/

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
        if(check_collision(i, context, -1)){
            return false;
        }

    }

    return true;
}

bool State::can_slide(int from, int to, int direction) const
{
    auto p_bits = pieces[from]->get_p();
    uint64_t e_bits=0;
    //Check for overlapping pieces, otherwise they appear "cut".
    switch(direction)
    {
        case UP:
            e_bits = (p_bits&BOTTOM_ROW)<<32;
        break;
        case DOWN:
            e_bits = (p_bits&~BOTTOM_ROW) >> 32;
        break;
        case RIGHT:
            e_bits = (p_bits&~RIGHT_COLUMN) >> 4;
        break;
        case LEFT:
            e_bits = (p_bits&RIGHT_COLUMN) << 4;
        break;
    }

    for(int index = 0; index < 4; index++){ // We need to emulate the slide.
        switch(direction)
        {
            case UP: // Means the piece is moving DOWN.
                p_bits = (p_bits&~SLIDING_LIMITS[DOWN]) >> 8;
                e_bits = ((p_bits&BOTTOM_ROW)<<32)|((e_bits&~SLIDING_LIMITS[DOWN]) >> 8);
            break;
            case DOWN: // Means the piece is moving UP.
                p_bits = (p_bits&~SLIDING_LIMITS[UP]) << 8;
                e_bits = (((p_bits&~BOTTOM_ROW) >> 32)|((e_bits&~SLIDING_LIMITS[UP]) << 8));
            break;
            case RIGHT: // Means the piece is moving LEFT.
                p_bits = (p_bits&~SLIDING_LIMITS[LEFT]) << 1;
                e_bits = (((p_bits&~RIGHT_COLUMN) >> 4)|(e_bits&~SLIDING_LIMITS[LEFT]) << 1);
            break;
            case LEFT: //Means the piece is moving RIGHT.
                p_bits = (p_bits&~SLIDING_LIMITS[RIGHT]) >> 1;
                e_bits = ((p_bits&RIGHT_COLUMN) << 4)|((e_bits&~SLIDING_LIMITS[RIGHT]) >> 1);
            break;

        }
        bool collision_1 = check_collision(from, p_bits, to);
        bool collision_2 = check_collision(to, e_bits, from);
        if(collision_1 || collision_2){
            //std::cout << "pieces " << a.first << ", " << current_empty << " found collision in direction " << a.second <<  std::endl;
            return false;
        }
        //std::cout << Piece::print_as_piece(p_bits) << std::endl;
        //std::cout << Piece::print_as_piece(e_bits) << std::endl;
    }
    return true;
}

std::vector<std::pair<int,int>> State::get_legal_moves() const
{
    std::vector<std::pair<int,int>> v;
    //std::cout << "Current empty piece is: " << current_empty << std::endl;

    //Movements.
    auto adj = get_adjacencies(current_empty);
    for(const auto& a : adj){
        if(a.second > 3 || !pieces[a.first]->get_p()){
            continue;
        }

        if(can_slide(a.first, current_empty, a.second)){
            v.push_back(std::make_pair(a.first, opposite((ADJACENCY_DIRECTIONS)a.second)));
        }
    }

    //This checks if the spaceship can exit.
    adj = get_adjacencies(GOAL_POSITION); // Goal piece.
    for(const auto& a : adj)
    {
        if(a.second > 3 || !pieces[a.first]->get_p()){
            continue;
        }
        if(pieces[a.first]->get_piece_type() == SG_426A){
            if(can_slide(a.first, 10, a.second)){
                v.push_back(std::make_pair(a.first, opposite((ADJACENCY_DIRECTIONS)a.second)));
            }
        }
    }
    return v;
}

bool State::is_goal() const{
    return pieces[GOAL_POSITION]->get_piece_type() == SG_426A;
}

State::~State()
{
    for(int i = 0; i < 12; i++){
        delete pieces[i];
    }
    delete [] pieces;
    //std::cout << "Delete state." << std::endl;
}