#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QtGui>

#include <QTimer>
#include <QTime>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = 0);

    enum GameState{GameRunning, GamePaused, GameMenu};
    enum ParticleType{ParticleNegative, ParticlePositive};

    int currentTime;
    int lastFrameTime;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    float dt(); // time difference in seconds, should never fall below 20fps
    QRectF gameRectF();

signals:

public slots:
    void advance();
    void clickedPositiveButton();
    void clickedNegativeButton();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int level;
    int gameState;
    int selectedParticleType;
    float _dt; // time difference in seconds

    void startLevel(int level);

    QTimer timer;
    QTime time;

    bool firstStep;
    QPushButton* positiveButton;
    QPushButton* negativeButton;

};

#endif // GAMESCENE_H
