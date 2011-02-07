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
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    float dt(); // time difference in seconds, should never fall below 20fps
signals:

public slots:
    void advance();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int level;
    int gameState;
    float _dt; // time difference in seconds

    void startLevel(int level);

    QTimer timer;
    QTime time;

    bool firstStep;

};

#endif // GAMESCENE_H
