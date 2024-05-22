#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

enum class maze_node_type {
    START = 'D',
    EXIT = 'E',
    MANHOLE = 'M',
    EMPTY = '.',
    WALL = '#'
};

istream& operator>>(istream& is, maze_node_type& type) {
    char c;
    is >> c;
    switch (c) {
        case 'D':
            type = maze_node_type::START;
            break;
        case 'E':
            type = maze_node_type::EXIT;
            break;
        case 'M':
            type = maze_node_type::MANHOLE;
            break;
        case '.':
            type = maze_node_type::EMPTY;
            break;
        case '#':
            type = maze_node_type::WALL;
            break;
        default:
            // Handle invalid input
            break;
    }
    return is;
}

typedef struct maze_node {
    int row;
    int col;
    int dist;
    maze_node_type type;
    bool watered;
    bool covered;
    vector<struct maze_node*> neighbors;
    struct maze_node *next_shortest;
    int distance_from_start;
    maze_node(int r, int c, int d, maze_node_type t):
        row(r), col(c), dist(d), type(t), watered(false), neighbors(vector<struct maze_node*>()),
        next_shortest(nullptr), distance_from_start(-1), covered(false) {}
} maze_node_t;

class node_t {
public:
    int dist;
    maze_node_t* node;
    node_t(maze_node_t *n, int d): node(n), dist(d) {}
};

// Define a custom comparator for priority queue
struct CompareDist {
    bool operator()(const node_t& n1, const node_t& n2) {
        return n1.dist > n2.dist;
    }
};

class Maze {
public:
    vector<vector<maze_node_t*>> maze;
    vector<maze_node_t*> manholes;
    vector<maze_node_t*> last_watered;
    vector<maze_node_t*> shortestPath;
    maze_node_t* start;
    maze_node_t* end;
    int rows;
    int cols;
    maze_node_t* flood_gate[2];

    Maze(int r, int c) : rows(r), cols(c) {
        maze = vector<vector<maze_node_t*>>(r, vector<maze_node_t*>(c, nullptr));
        flood_gate[0] = nullptr;
        flood_gate[1] = nullptr;
        start = nullptr;
        end = nullptr;
    }

