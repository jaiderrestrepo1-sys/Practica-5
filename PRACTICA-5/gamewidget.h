#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "maze.h"
#include "pacman.h"
#include "ghost.h"

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void tick();

private:

    bool showMenu;
    bool introPlaying;
    bool gameOver;
    bool youWin;

    // Juego
    Maze maze;
    Pacman pacman;
    Ghost ghostRed;
    Ghost ghostPink;
    Ghost ghostBlue;
    QTimer timer;
    int score;
    int cellSize;

    // ANIMACIÓN BOCA
    bool mouthOpen;
    int mouthToggleCounter;

    // SONIDOS
    QMediaPlayer *waka;
    QAudioOutput *wakaOutput;

    QMediaPlayer *intro;
    QAudioOutput *introOutput;

    QMediaPlayer *death;
    QAudioOutput *deathOutput;

    // VIDAS
    int lives;
    void loadDefaultMaze();
    void checkEatDot();
    void checkGhostCollisions();

    void resetPositions();
    void resetGame();

    // SONIDOS
    void playIntro();
    void playWaka();
    void playDeath();
};

#endif // GAMEWIDGET_H
