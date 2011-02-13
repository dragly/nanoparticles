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

    enum GameState{GameStarted, GameRunning, GamePaused, GameOver};
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
    double toFp(double number, bool useSmallest = false) const;
    double fromFp(double number, bool useSmallest = false) const;
    void resized();
    void setGameState(int gameState);
    int gameState() {return _gameState;}

    void showGameMenu();

    int remainingNegativeCharges;
    int remainingPositiveCharges;
    QTime time;
    int levelTime;
    QTimer *levelTimer;

signals:

public slots:
    void advance();
    void clickedPositiveButton();
    void clickedNegativeButton();
    void continueGame();
    void pauseGame();
    void retryGame();
    void exitGame();
    void clickedNextLevelButton();
    void clickedPrevLevelButton();
    void updateTime();
    void gameOver();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int level;
    int _gameState;
    int selectedParticleType;
    float _dt; // time difference in seconds

    void startLevel(int level);

    void prepareButton(Button* button);

    QTimer timer;

    bool firstStep;

    // right menu buttons
    Button* positiveButton;
    Button* negativeButton;
    Button* pauseGameButton;

    // menu buttons
    Button* continueButton;
    Button* retryButton;
    Button* exitButton;
    Button* nextLevelButton;
    Button* prevLevelButton;

    // menu background and effect
    QGraphicsRectItem *menuBackgroundRect;
    QGraphicsBlurEffect menuBackgroundBlur;

    QGraphicsTextItem *timerText;
    QGraphicsTextItem *menuTitleText;

    void removeNegativeCharge();
    void removePositiveCharge();

};

#endif // GAMESCENE_H
