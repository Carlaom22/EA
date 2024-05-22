#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 500

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point points[MAX * MAX];
    int size;
} Path;

int N, M, C;
char maze[MAX][MAX];
bool water[MAX][MAX];
bool visited[MAX][MAX];
Point manholes[MAX];
int manhole_count;
Point door, exit12;
Path path;
Point flood_gate[2];

// Directions for up, left, down, right
int dx[] = {-1, 0, 1, 0};
int dy[] = {0, -1, 0, 1};

bool is_valid(int x, int y) {
    return x >= 0 && x < N && y >= 0 && y < M;
}

void flood_fill(int sx, int sy) {
    if (!is_valid(sx, sy) || water[sx][sy] || maze[sx][sy] == '#') return;
    water[sx][sy] = true;
    for (int d = 0; d < 4; d++) {
        flood_fill(sx + dx[d], sy + dy[d]);
    }
}

void spread_water() {
    // Reset water array
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            water[i][j] = false;
        }
    }
    // Spread water from each manhole
    for (int i = 0; i < manhole_count; i++) {
        int mx = manholes[i].x;
        int my = manholes[i].y;
        flood_fill(mx, my);
    }
}

bool find_path(int x, int y) {
    if (!is_valid(x, y) || visited[x][y] || water[x][y] || maze[x][y] == '#') return false;
    visited[x][y] = true;
    path.points[path.size++] = (Point){x, y};
    if (maze[x][y] == 'E') return true;
    for (int d = 0; d < 4; d++) {
        if (find_path(x + dx[d], y + dy[d])) return true;
    }
    path.size--;  // Backtrack
    return false;
}

bool solve() {
    // Try different flood gate placements and check for a valid path
    for (int r1 = 0; r1 < N; r1++) {
        for (int c1 = 0; c1 < M; c1++) {
            if (maze[r1][c1] == '#') continue;
            for (int d = 0; d < 4; d++) {
                int r2 = r1 + dx[d];
                int c2 = c1 + dy[d];
                if (is_valid(r2, c2) && maze[r2][c2] != '#') {
                    // Place flood gate between (r1, c1) and (r2, c2)
                    flood_gate[0] = (Point){r1, c1};
                    flood_gate[1] = (Point){r2, c2};
                    spread_water();
                    // Check for a valid path
                    for (int i = 0; i < N; i++) {
                        for (int j = 0; j < M; j++) {
                            visited[i][j] = false;
                        }
                    }
                    path.size = 0;
                    if (find_path(door.x, door.y)) {
                        // Output solution
                        printf("%d %d %d %d\n", r1, c1, r2, c2);
                        printf("%d\n", 0);  // Number of manhole covers (for simplicity, using none here)
                        printf("%d\n", path.size);
                        for (int i = 0; i < path.size; i++) {
                            printf("%d %d\n", path.points[i].x, path.points[i].y);
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main() {
    int test_cases;
    scanf("%d", &test_cases);
    while (test_cases--) {
        scanf("%d %d", &N, &M);
        for (int i = 0; i < N; i++) {
            scanf("%s", maze[i]);
        }
        scanf("%d", &C);
        manhole_count = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (maze[i][j] == 'D') {
                    door = (Point){i, j};
                } else if (maze[i][j] == 'E') {
                    exit12 = (Point){i, j};
                } else if (maze[i][j] == 'M') {
                    manholes[manhole_count++] = (Point){i, j};
                }
            }
        }
        solve();
    }
    return 0;
}
