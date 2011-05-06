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

    enum GameState{GameStarted, GameRunning, GamePaused, GameOver, GameInstructionPause};
    enum ParticleType{ParticleNegative, ParticlePositive};

    int currentTime;
    int lastFrameTime;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    QImage enemyImage;
    QImage playerImage;
    QImage playerOverchargedImage;
    QImage selectionImage;
    float dt(); // time difference in seconds, should never fall below 20fps
    QRectF gameRectF();
    double toFp(double number, bool useSmallest = false) const;
    double fromFp(double number, bool useSmallest = false) const;
    void resized();
    void setGameState(int gameState);
    int gameState() {return _gameState;}

    bool isDemo();

    void showGameMenu();

    int remainingNegativeCharges;
    int remainingPositiveCharges;
    QTime time;
    int levelTime;
    QTimer *levelTimer;
    QTimer *instructionTimer;
    QTime instructionTime;
    QTime menuTime;

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
    void toggleInstructionText();
    void animateMenuIn();
    void showAboutDialog();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int level;
    int instructionNumber;
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
    Button* aboutDialogButton;
    Button* exitButton;
    Button* nextLevelButton;
    Button* prevLevelButton;

    // menu background and effect
    QGraphicsPixmapItem *menuBackgroundRect;
    QGraphicsBlurEffect menuBackgroundBlur;
    QGraphicsRectItem *gameMenuBackgroundRect;

    QGraphicsTextItem *timerText;
    QGraphicsTextItem *levelText;
    QGraphicsTextItem *instructionText;
    QGraphicsTextItem *menuTitleText;
    QGraphicsTextItem *remainingPositiveChargesText;
    QGraphicsTextItem *remainingNegativeChargesText;

    // About dialog
    QGraphicsObject *aboutDialog;

    void removeNegativeCharge();
    void removePositiveCharge();

    void updateRemainingChargeText();

    QSettings settings;

};

#endif // GAMESCENE_H
