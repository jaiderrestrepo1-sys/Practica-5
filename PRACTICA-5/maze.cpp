#include "maze.h"

Maze::Maze() : rows(0), cols(0), cellSize(24) {}

void Maze::setGrid(const std::vector<std::vector<int>>& g, int cellSize_) {
    grid = g;
    rows = static_cast<int>(g.size());
    cols = (rows>0) ? static_cast<int>(g[0].size()) : 0;
    cellSize = cellSize_;
}

bool Maze::isWall(int r, int c) const {
    if (!inBounds(r,c)) return false;
    return grid[r][c] == 2;
}

bool Maze::hasDot(int r, int c) const {
    if (!inBounds(r,c)) return false;
    return grid[r][c] == 1;
}

void Maze::eatDot(int r, int c) {
    if (inBounds(r,c) && grid[r][c] == 1) grid[r][c] = 0;
}

bool Maze::inBounds(int r, int c) const {
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

int Maze::wrapX(int x) const {
    if (cols == 0) return x;
    if (x < 0) return cols - 1;
    if (x >= cols) return 0;
    return x;
}
