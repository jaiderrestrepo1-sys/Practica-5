#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "maze.h"
#include "pacman.h"

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void tick();
private:
    Maze maze;
    Pacman pacman;
    QTimer timer;
    int score;
    int cellSize;
    void loadDefaultMaze();
    void checkEatDot();
};

#endif
