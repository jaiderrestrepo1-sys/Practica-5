#include "pacman.h"

Pacman::Pacman() : r(1), c(1), dirR(0), dirC(0) {}

void Pacman::setDirection(int dr, int dc) {
    dirR = dr; dirC = dc;
}

void Pacman::tryMove(const Maze &m) {
    if (dirR == 0 && dirC == 0) return;
    int nr = r + dirR;
    int nc = c + dirC;
    // wrap horizontally (teleport)
    if (nc < 0) nc = m.wrapX(nc);
    if (nc >= m.cols) nc = m.wrapX(nc);
    // check bounds for rows
    if (!m.inBounds(nr, nc)) return;
    if (!m.isWall(nr, nc)) {
        r = nr;
        c = nc;
    } else {
    }
}

void Pacman::applyWrap(int cols) {
    if (c < 0) c = cols - 1;
    if (c >= cols) c = 0;
}
