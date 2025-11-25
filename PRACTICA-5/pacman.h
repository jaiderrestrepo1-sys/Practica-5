#ifndef PACMAN_H
#define PACMAN_H

#include "maze.h"

struct Pacman {
    int r;
    int c;
    int dirR;
    int dirC;
    Pacman();
    void setDirection(int dr, int dc);
    void tryMove(const Maze &m);
    void applyWrap(int cols);
};

#endif // PACMAN_H
