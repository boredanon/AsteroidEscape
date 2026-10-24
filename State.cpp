#include "State.h"
#include <cctype>
#include <cstdlib>

State::State()
{
    pieces = new Piece*[12];
    g=0;
    h=0;
    current_empty=-1;
    current_ship=-1;
    for(int i = 0; i < 12; i++){
        pieces[i] = new Piece();
    }
}

State::State(std::string ps, std::string orientations)
{
    // Validate the two board lines before allocating anything.
    auto strip = [](std::string& line) {
        while(!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) line.pop_back();
    };
    strip(ps);
    strip(orientations);
    if(ps.size() < 9 || orientations.size() < 9){
        throw std::runtime_error("Illegal board: each of the two lines must have 9 characters "
                                 "(line 1: pieces, line 2: orientations).");
    }
    int ships = 0;
    for(int k = 0; k < 9; k++){
        if(std::string("-ABCDEFGH").find(ps[k]) == std::string::npos){
            throw std::runtime_error(std::string("Illegal board: unknown piece '") + ps[k] + "' in cell "
                                     + std::to_string(k) + " (valid: - A B C D E F G H).");
        }
        bool empty_cell = ps[k] == '-'; // The orientation of the empty cell is ignored ('-' is common).
        if(!empty_cell && (orientations[k] < '0' || orientations[k] > '3')){
            throw std::runtime_error(std::string("Illegal board: unknown orientation '") + orientations[k]
                                     + "' in cell " + std::to_string(k) + " (valid: 0 1 2 3).");
        }
        if(ps[k] == 'A') ships++;
    }
    if(ships != 1){
        throw std::runtime_error("Illegal board: there must be exactly one spaceship (A), found "
                                 + std::to_string(ships) + ".");
    }

    pieces = new Piece*[12];
    current_empty=-1;
    current_ship=-1;
    int empty_amounts = 0;
    g=0;

    int i = 0;
    for(i = 0; i < 9; i++)
    {
        PIECE_TYPE type = EMPTY;
        switch(ps[i])
        {
            case '-':
                type = EMPTY;
                current_empty=i;
                empty_amounts++;
            break;
            case 'A':
                type = SG_426A;
                current_ship=i;
            break;
            case 'B':
                type = SG_426B;
            break;
            case 'C':
                type = SG_426C;
            break;
            case 'D': // D, E and F are the same shape.
            case 'E':
            case 'F':
                type = SG_426DEF;
            break;
            case 'G':
                type = SG_426G;
            break;
            case 'H':
                type = SG_426H;
            break;
        }

        ORIENTATION o = type == EMPTY ? CANONICAL : (ORIENTATION)(orientations[i] - '0');
        pieces[i] = new Piece(type, o);
    }

    for(; i < 12; i++){
        pieces[i] = new Piece(EMPTY, CANONICAL); //Goal piece is Piece[10].
    }

    if(!is_legal() || empty_amounts>1 || empty_amounts == 0){

        for(int i = 0; i <  12; i++){
            delete pieces[i];
        }
        delete [] pieces;
        throw std::runtime_error(empty_amounts != 1
            ? "Illegal board: there must be exactly one empty cell (-), found " + std::to_string(empty_amounts) + "."
            : std::string("Illegal board: two pieces overlap."));
    }
    h = calculateH();
}

