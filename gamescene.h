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
    Q_PROPERTY(int highestLevel READ highestLevel WRITE setHighestLevel NOTIFY highestLevelChanged)
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(int levelTime READ levelTime WRITE setLevelTime NOTIFY levelTimeChanged)
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

    QString adjustPath(const QString &path);

    bool isDemo();

    void showGameMenu();

    int remainingNegativeCharges;
    int remainingPositiveCharges;
    QTime time;
    int m_levelTime;
    QTimer *levelTimer;
    QTimer *instructionTimer;
    QTime instructionTime;
    QTime menuTime;

    void setHighestLevel(int alevel) {
        m_highestLevel = alevel;
        emit highestLevelChanged(alevel);
    }
    int highestLevel() {
        return m_highestLevel;
    }

    void setLevel(int alevel) {
        m_level = alevel;
        emit levelChanged(alevel);
    }
    int level() {
        return m_level;
    }
    void setLevelTime(int alevelTime) {
        m_levelTime = alevelTime;
        emit levelTimeChanged(alevelTime);
    }
    int levelTime() {
        return m_levelTime;
    }


signals:
    void highestLevelChanged(int);
    void levelChanged(int);
    void levelTimeChanged(int);

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
    void minimizeToDashboard();

private:
    int m_highestLevel;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int m_level;
    int instructionNumber;
    int _gameState;
    int selectedParticleType;
    float _dt; // time difference in seconds

    void startLevel(int m_level);

    QTimer timer;

    bool firstStep;
    bool firstResize;

    QGraphicsTextItem *timerText;
    QGraphicsTextItem *levelText;
    QGraphicsTextItem *instructionText;
    QGraphicsTextItem *menuTitleText;
    QGraphicsTextItem *remainingPositiveChargesText;
    QGraphicsTextItem *remainingNegativeChargesText;

    // QML Components
    QGraphicsObject *aboutDialog;
    QGraphicsObject *mainMenu;

    void removeNegativeCharge();
    void removePositiveCharge();

    void updateRemainingChargeText();

    QSettings settings;

};

#endif // GAMESCENE_H
