#include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <cmath>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent),
    maze(), pacman(), timer(this),
    score(0), cellSize(24),
    mouthOpen(true), mouthToggleCounter(0),
    waka(nullptr), wakaOutput(nullptr),
    intro(nullptr), introOutput(nullptr),
    death(nullptr), deathOutput(nullptr),
    ghostRed(), lives(3), gameOver(false)
{
    setWindowTitle("Pac-Man - Práctica 5");
    loadDefaultMaze();


    pacman.r = 1; pacman.c = 1;
    pacman.dirR = 0; pacman.dirC = 0;

    ghostRed.r = 9; ghostRed.c = maze.cols - 2;
    ghostRed.dirR = 0; ghostRed.dirC = 0;

    waka = new QMediaPlayer(this);
    wakaOutput = new QAudioOutput(this);
    waka->setAudioOutput(wakaOutput);
    waka->setSource(QUrl::fromLocalFile("sounds/waka.wav"));
    wakaOutput->setVolume(0.35);

    intro = new QMediaPlayer(this);
    introOutput = new QAudioOutput(this);
    intro->setAudioOutput(introOutput);
    intro->setSource(QUrl::fromLocalFile("sounds/intro.wav"));
    introOutput->setVolume(0.8);

    death = new QMediaPlayer(this);
    deathOutput = new QAudioOutput(this);
    death->setAudioOutput(deathOutput);
    death->setSource(QUrl::fromLocalFile("sounds/death.wav"));
    deathOutput->setVolume(0.9);

    // reproducir intro al empezar
    playIntro();

    connect(&timer, &QTimer::timeout, this, &GameWidget::tick);
    timer.start(140);

    setFixedSize(maze.cols * cellSize, maze.rows * cellSize + 40);
}

GameWidget::~GameWidget()
{
    delete waka;
    delete wakaOutput;
    delete intro;
    delete introOutput;
    delete death;
    delete deathOutput;
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
        {0,0,0,2,1,2,0,0,0,0,0,0,0,2,1,2,0,0,0},
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

    // fondo
    p.fillRect(rect(), Qt::black);

    // laberinto y puntos
    for (int r=0; r<maze.rows; ++r) {
        for (int c=0; c<maze.cols; ++c) {
            int x = c * cellSize;
            int y = r * cellSize;
            if (maze.grid[r][c] == 2) {
                p.fillRect(x, y, cellSize, cellSize, Qt::blue);
            } else if (maze.grid[r][c] == 1) {
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

    // fantasma rojo
    {
        int gx = ghostRed.c * cellSize;
        int gy = ghostRed.r * cellSize;
        int margin = 2;
        QRect gRect(gx + margin, gy + margin, cellSize - 2*margin, cellSize - 2*margin);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::red);
        p.drawEllipse(gRect);
    }

    // pacman con animacion de boca
    int px = pacman.c * cellSize;
    int py = pacman.r * cellSize;
    int margin = 2;
    QRect pacRect(px + margin, py + margin, cellSize - 2*margin, cellSize - 2*margin);

    int startAngle = 330*16;
    int spanAngle = mouthOpen ? 240*16 : 360*16;
    if (pacman.dirR == 0 && pacman.dirC == 1) { // derecha
        startAngle = mouthOpen ? 330*16 : 0;
    } else if (pacman.dirR == 0 && pacman.dirC == -1) { // izquierda
        startAngle = mouthOpen ? 150*16 : 0;
    } else if (pacman.dirR == -1 && pacman.dirC == 0) { // arriba
        startAngle = mouthOpen ? 210*16 : 0;
    } else if (pacman.dirR == 1 && pacman.dirC == 0) { // abajo
        startAngle = mouthOpen ? 30*16 : 0;
    } else {
        startAngle = mouthOpen ? 330*16 : 0;
    }

    p.setBrush(Qt::yellow);
    p.setPen(Qt::NoPen);
    p.drawPie(pacRect, startAngle, spanAngle);

    // score y vidas
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(12);
    p.setFont(f);
    p.drawText(4, height() - 18, QString("Score: %1").arg(score));
    p.drawText(120, height() - 18, QString("Lives: %1").arg(lives));

    // si game over: mensaje grande
    if (gameOver) {
        QFont f2 = p.font();
        f2.setPointSize(28);
        f2.setBold(true);
        p.setFont(f2);
        p.setPen(Qt::yellow);
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER\nPress R to restart");
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (gameOver) {
        if (event->key() == Qt::Key_R) {
            resetGame();
            return;
        } else {
            return;
        }
    }

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
    if (gameOver) return;

    mouthToggleCounter++;
    if (mouthToggleCounter >= 2) {
        mouthOpen = !mouthOpen;
        mouthToggleCounter = 0;
    }

    pacman.tryMove(maze);
    pacman.applyWrap(maze.cols);

    // mover fantasma
    ghostRed.randomStep(maze);

    // comprobar recolección de puntos
    checkEatDot();

    // colisiones con fantasma
    checkGhostCollisions();

    update();
}

void GameWidget::checkEatDot()
{
    if (maze.inBounds(pacman.r, pacman.c) && maze.hasDot(pacman.r, pacman.c)) {
        maze.eatDot(pacman.r, pacman.c);
        score += 10;
        playWaka();
    }
}

void GameWidget::checkGhostCollisions()
{
    if (pacman.r == ghostRed.r && pacman.c == ghostRed.c) {

        lives--;
        playDeath();

        if (lives <= 0) {
            // game over
            gameOver = true;
            timer.stop();
            if (waka->playbackState() == QMediaPlayer::PlayingState) waka->stop();
            return;
        } else {
            resetPositions();
        }
    }
}

void GameWidget::resetPositions()
{

    pacman.r = 1; pacman.c = 1; pacman.dirR = 0; pacman.dirC = 0;
    ghostRed.r = 9; ghostRed.c = maze.cols - 2; ghostRed.dirR = 0; ghostRed.dirC = 0;
}

void GameWidget::resetGame()
{
    score = 0;
    lives = 3;
    gameOver = false;
    loadDefaultMaze();
    resetPositions();
    playIntro();
    timer.start(140);
    update();
}

void GameWidget::playIntro()
{
    if (!intro) return;
    intro->stop();
    intro->play();
}

void GameWidget::playWaka()
{
    if (!waka) return;
    if (waka->playbackState() == QMediaPlayer::PlayingState) {
        waka->stop();
    }
    waka->play();
}

void GameWidget::playDeath()
{
    if (!death) return;
    death->stop();
    death->play();
}
