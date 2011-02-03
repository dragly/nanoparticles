#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>

#include <QTimer>
#include <QTime>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = 0);

    enum GameState{GameRunning, GamePaused, GameMenu};
    int currentTime;
    int lastFrameTime;
    float dt; // time difference in seconds
signals:

public slots:
    void advance();

private:
    int level;
    int gameState;

    void startLevel(int level);

    QTimer timer;
    QTime time;

    bool firstStep;

};

#endif // GAMESCENE_H
