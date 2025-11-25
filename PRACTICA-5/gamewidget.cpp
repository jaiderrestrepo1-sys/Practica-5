#include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <cmath>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), maze(), pacman(), timer(this), score(0), cellSize(24)
{
    setWindowTitle("Pac-Man - Práctica 5");
    loadDefaultMaze();

    pacman.r = 1;
    pacman.c = 1;
    pacman.dirR = 0; pacman.dirC = 0;


    connect(&timer, &QTimer::timeout, this, &GameWidget::tick);
    timer.start(140);


    setFixedSize(maze.cols * cellSize, maze.rows * cellSize + 30);
}

void GameWidget::loadDefaultMaze()
{
    std::vector<std::vector<int>> grid = {

    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,2},
    {2,1,2,2,1,2,2,2,1,2,1,2,2,2,1,2,2,1,2},
    {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
    {2,1,2,2,1,2,1,2,2,2,2,2,1,2,1,2,2,1,2},
    {2,1,1,1,1,2,1,1,1,2,1,1,1,2,1,1,1,1,2},
    {2,2,2,2,1,2,2,2,0,2,0,2,2,2,1,2,2,2,2},
    {0,0,0,2,1,2,0,0,0,0,0,0,0,2,1,2,0,0,0},  // MAPA
    {2,2,2,2,1,2,0,2,2,0,2,2,0,2,1,2,2,2,2},
    {0,0,0,0,1,0,0,2,0,0,0,2,0,0,1,0,0,0,0},
    {2,2,2,2,1,2,0,2,0,0,0,2,0,2,1,2,2,2,2},
    {0,0,0,2,1,0,0,2,2,2,2,2,0,0,1,2,0,0,0},
    {2,2,2,2,1,2,0,0,0,0,0,0,0,2,1,2,2,2,2},
    {2,1,1,1,1,2,0,2,2,2,2,2,0,2,1,1,1,1,2},
    {2,1,2,2,1,1,1,1,1,2,1,1,1,1,1,2,2,1,2},
    {2,1,1,2,1,2,1,1,1,1,1,1,1,2,1,2,1,1,2},
    {2,2,1,1,1,2,1,1,2,2,2,1,1,2,1,1,1,2,2},
    {2,1,1,2,2,2,2,1,1,2,1,1,2,2,2,2,1,1,2},
    {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}

};

maze.setGrid(grid, cellSize);
}


void GameWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // background
    p.fillRect(rect(), Qt::black);

    // draw maze walls and dots
    for (int r=0; r<maze.rows; ++r) {
        for (int c=0; c<maze.cols; ++c) {
            int x = c * cellSize;
            int y = r * cellSize;
            if (maze.grid[r][c] == 2) {
                // wall
                p.fillRect(x, y, cellSize, cellSize, Qt::blue);
            } else if (maze.grid[r][c] == 1) {
                // dot
                int dotSize = std::max(2, cellSize/6);
                int cx = x + cellSize/2;
                int cy = y + cellSize/2;
                p.setBrush(Qt::white);
                p.setPen(Qt::NoPen);
                p.drawEllipse(cx - dotSize/2, cy - dotSize/2, dotSize, dotSize);
                p.setPen(Qt::NoPen);
                p.setBrush(Qt::NoBrush);
            }
        }
    }

    // DIBUJO PACMAN
    int px = pacman.c * cellSize;
    int py = pacman.r * cellSize;
    int margin = 2;
    QRect pacRect(px + margin, py + margin, cellSize - 2*margin, cellSize - 2*margin);


    int startAngle = 30*16;
    int spanAngle = 300*16;
    if (pacman.dirR == 0 && pacman.dirC == 1) { // right
        startAngle = 330*16; spanAngle = 300*16;
    } else if (pacman.dirR == 0 && pacman.dirC == -1) { // left
        startAngle = 150*16; spanAngle = 300*16;
    } else if (pacman.dirR == -1 && pacman.dirC == 0) { // up
        startAngle = 210*16; spanAngle = 300*16;
    } else if (pacman.dirR == 1 && pacman.dirC == 0) { // down
        startAngle = 30*16; spanAngle = 300*16;
    } else {

        startAngle = 330*16; spanAngle = 300*16;
    }

    p.setBrush(Qt::yellow);
    p.setPen(Qt::NoPen);
    p.drawPie(pacRect, startAngle, spanAngle);

    // score
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(12);
    p.setFont(f);
    p.drawText(4, height() - 8, QString("Score: %1").arg(score));
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Z)       { pacman.setDirection(1, 0); } // abajo
    else if (event->key() == Qt::Key_D)  { pacman.setDirection(0, 1); } // derecha
    else if (event->key() == Qt::Key_S)  { pacman.setDirection(-1, 0); } // arriba
    else if (event->key() == Qt::Key_A)  { pacman.setDirection(0, -1); } // izquierda

    else if (event->key() == Qt::Key_Up)    { pacman.setDirection(-1,0); }
    else if (event->key() == Qt::Key_Down)  { pacman.setDirection(1,0); }
    else if (event->key() == Qt::Key_Left)  { pacman.setDirection(0,-1); }
    else if (event->key() == Qt::Key_Right) { pacman.setDirection(0,1); }

    QWidget::keyPressEvent(event);
}

void GameWidget::tick()
{
    pacman.tryMove(maze);
    pacman.applyWrap(maze.cols);

    checkEatDot();

    update();
}

void GameWidget::checkEatDot()
{
    if (maze.inBounds(pacman.r, pacman.c) && maze.hasDot(pacman.r, pacman.c)) {
        maze.eatDot(pacman.r, pacman.c);
        score += 10;
    }
}