    ~Maze() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (maze[i][j] != nullptr)
                    delete maze[i][j];
            }
        }
    }

    void build_maze_from_input() {
        maze_node_type type;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                cin >> type;
                maze[i][j] = new maze_node_t(i, j, 0, type);
                if (type == maze_node_type::START) {
                    start = maze[i][j];
                } else if (type == maze_node_type::EXIT) {
                    end = maze[i][j];
                } else if (type == maze_node_type::MANHOLE) {
                    manholes.push_back(maze[i][j]);
                }
            }
        }
    }

    void add_neighbors(maze_node_t* node) {
        if (node->row > 0 && maze[node->row - 1][node->col]->type != maze_node_type::WALL) {
            node->neighbors.push_back(maze[node->row - 1][node->col]);
        }
        if (node->row < rows - 1 && maze[node->row + 1][node->col]->type != maze_node_type::WALL) {
            node->neighbors.push_back(maze[node->row + 1][node->col]);
        }
        if (node->col > 0 && maze[node->row][node->col - 1]->type != maze_node_type::WALL) {
            node->neighbors.push_back(maze[node->row][node->col - 1]);
        }
        if (node->col < cols - 1 && maze[node->row][node->col + 1]->type != maze_node_type::WALL) {
            node->neighbors.push_back(maze[node->row][node->col + 1]);
        }
    }

    void build_maze_graph() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (maze[i][j]->type != maze_node_type::WALL) {
                    add_neighbors(maze[i][j]);
                }
            }
        }
    }

    void water_neighbors(maze_node_t* node, int step) {
        for (auto neighbor : node->neighbors) {
            if (neighbor->type == maze_node_type::EMPTY && !neighbor->watered) {
                if (neighbor->distance_from_start != -1 && neighbor->distance_from_start >= step) {
                    if (flood_gate[0] == nullptr) {
                        flood_gate[0] = neighbor;
                        flood_gate[1] = node;
                        continue;
                    }
                }
                neighbor->watered = true;
                last_watered.push_back(neighbor);
            }
        }
    }

    void flood(int step) {
        if (last_watered.empty()) {
            for (auto manhole : manholes) {
                if (!manhole->covered) {
                    water_neighbors(manhole, step);
                }
            }
        } else {
            auto nodes_to_start_water = last_watered;
            last_watered.clear();
            for (auto node : nodes_to_start_water) {
                water_neighbors(node, step);
            }
        }
    }

    void clear_flood(maze_node_t* node) {
        if (node->watered) {
            node->watered = false;
            for (auto neighbor : node->neighbors) {
                clear_flood(neighbor);
            }
        }
    }

    void clear_flood() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (maze[i][j] != nullptr && maze[i][j]->watered) {
                    clear_flood(maze[i][j]);
                }
            }
        }
    }

    void find_shortest_path() {
        vector<vector<int>> distance(rows, vector<int>(cols, INT_MAX));
        vector<vector<maze_node_t*>> parent(rows, vector<maze_node_t*>(cols, nullptr));
        priority_queue<node_t, vector<node_t>, CompareDist> pq;

        pq.push(node_t(start, 0));
        distance[start->row][start->col] = 0;

        while (!pq.empty()) {
            node_t curr = pq.top();
            pq.pop();

            if (curr.node == end) {
                shortestPath.clear();
                maze_node_t* currentNode = end;
                maze_node_t* previous = nullptr;
                while (currentNode != start) {
                    currentNode->distance_from_start = distance[currentNode->row][currentNode->col];
                    shortestPath.push_back(currentNode);
                    previous = currentNode;
                    currentNode = parent[currentNode->row][currentNode->col];
                    currentNode->next_shortest = previous;
                }
                shortestPath.push_back(start);
                reverse(shortestPath.begin(), shortestPath.end());
                return;
            }

            for (const auto& node : curr.node->neighbors) {
                if (curr.dist + 1 < distance[node->row][node->col]) {
                    distance[node->row][node->col] = curr.dist + 1;
                    parent[node->row][node->col] = curr.node;
                    pq.push(node_t(node, curr.dist + 1));
                }
            }
        }
    }

    bool start_cross_maze(int covers, int p) {
        if (covers != 0 && p < manholes.size()) {
            manholes[p]->covered = true;
            if (start_cross_maze(covers - 1, p + 1)) {
                return true;
            }
            manholes[p]->covered = false;
            if (start_cross_maze(covers, p + 1)) {
                return true;
            }
        } else {
            int i = 0;
            clear_flood();
            flood_gate[0] = nullptr;
            flood_gate[1] = nullptr;
            for (auto node : shortestPath) {
                if (i == 0) {
                    i++;
                    continue;
                }
                flood(i);
                if (node->watered) {
                    return false;
                }
                ++i;
            }
            return true;
        }
        return false;
    }

    void start_cross_maze(int covers) {
        if (!start_cross_maze(covers, 0) && flood_gate[0] == nullptr) {
            if (!manholes.empty()) {
                auto manhole = manholes[0];
                flood_gate[0] = manhole;
                flood_gate[1] = (manhole->neighbors.empty()) ? manhole : manhole->neighbors[0];
            }
        }
    }
};

int main() {
    int test_cases;
    cin >> test_cases;

    for (int t = 1; t <= test_cases; ++t) {
        int rows, cols;
        cin >> rows >> cols;
        Maze maze(rows, cols);
        maze.build_maze_from_input();
        maze.build_maze_graph();
        maze.find_shortest_path();
        int covers;
        cin >> covers;

        maze.start_cross_maze(covers);

        cout << maze.flood_gate[0]->row << " " << maze.flood_gate[0]->col << " " << maze.flood_gate[1]->row << " " << maze.flood_gate[1]->col << endl;

        int count = 0;
        for (auto manhole : maze.manholes) {
            if (manhole->covered) {
                ++count;
            }
        }
        cout << count << endl;

        for (auto manhole : maze.manholes) {
            if (manhole->covered) {
                cout << manhole->row << " " << manhole->col << endl;
            }
        }

        cout << maze.shortestPath.size() << endl;
        for (const auto& point : maze.shortestPath) {
            cout << point->row << " " << point->col << endl;
        }
        cout << endl;
    }

    return 0;
}