int State::calculateH(){
    int calculated_h = 0;

    switch(heuristic)
    {
        case ZERO:
            return 0;
        break;
        case DISTANCE:
        {
            // Manhattan distance (in cells) from the ship to the exit cell. The ship moves one cell
            // per move, so this is admissible and consistent.
            int row_distance = std::abs(current_ship / WIDTH - GOAL_POSITION / WIDTH);
            int column_distance = std::abs(current_ship % WIDTH - GOAL_POSITION % WIDTH);
            calculated_h += row_distance + column_distance;
        }
        break;
        case ROWDISTANCE:
            // Rows between the ship and the exit only. This is what DISTANCE computed before the
            // column term was fixed (it subtracted current_ship%3 from itself); kept so that results
            // obtained with the old DISTANCE can be reproduced.
            calculated_h += std::abs(current_ship / WIDTH - GOAL_POSITION / WIDTH);
        break;
        case TWOONE:
            if(current_ship%3 != 1){
                calculated_h+=1;
            }
            if(current_ship/3 != 3){
                calculated_h+=1;
            }
        break;
        case OBSTACLES0:
        {
            if(current_ship%3 != 1){
                calculated_h+=1;
            }
            if(current_ship/3 != 3){
                calculated_h+=1;
            }

            if(pieces[GOAL_POSITION]->get_piece_type() != SG_426A){
                auto adj = get_adjacencies(GOAL_POSITION);
                for(const auto& a : adj)
                {
                    if(a.second > 3){ // No diagonals.
                        continue;
                    }

                    if(pieces[a.first]->get_piece_type() > SG_426A){
                        calculated_h+=1;
                    }
                }
            }
        }
        break;
        case OBSTACLES1:

            if(current_ship%3 != 1){
                calculated_h+=1;
            }
            if(current_ship/3 != 3){
                calculated_h+=1;
            }
            for(int i = GOAL_POSITION; i/3 > current_ship/3; i-=3){ // Check upwards, columns are already checked.
                    if(pieces[i]->get_piece_type() != EMPTY){
                        calculated_h+=1;
                    }
                }

        break;
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

void State::set_empty(){
    for(int i = 0; i < 9; i++)
    {
        if(pieces[i]->get_piece_type() == EMPTY){
            current_empty=i;
            break;
        }
    }
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

// Draws the whole board as one picture.
//
// Every piece is an 8x8 bitmap whose 4x4 centre is its own cell and whose 2-pixel border reaches
// into the neighbouring cells (the physical pieces interlock). All bitmaps are placed on a common
// 16x16 canvas (cell i starts at pixel (4*row, 4*col)), so the drawing shows the real shapes:
//   - each pixel shows the letter of its piece: UPPERCASE inside the piece's own cell,
//     lowercase where it sticks out into a neighbouring cell;
//   - '.' is free space inside the board, '*' would be two pieces overlapping (illegal board);
//   - the frame is the 3x3 board, ':' separates cells, and the exit is below the middle
//     bottom cell (cell 7).
// Below the picture: one line per cell (letter, name, orientation) and the search state.
std::string State::print_board() const
{
    const int CANVAS_ROWS = 20; // 16 board rows + the exit cell below cell 7.
    const int CANVAS_COLS = 16;
    char canvas[CANVAS_ROWS][CANVAS_COLS];

    for(int y = 0; y < CANVAS_ROWS; y++)
        for(int x = 0; x < CANVAS_COLS; x++)
            canvas[y][x] = (y >= 2 && y < 14 && x >= 2 && x < 14) ? '.' : ' ';

    auto owner_cell = [](int y, int x) -> int { // Cell whose 4x4 interior contains pixel (y, x).
        if(y < 2 || x < 2 || x >= 14) return -1;
        int r = (y - 2) / 4, c = (x - 2) / 4;
        if(r < 3) return 3 * r + c;
        return (r == 3 && c == 1) ? GOAL_POSITION : -1;
    };

    for(int i = 0; i < 11; i++)
    {
        if(i == 9 || pieces[i]->get_piece_type() == EMPTY) continue; // 9 is not part of the board.
        int r = i / 3, c = i % 3;
        char letter = pieces[i]->get_letter();
        for(int y = 0; y < 8; y++)
            for(int x = 0; x < 8; x++)
            {
                if(!pieces[i]->has_pixel(y, x)) continue;
                int Y = 4 * r + y, X = 4 * c + x;
                if(Y >= CANVAS_ROWS || X >= CANVAS_COLS) continue;
                char ch = owner_cell(Y, X) == i ? letter : (char)std::tolower(letter);
                canvas[Y][X] = (canvas[Y][X] == '.' || canvas[Y][X] == ' ') ? ch : '*';
            }
    }

    auto row_has_content = [&](int y) {
        for(int x = 0; x < CANVAS_COLS; x++)
            if(canvas[y][x] != ' ' && canvas[y][x] != '.') return true;
        return false;
    };

    // Pixel row y: 2 outer pixels | 4 px : 4 px : 4 px | 2 outer pixels, then the row label.
    auto pixel_row = [&](int y) {
        bool in_frame = y >= 2 && y < 14;
        bool in_exit = y >= 14 && y < 18;
        std::string line = "  ";
        for(int x = 0; x < CANVAS_COLS; x++)
        {
            if(x == 2 || x == 14) line += in_frame ? '|' : ' ';
            else if(x == 6 || x == 10) line += in_frame ? ':' : (in_exit ? '|' : ' ');
            line += canvas[y][x];
            line += ' ';
        }
        if(in_frame && (y - 2) % 4 == 1) line += "  row " + std::to_string((y - 2) / 4);
        while(!line.empty() && line.back() == ' ') line.pop_back();
        return line + "\n";
    };

    auto rule = [](const std::string& left, const std::string& mid, const std::string& right) {
        return "      +" + left + "+" + mid + "+" + right + "+\n";
    };

    std::string out;
    out += "        col 0    col 1    col 2\n";
    if(row_has_content(0)) out += pixel_row(0);
    if(row_has_content(1)) out += pixel_row(1);
    out += rule("--------", "--------", "--------");
    for(int y = 2; y < 14; y++)
    {
        if(y == 6 || y == 10) out += rule("- - - - ", "- - - - ", "- - - - ");
        out += pixel_row(y);
    }
    out += rule("--------", "  EXIT  ", "--------");
    for(int y = 14; y < CANVAS_ROWS; y++)
        if(row_has_content(y)) out += pixel_row(y);
    out += "\n";

    // Legend: one line per cell.
    out += "  cell  (r,c)  piece  name                        orientation\n";
    for(int i = 0; i < 9; i++)
    {
        const Piece* p = pieces[i];
        std::string line = "  " + std::to_string(i) + "     (" + std::to_string(i / 3) + "," + std::to_string(i % 3) + ")  ";
        if(p->get_piece_type() == EMPTY)
        {
            line += "  -    (empty)";
        }
        else
        {
            std::string name = p->get_name();
            line += "  ";
            line += p->get_letter();
            line += "    " + name + std::string(name.size() < 28 ? 28 - name.size() : 1, ' ');
            line += std::to_string(p->get_degrees()) + " deg";
        }
        out += line + "\n";
    }

    // Search state.
    out += "\n";
    if(is_goal())
        out += "  Ship: has left the board (goal reached).\n";
    else
        out += "  Ship: cell " + std::to_string(current_ship) + " (row " + std::to_string(current_ship / 3)
             + ", col " + std::to_string(current_ship % 3) + ")   exit: below cell 7\n";
    out += "  Empty cell: " + std::to_string(current_empty) + "\n";
    out += "  g = " + std::to_string(g) + "   h = " + std::to_string(h)
         + " (" + heuristic_name(heuristic) + ")   f = " + std::to_string(g + h) + "\n";
    return out;
}

std::string State::print_board2() const
{
    std::string full_board;
    uint64_t context_pieces[9];
    std::string string_pieces[9];


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
        context_pieces[p] = piece;
    }

    for(int x = 0; x < 9; x++)
    {

        auto outside = get_binary64(~COMBINED[x]);
        auto context_outside = get_binary64(context_pieces[x] & ~COMBINED[x]);
        std::string c_piece = get_binary64(context_pieces[x]);



        for(unsigned int c = 0; c < c_piece.size(); c++){
            
            if(context_outside[c] == '0' && outside[c] == '1'){
                c_piece[c] = ' ';
            }

            if(c_piece[c] == '0'){
                c_piece[c] = '.';
            }

        }
        string_pieces[x] = c_piece;
    }


    auto size=4*HEIGHT+4;
    std::string b[size];
    int horizontal_offsets[3] = {0, 2, 4};
    int vertical_offsets[3] = {0, 16, 32};
    int column=0;
    for(int j = 0; j < size; j++){
        for(int i = 0; i < size;i++){
            column++;
            auto row = j/(8 - 2);
            auto col = i/(8 - 2);

            auto piece_index = col + row*3;
            auto string_index=(((j%(8-2)*8) + vertical_offsets[piece_index/HEIGHT]) + i%(8-2) + horizontal_offsets[piece_index%WIDTH]);
            auto to_add = string_pieces[piece_index][string_index];
            b[j] += to_add;

            if(column%size==6 || column%size==10){
                b[j]+=" ";
            }

        }
        if(j%size == 6 || j%size == 10){
            std::string blank(4, ' ');
            full_board+= blank+"\n";
        }
        full_board+=b[j] + "\n";
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
    previous_move = (ADJACENCY_DIRECTIONS)m.second;
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

    //Movements.
    auto adj = get_adjacencies(current_empty);
    for(const auto& a : adj){
        if(a.second > 3 || !pieces[a.first]->get_p() || (!CAN_RETURN && previous_move == a.second)){
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

double State::calculate_entropy(std::vector<double>& child_entropies) const
{
    if(is_goal()) return 0.0;
    if(!REMUSE){   
        double H=0;
        auto m = get_legal_moves().size();
    //Precalculated, there are max 4 moves. Otherwise use sum P(z_k)*log2(1/P(z_k)), k=1 to n
        switch(m)
        {
            case 0:
                H=1000000; // Arbitrary high number.
                break;
            case 1:
                H=0.0;
                break;
            case 2:
                H=1.0;
                break;
            case 3:
                H=1.6;
                break;
            case 4:
                H=2.0;
                break;        
            }

        return H;
    }
    else
    {
        // Softmin.
        std::vector<double> P;
        double Q = (1.0 / child_entropies.size());

        float denominator = 0.0;
        for(const auto & z_j : child_entropies)
        {
            if(z_j < 10000.0)
                denominator += std::exp(-z_j);
        }

        for(const auto & z_i : child_entropies)
        {
            if(z_i >= 10000.0){
                P.push_back(0);
                continue;
            }
            auto e = std::exp(-z_i);
            P.push_back(e/denominator);
        }


        //KL(softmin)
        double divergence_KL = 0.0;
        for(unsigned int x = 0; x < child_entropies.size(); x++)
        {
            if(P[x] != 0)
                divergence_KL += P[x] * log2((P[x]/Q));
        }
        return divergence_KL;
    }
    //KL-softmin
}

State::~State()
{
    for(int i = 0; i < 12; i++){
        delete pieces[i];
    }
    delete [] pieces;
    //std::cout << "Delete state." << std::endl;
}