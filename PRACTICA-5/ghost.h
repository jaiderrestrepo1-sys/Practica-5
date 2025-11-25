#ifndef GHOST_H
#define GHOST_H

#include <random>

struct Ghost {
    int r;
    int c;
    int dirR;
    int dirC;

    Ghost() : r(1), c(17), dirR(0), dirC(0) {}
    void randomStep(const class Maze &m) {
        static std::mt19937 rng((unsigned)std::random_device{}());
        std::uniform_int_distribution<int> d(0,3);
        int attempts = 0;
        if (dirR == 0 && dirC == 0) {
            int choice = d(rng);
            switch(choice) {
            case 0: dirR = -1; dirC = 0; break;
            case 1: dirR = 1; dirC = 0; break;
            case 2: dirR = 0; dirC = -1; break;
            case 3: dirR = 0; dirC = 1; break;
            }
        }
        int nr = r + dirR;
        int nc = c + dirC;
        if (nc < 0) nc = m.wrapX(nc);
        if (nc >= m.cols) nc = m.wrapX(nc);
        if (!m.inBounds(nr, nc) || m.isWall(nr, nc)) {
            // elegir nueva direccion distinta
            bool moved = false;
            for (int i = 0; i < 4 && !moved; ++i) {
                int choice = d(rng);
                int tr=0, tc=0;
                switch(choice) {
                case 0: tr=-1; tc=0; break;
                case 1: tr=1; tc=0; break;
                case 2: tr=0; tc=-1; break;
                case 3: tr=0; tc=1; break;
                }
                int trn = r + tr;
                int tcn = c + tc;
                if (tcn < 0) tcn = m.wrapX(tcn);
                if (tcn >= m.cols) tcn = m.wrapX(tcn);
                if (m.inBounds(trn, tcn) && !m.isWall(trn, tcn)) {
                    dirR = tr; dirC = tc;
                    r = trn; c = tcn;
                    moved = true;
                }
            }
            if (!moved) { dirR = 0; dirC = 0; }
        } else {
            r = nr; c = nc;
        }
    }
};

#endif // GHOST_H
