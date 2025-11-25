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
    // Juego
    Maze maze;
    Pacman pacman;
    QTimer timer;
    int score;
    int cellSize;

    // Animación boca
    bool mouthOpen;
    int mouthToggleCounter;

    QMediaPlayer *waka;
    QAudioOutput *wakaOutput;
    QMediaPlayer *intro;
    QAudioOutput *introOutput;
    QMediaPlayer *death;
    QAudioOutput *deathOutput;

    // Fantasma(s)
    Ghost ghostRed;

    // Vidas
    int lives;
    bool gameOver;
    void resetPositions();
    void resetGame();

    // Funciones util
    void loadDefaultMaze();
    void checkEatDot();
    void checkGhostCollisions();

    void playIntro();
    void playWaka();
    void playDeath();
};

#endif // GAMEWIDGET_H
