#ifndef MAZE_H
#define MAZE_H

#include <vector>

class Maze {
public:
    std::vector<std::vector<int>> grid;
    int rows;
    int cols;
    int cellSize;
    Maze();
    void setGrid(const std::vector<std::vector<int>>& g, int cellSize_);
    bool isWall(int r, int c) const;
    bool hasDot(int r, int c) const;
    void eatDot(int r, int c);
    bool inBounds(int r, int c) const;
    int wrapX(int x) const;
};

#endif // MAZE_H
