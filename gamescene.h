#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QtGui>

#include <QTimer>
#include <QTime>

class Button;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = 0);

    enum GameState{GameRunning, GamePaused};
    enum ParticleType{ParticleNegative, ParticlePositive};

    int currentTime;
    int lastFrameTime;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    QImage enemyImage;
    QImage playerImage;
    QImage playerOverchargedImage;
    float dt(); // time difference in seconds, should never fall below 20fps
    QRectF gameRectF();
    double toFp(double number);
    double fromFp(double number);
    void resized();

    void showGameMenu();

    int remainingNegativeCharges;
    int remainingPositiveCharges;
    QTime time;

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

    bool firstStep;
    Button* positiveButton;
    Button* negativeButton;

    QGraphicsRectItem *menuBackgroundRect;

};

#endif // GAMESCENE_H
