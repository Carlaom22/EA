#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <stack>

enum direction  {
    left,
    right
};

using namespace std;

class Vault {
private:
    vector<vector<int>> grid;
    vector<vector<int>> original;
    int rows, columns, max_moves;
    bool found_solution;

public:
    Vault(int r, int c, int m) : rows(r), columns(c), max_moves(m) {
        grid.resize(columns, vector<int>(rows));
        original.resize(columns, vector<int>(rows));
    }

    Vault(int r, int c, int m, vector<vector<int>> grid): Vault(r,c,m){
        this->grid = grid;
        this->original = grid;
    }


    int get_symbol(int x, int y) {
        return grid[x][y];
    }

    void set_symbol(int x, int y, int val) {
        grid[x][y] = val;
        original[x][y] = val;
    }

    void print() {
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < columns; ++x) {
                cout << get_symbol(x, y) << " ";
            }
            cout << endl;
        }
    }

    bool correct() {
        for (int y = 0; y < rows; ++y) {
            if (get_symbol(0, y) != y + 1) {
                return false;
            }
            for (int x = 1; x < columns; ++x) {
                if (get_symbol(0, y) != get_symbol(x, y)) {
                    return false;
                }
            }
        }
        return true;
    }



    void rotate_left(int x, int y) {
        int temp = grid[x][y];
        grid[x][y] = grid[x + 1][y];
        grid[x + 1][y] = grid[x + 1][y + 1];
        grid[x + 1][y + 1] = grid[x][y + 1];
        grid[x][y + 1] = temp;
    }

    void rotate_left_2(int x, int y) {
        int temp = grid[x][y];
        grid[x][y] = grid[x + 1][y + 1];
        grid[x + 1][y + 1] = temp;

        temp = grid[x + 1][y];
        grid[x + 1][y] = grid[x][y + 1];
        grid[x][y + 1] = temp;
    }


    void rotate_right(int x, int y) {
        int temp = grid[x][y];
        grid[x][y] = grid[x][y + 1];
        grid[x][y + 1] = grid[x + 1][y + 1];
        grid[x + 1][y + 1] = grid[x + 1][y];
        grid[x + 1][y] = temp;
    }


    bool valid_move(int x, int y) {
        return x >= 0 && x < rows - 1 && y >= 0 && y < columns - 1;
    }

    bool rotperline() {
        int totalMovimentosNecessarios = 0;
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < columns; ++x) {
                int valorDesejado = y+1;
                if(grid[x][y]!=valorDesejado){
                    if (grid[y][x]-y>0){
                    totalMovimentosNecessarios += abs(grid[y][x]-y)-1;
                    }
                    if (grid[x][y]-y<0){
                        totalMovimentosNecessarios += abs(grid[y][x]-y)+1;
                    }
                }

            }
        }

        return totalMovimentosNecessarios > max_moves;
    }

    bool solve(int moves) {
        if (moves == 0) {
            return correct();
        }

        for (int x = 0; x < columns - 1; ++x) {
            for (int y = 0; y < rows - 1; ++y) {
                rotate_right(x, y);
                if (solve(moves - 1)) {
                    return true;
                }
                rotate_left_2(x, y);

                // rotate_left(x, y);
                if (solve(moves - 1)) {
                    return true;
                }

                rotate_right(x, y);
            }
        }

        return false;
    }

    int solve() {
        if (rotperline()) {
            return -1; // Immediate return if the case is impossible.
        }
        for (int moves = 0; moves <= max_moves; ++moves) {
            if (solve(moves)) {
                return moves;
            }
        }
        return -1;
    }


};

void test_correct()
{
    Vault vault = Vault(2, 3, 5, {{2, 2}, {1, 2}, {1, 1}});
    cout << vault.correct()  << endl;
    vault.print();
    vault.rotate_left(1, 0);
    vault.rotate_left(1, 0);
    vault.rotate_left(0, 0);
    vault.print();
    cout << vault.correct()  << endl;
}



int main() {
    int T;
    int pattern;

    // test_correct();
    // return 0;


    cin >> T;

    for (int t = 0; t < T; ++t) {
        int R, C, M;
        cin >> R >> C >> M;

        Vault vault(R, C, M);

        for (int y = 0; y < R; ++y) {
            for (int x = 0; x < C; ++x) {
                cin >> pattern;
                vault.set_symbol(x, y, pattern);
            }
        }

        int moves = vault.solve();

        if (moves == -1) {
            cout << "the treasure is lost!\n";
        } else {
            cout << moves << endl;
        }
    }

    return 0;
}