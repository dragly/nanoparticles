#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QtGui>

#include <QTimer>
#include <QTime>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(int highestLevel READ highestLevel WRITE setHighestLevel NOTIFY highestLevelChanged)
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(double timeFactor READ timeFactor WRITE setTimeFactor)
    Q_PROPERTY(int levelTime READ levelTime WRITE setLevelTime NOTIFY levelTimeChanged)
    Q_PROPERTY(GameState gameState READ gameState WRITE setGameState NOTIFY gameStateChanged)
    Q_PROPERTY(GameMode gameMode READ gameMode WRITE setGameMode NOTIFY gameModeChanged)
    Q_PROPERTY(int remainingPositiveCharges READ remainingPositiveCharges WRITE setRemainingPositiveCharges NOTIFY remainingPositiveChargesChanged)
    Q_PROPERTY(int remainingNegativeCharges READ remainingNegativeCharges WRITE setRemainingNegativeCharges NOTIFY remainingNegativeChargesChanged)
    Q_ENUMS(GameMode)
    Q_ENUMS(GameState)
public:
    explicit GameScene(QObject *parent = 0);

    /*!
      Defines the state of the game
      */
    enum GameState{
        GameStarted, /*! The game has just started */
        GameRunning, /*! The game is running */
        GamePaused,  /*! The game has been paused */
        GameOver,    /*! The level was failed */
        GameInstructionPause /*! The game is running, but we are showing instructions on screen */
    };
    enum ParticleType{ParticleNegative, ParticlePositive};
    enum GameMode{ ModeClassic, ModeParty };

    //Time variables
    int currentTime;
    int lastFrameTime;
    int lastSpecialSpawnTime;
    double timeFactor() {
        return m_timeFactor;
    }
    void setTimeFactor(double timeFactor) {
        m_timeFactor = timeFactor;
    }

    float dt(); // time difference in seconds, should never fall below 20fps
    // Image variables
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    QImage slowMotionImage;
    QImage enemyImage;
    QImage playerImage;
    QImage playerOverchargedImage;
    QImage selectionImage;
    QRectF gameRectF();
    double toFp(double number, bool useSmallest = false) const;
    double fromFp(double number, bool useSmallest = false) const;
    void resized();
    void setGameState(GameState gameState);
    GameState gameState() {return m_gameState;}

    QString adjustPath(const QString &path);

    bool isDemo();

    void showGameMenu();

    QTime time;
    int m_levelTime;
    QTimer *levelTimer;
    QTimer *instructionTimer;
    QTime instructionTime;
    QTime menuTime;

    GameMode gameMode() { return m_gameMode; }
    void setGameMode(GameMode gameMode);

    void setHighestLevel(int alevel) {
        m_highestLevel = alevel;
        emit highestLevelChanged(alevel);
    }
    int highestLevel() {
        return m_highestLevel;
    }

    void setLevel(int level);
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


    void enableSlowMotion(int time);
    QPropertyAnimation *timeFactorAnimation;


    void checkAddSpecialParticle();
    int remainingPositiveCharges() const
    {
        return m_remainingPositiveCharges;
    }

    int remainingNegativeCharges() const
    {
        return m_remainingNegativeCharges;
    }

    void disableSlowMotion();
signals:
    void highestLevelChanged(int);
    void levelChanged(int);
    void levelTimeChanged(int);
    void gameStateChanged(GameState);
    void gameModeChanged(GameMode);

    void remainingPositiveChargesChanged(int arg);

    void remainingNegativeChargesChanged(int arg);

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

    void setRemainingNegativeCharges(int arg)
    {
        if (m_remainingNegativeCharges != arg) {
            m_remainingNegativeCharges = arg;
            emit remainingNegativeChargesChanged(arg);
        }
    }

    void setRemainingPositiveCharges(int arg)
    {
        if (m_remainingPositiveCharges != arg) {
            m_remainingPositiveCharges = arg;
            emit remainingPositiveChargesChanged(arg);
        }
    }

private:
    void addEnemies();
    int m_highestLevel;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int m_level;
    int instructionNumber;
    GameState m_gameState;
    GameMode m_gameMode;
    int selectedParticleType;
    float m_dt; // time difference in seconds
    double m_timeFactor;

    QTimer timer;

    bool firstStep;
    bool firstResize;

    QGraphicsTextItem *timerText;
    QGraphicsTextItem *levelText;
    QGraphicsTextItem *instructionText;
    QGraphicsTextItem *menuTitleText;
    QGraphicsTextItem *remainingPositiveChargesText;
    QGraphicsTextItem *remainingNegativeChargesText;

    QGraphicsPixmapItem *backgroundImage;

    // QML Components
    QGraphicsObject *aboutDialog;
    QGraphicsObject *mainMenu;

    // charges
    int m_remainingPositiveCharges;
    int m_remainingNegativeCharges;

    void removeNegativeCharge();
    void removePositiveCharge();

    void updateRemainingChargeText();

    QSettings settings;

};
Q_DECLARE_METATYPE(GameScene::GameMode)
Q_DECLARE_METATYPE(GameScene::GameState)


#endif // GAMESCENE_H
