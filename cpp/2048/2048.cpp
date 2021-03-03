#include <iostream>
#include <vector>
#include <random>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <termios.h>
using namespace std;

// listen key press
void setBufferedInpu(bool enable) {
    static bool enabled = true;
    static struct termios old_t;
    struct termios new_t;

    if(enable && !enabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
        enabled = true;
    } else if(!enable && enabled) {
        tcgetattr(STDIN_FILENO, &new_t);
        old_t = new_t;
        new_t.c_lflag &= (~ICANON & ~ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
        enabled = false;
    }
}

enum {
    MOVE_FAILED = 0,
    MOVE_SUCCESS = 1
};

enum {
    SIZE = 4
};
class ChessBoard {
    friend void print_chess_board(const ChessBoard &);
public:
    ChessBoard(){
        initBoard();
    };

    const int getValue(size_t row, size_t col) const {
        return pieces[row*SIZE + col];
    }
        
    void setValue(size_t row, size_t col, int value) {
        pieces[row*SIZE + col] = value;
    }

    const vector<int> getRow(size_t row) const {
        vector<int> geted_row(SIZE);
        for(size_t col = 0; col < SIZE; ++col)
            geted_row[col] = getValue(row, col);

        return geted_row;
    }

    void setRow(size_t row, const vector<int> &set_row) {
        for(size_t col = 0; col < SIZE; ++col)
            setValue(row, col, set_row[col]);
    }

    const vector<int> getCol(size_t col) const {
        vector<int> geted_col(SIZE);
        for(size_t row = 0; row < SIZE; ++row)
            geted_col[row] = getValue(row, col);
        return geted_col;
    }

    void setCol(size_t col, const vector<int> &set_col) {
        for(size_t row = 0; row < SIZE; ++row)
            setValue(row, col, set_col[row]);
    }

    void resetBoard() {
        for(size_t i = 0; i < SIZE*SIZE; ++i)
            pieces[i] = 0;
        initBoard();
    }

private:
    int pieces[SIZE*SIZE] = {0}; 
    void initBoard();
};

void ChessBoard::initBoard() {
    uniform_int_distribution<size_t> u(0,15);
    default_random_engine e(time(0));
    size_t pos1 = u(e);
    size_t pos2 = u(e); 
    while(pos1 == pos2)
        pos2 = u(e);
    pieces[pos1] = 2;
    pieces[pos2] = 2;

}

void print_chess_board(const ChessBoard &cb){
    for(int row = 0; row < SIZE; ++row){
        // edge line 
        printf("          ");
        for(int col = 0; col < SIZE; ++col)
            printf("+----");
        printf("+\n");

        // digits line
        printf("          ");
        for(int col = 0; col < SIZE; ++col) {
            if(cb.pieces[row*SIZE+col] == 0)
                printf("|    ");
            else if(cb.pieces[row*SIZE+col] < 1000)
                printf("|%3d ", cb.pieces[row*SIZE+col]);
            else
                printf("|%4d", cb.pieces[row*SIZE+col]);
        }
        printf("|\n");
    }
    // last line
    printf("          ");
    for(int col = 0; col < SIZE; ++col)
        printf("+----");
    printf("+\n");

}

class Game {
public:
    Game(){};
    void run();
    void restart();
    int moveTo(char direction);

private:
    vector<int> delete_zeros(const vector<int> &vi);
    void extent_zeros(vector<int> &vi, vector<int>::size_type sz);
    vector<int> added_numbers(const vector<int> &vi);
    void random_generate_number();
    int moveToTop();
    int moveToButtom();
    int moveToLeft();
    int moveToRight();

    unsigned reset_random_seed = 1;
    ChessBoard cb;
};

void Game::run() {
    // clear screen
    printf("\033[2J");
    // reset cursor
    printf("\033[H");
    // hide cursor
    printf("\033[?25l");
    printf("     ============= 2048 =============\n");
    printf("       (press arrow keys to move.)\n");
    printf("     (press r to restart, q to quit.)\n");
    printf("\n");

    print_chess_board(cb);
    while(true) {
        char c;
        c = getchar();
        if(c == 'q')
            break;
        else if(c == 'r')
            restart();
        else
            if(moveTo(c)){
                random_generate_number();
                for(int i = 0; i < SIZE*2 + 1; ++i){
                    printf("\033[1A");
                    printf("\033[K");
                }
                print_chess_board(cb);
            }
    }
    // clear screen
    printf("\033[2J");
    // reset cursor
    printf("\033[H");
    // show cursor
    printf("\033[?25h");
}

void Game::restart() {
    cb.resetBoard();
    for(int i = 0; i < SIZE*2 + 1; ++i){
        printf("\033[1A");
        printf("\033[K");
    }
    print_chess_board(cb);

}

vector<int> Game::delete_zeros(const vector<int> &vi) {
    vector<int> deleted_vi;
    auto it = vi.begin();

    while(it != vi.end()){
        if (*it != 0)
            deleted_vi.push_back(*it);
        it++;
    }
    return deleted_vi;
}

void Game::extent_zeros(vector<int> &vi, vector<int>::size_type sz) {
    auto vi_size = vi.size();
    while(vi_size++ < sz)
        vi.push_back(0);
}

vector<int> Game::added_numbers(const vector<int> &vi){
    vector<int> added_vi;
    vector<int> tmp_vi = delete_zeros(vi);
    auto sz = vi.size();
    auto it = tmp_vi.begin();
    if(it == tmp_vi.end()) {
        extent_zeros(tmp_vi, sz);
        return tmp_vi;
    }
    added_vi.push_back(*it);
    bool need_check = true;
    while(++it != tmp_vi.end()){
        if(need_check) {
            if(*it == *(it-1)) {
                added_vi[added_vi.size()-1] = *it * 2;
                need_check = false;
            } else
                added_vi.push_back(*it);
        } else {
            added_vi.push_back(*it);
            need_check = true;
        }
    }
    extent_zeros(added_vi, sz);
    return added_vi;
}

void Game::random_generate_number(){
    vector< pair<size_t, size_t> > pos;
    for(int row = 0; row < SIZE; ++row)
        for(int col = 0; col < SIZE; ++col)
            if(cb.getValue(row, col) == 0)
                pos.push_back(pair<size_t, size_t>(row, col));
    int zero_length = pos.size();
    uniform_int_distribution<size_t> u(0,zero_length-1);
    if(reset_random_seed == 8192)
        reset_random_seed = 1;
    default_random_engine e(time(0)*reset_random_seed);
    size_t generate_pos = u(e);
    cb.setValue(pos[generate_pos].first, 
            pos[generate_pos].second, 
            2);
}

int Game::moveToTop(){
    vector<int> col_value;
    vector<int> moved_value;
    int move_stat = MOVE_FAILED;
    for(size_t col = 0; col < SIZE; ++col){
        col_value = cb.getCol(col);
        moved_value = added_numbers(col_value);
        if(col_value != moved_value){
            cb.setCol(col, moved_value);
            move_stat = MOVE_SUCCESS;
        }
    }
    return move_stat;
}

int Game::moveToButtom(){
    vector<int> col_value;
    vector<int> moved_value;
    int move_stat = MOVE_FAILED;
    for(size_t col = 0; col < SIZE; ++col){
        col_value = cb.getCol(col);
        vector<int> r_col_value(col_value.rbegin(), col_value.rend());
        r_col_value = added_numbers(r_col_value);
        moved_value = vector<int>(r_col_value.rbegin(), 
                r_col_value.rend());
        if(col_value != moved_value){
            cb.setCol(col,moved_value);
            move_stat = MOVE_SUCCESS;
        }

    }
    return move_stat;
}

int Game::moveToLeft(){
    vector<int> row_value;
    vector<int> moved_value;
    int move_stat = MOVE_FAILED;
    for(size_t row = 0; row < SIZE; ++row){
        row_value = cb.getRow(row);
        moved_value = added_numbers(row_value);
        if(row_value != moved_value){
            cb.setRow(row,moved_value);
            move_stat = MOVE_SUCCESS;
        }
    }
    return move_stat;
}

int Game::moveToRight(){
    vector<int> row_value;
    vector<int> moved_value;
    int move_stat = MOVE_FAILED;
    for(size_t row = 0; row < SIZE; ++row){
        row_value = cb.getRow(row);
        vector<int> r_row_value(row_value.rbegin(), row_value.rend());
        r_row_value = added_numbers(r_row_value);
        moved_value = vector<int>(r_row_value.rbegin(), 
                r_row_value.rend());
        if(row_value != moved_value){
            cb.setRow(row, moved_value);
            move_stat = MOVE_SUCCESS;
        }
    }
    return move_stat;
}

int Game::moveTo(char direction){
    int move_stat = MOVE_FAILED;
    switch(direction){
        case 65:
            move_stat = moveToTop();
            break;
        case 66:
            move_stat = moveToButtom();
            break;
        case 68:
            move_stat = moveToLeft();
            break;
        case 67:
            move_stat = moveToRight();
            break;
        default:
            move_stat = MOVE_FAILED;
            break;
    }
    return move_stat;

}

int main() {
 
    setBufferedInpu(false);
    Game gm;
    gm.run();

    return 0;
}
