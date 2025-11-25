#include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <cmath>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent),
    maze(), pacman(), ghostRed(),
    timer(this),
    score(0), cellSize(24),
    mouthOpen(true), mouthToggleCounter(0),
    waka(nullptr), wakaOutput(nullptr),
    intro(nullptr), introOutput(nullptr),
    death(nullptr), deathOutput(nullptr),
    lives(3), gameOver(false),youWin(false),
    showMenu(true), introPlaying(false)

{
    setWindowTitle("Pac-Man - Práctica 5");
    loadDefaultMaze();

    pacman.r = 1; pacman.c = 1;
    ghostRed.r = 7;
    ghostRed.c = 9;


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

    connect(intro, &QMediaPlayer::playbackStateChanged, this,
            [=](QMediaPlayer::PlaybackState s) {
                if (introPlaying && s == QMediaPlayer::StoppedState) {
                    introPlaying = false;
                }
            });

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

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), Qt::black);

    for (int r=0; r<maze.rows; r++){
        for (int c=0; c<maze.cols; c++){
            int x=c*cellSize, y=r*cellSize;
            if (maze.grid[r][c]==2)
                p.fillRect(x,y,cellSize,cellSize,Qt::blue);
            else if (maze.grid[r][c]==1){
                p.setBrush(Qt::white);
                p.setPen(Qt::NoPen);
                p.drawEllipse(x+cellSize/2-3, y+cellSize/2-3, 6,6);
            }
        }
    }

    // Fantasma
    p.setBrush(Qt::red);
    p.setPen(Qt::NoPen);
    p.drawEllipse(ghostRed.c*cellSize+3, ghostRed.r*cellSize+3,
                  cellSize-6, cellSize-6);

    int px = pacman.c * cellSize;
    int py = pacman.r * cellSize;

    int startAngle = 330*16;
    int spanAngle = mouthOpen ? 240*16 : 360*16;

    if (pacman.dirR==0 && pacman.dirC==1) startAngle=330*16;
    else if (pacman.dirR==0 && pacman.dirC==-1) startAngle=150*16;
    else if (pacman.dirR==-1 && pacman.dirC==0) startAngle=210*16;
    else if (pacman.dirR==1 && pacman.dirC==0) startAngle=30*16;

    p.setBrush(Qt::yellow);
    p.drawPie(px+3, py+3, cellSize-6, cellSize-6, startAngle, spanAngle);

    // Score y vidas
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(12);
    p.setFont(f);

    p.drawText(4, height()-18, QString("Score: %1").arg(score));
    p.drawText(120, height()-18, QString("Lives: %1").arg(lives));

    if (showMenu) {
        QFont f2 = p.font();
        f2.setPointSize(30);
        f2.setBold(true);

        p.setFont(f2);
        p.setPen(Qt::yellow);

        // Fondo semitransparente opcional
        p.fillRect(rect(), QColor(0,0,0,180));

        p.drawText(rect(), Qt::AlignCenter,
                   "PACMAN\n\nPresiona R para iniciar");
    }

    if (gameOver){
        QFont f2=f;
        f2.setPointSize(28);
        f2.setBold(true);
        p.setFont(f2);
        p.setPen(Qt::yellow);
        p.drawText(rect(), Qt::AlignCenter,
                   "GAME OVER\nPresiona R para reiniciar");
    }

    if (youWin) {
        QFont f2 = p.font();
        f2.setPointSize(28);
        f2.setBold(true);
        p.setFont(f2);
        p.setPen(Qt::yellow);

        p.drawText(rect(), Qt::AlignCenter,
                   "FELICIDADES, GANASTE!\n\nPresiona R para reiniciar");

        return;
    }

}




void GameWidget::keyPressEvent(QKeyEvent *event)
{
    // MENÚ INICIAL
    if (showMenu && event->key()==Qt::Key_R){
        showMenu = false;
        introPlaying = true;
        playIntro();
        return;
    }
    // GAME OVER
    if (gameOver){
        if (event->key()==Qt::Key_R){
            resetGame();
        }
        return;
    }

    // GANASTE
    if (youWin) {
        if (event->key() == Qt::Key_R) {
            resetGame();
        }
        return;
    }


    if (introPlaying)
        return;

    // Movimiento Pacman
    if (event->key() == Qt::Key_Z) pacman.setDirection(1, 0);
    else if (event->key() == Qt::Key_D) pacman.setDirection(0, 1);
    else if (event->key() == Qt::Key_S) pacman.setDirection(-1, 0);
    else if (event->key() == Qt::Key_A) pacman.setDirection(0, -1);

    else if (event->key() == Qt::Key_Up) pacman.setDirection(-1,0);
    else if (event->key() == Qt::Key_Down) pacman.setDirection(1,0);
    else if (event->key() == Qt::Key_Left) pacman.setDirection(0,-1);
    else if (event->key() == Qt::Key_Right) pacman.setDirection(0,1);
}

void GameWidget::tick()
{
    if (showMenu || gameOver) return;

    if (introPlaying) return;

    mouthToggleCounter++;
    if (mouthToggleCounter >= 2){
        mouthOpen = !mouthOpen;
        mouthToggleCounter = 0;
    }

    pacman.tryMove(maze);
    pacman.applyWrap(maze.cols);

    ghostRed.randomStep(maze);

    checkEatDot();
    checkGhostCollisions();

    // --------------------------
    // DETECTAR SI GANÓ
    // --------------------------
    bool anyDot = false;
    for (int r = 0; r < maze.rows; r++) {
        for (int c = 0; c < maze.cols; c++) {
            if (maze.grid[r][c] == 1) {
                anyDot = true;
                break;
            }
        }
        if (anyDot) break;
    }

    if (!anyDot) {
        youWin = true;
        timer.stop();
    }


    update();
}

void GameWidget::checkEatDot()
{
    if (maze.inBounds(pacman.r,pacman.c) && maze.hasDot(pacman.r,pacman.c)){
        maze.eatDot(pacman.r,pacman.c);
        score += 10;
        playWaka();
    }
}

void GameWidget::checkGhostCollisions()
{
    if (pacman.r==ghostRed.r && pacman.c==ghostRed.c){
        lives--;
        playDeath();

        if (lives<=0){
            gameOver = true;
            timer.stop();
            return;
        } else {
            resetPositions();
        }
    }
}

void GameWidget::resetPositions()
{
    pacman.r=1; pacman.c=1; pacman.dirR=0; pacman.dirC=0;
    ghostRed.r = 7;
    ghostRed.c = 9;

}

void GameWidget::resetGame()
{
    score = 0;
    lives = 3;
    gameOver = false;
    youWin = false;
    showMenu = true;
    introPlaying = false;

    resetPositions();
    loadDefaultMaze();

    timer.start(140);
    update();
}

void GameWidget::playIntro()
{
    introPlaying = true;
    intro->stop();
    intro->play();
}

void GameWidget::playWaka()
{
    if (waka->playbackState()==QMediaPlayer::PlayingState)
        waka->stop();
    waka->play();
}

void GameWidget::playDeath()
{
    death->stop();
    death->play();
}
