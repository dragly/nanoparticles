#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QtGui>

#include <QTimer>
#include <QTime>

const QString NANOPARTICLES_VERSION = QString("2.0.1");

class Particle;
class GameView;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(int highestLevel READ highestLevel WRITE setHighestLevel NOTIFY highestLevelChanged)
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(double timeFactor READ timeFactor WRITE setTimeFactor)
    Q_PROPERTY(int levelTime READ levelTime WRITE setLevelTime NOTIFY levelTimeChanged)
    Q_PROPERTY(GameState gameState READ gameState WRITE setGameState NOTIFY gameStateChanged)
    Q_PROPERTY(GameMode gameMode READ gameMode WRITE setGameMode NOTIFY gameModeChanged)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)
    Q_PROPERTY(int remainingPositiveCharges READ remainingPositiveCharges WRITE setRemainingPositiveCharges NOTIFY remainingPositiveChargesChanged)
    Q_PROPERTY(int remainingNegativeCharges READ remainingNegativeCharges WRITE setRemainingNegativeCharges NOTIFY remainingNegativeChargesChanged)
    Q_PROPERTY(int remainingSpecialCharges READ remainingSpecialCharges NOTIFY remainingSpecialChargesChanged)
    Q_PROPERTY(QList<int> *specialParticles READ specialParticles)
    Q_PROPERTY(Selection selectedType READ selectedType WRITE setSelectedType NOTIFY selectedTypeChanged)
    Q_PROPERTY(bool levelUpgrade READ levelUpgrade WRITE setLevelUpgrade NOTIFY levelUpgradeChanged)
    Q_PROPERTY(bool isSlowMotionEnabled READ isSlowMotionEnabled WRITE setSlowMotionEnabled NOTIFY slowMotionEnabledChanged)
    Q_PROPERTY(Platform platform READ platform)
    Q_ENUMS(GameMode)
    Q_ENUMS(GameState)
    Q_ENUMS(Selection)
    Q_ENUMS(ViewMode)
    Q_ENUMS(Platform)
public:
    explicit GameScene(GameView *view = 0);

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
    enum Selection { ParticleNegative, ParticlePositive, ParticleSpecial };
    enum GameMode { ModeClassic, ModeParty };
    enum ViewMode { ViewNormal, ViewFullScreen };
    enum Platform {
        Maemo,
        Harmattan,
        Symbian,
        Android,
        MacOSX,
        DesktopLinux,
        Windows
    };

    //Time variables
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
    QImage glowingImage;
    QImage selectionImage;
    QImage transferImage;
    QImage repellentImage;
    QRectF gameRectF();
    double toFp(double number, bool useSmallest = false) const;
    double fromFp(double number, bool useSmallest = false) const;
    void resized();
    void setGameState(GameState gameState);
    GameState gameState() {return m_gameState;}
    Platform platform() {
#ifdef OS_IS_HARMATTAN
        return Harmattan;
#elif defined(OS_IS_ANDROID)
        return Android;
#elif defined(Q_WS_MAEMO_5)
        return Maemo;
#elif defined(OS_IS_SYMBIAN)
        return Symbian;
#elif defined(OS_IS_DESKTOP_LINUX)
        return DesktopLinux;
#endif
    }

    QString adjustPath(const QString &path);

    Q_INVOKABLE bool isDemo();


    void showGameMenu();

    QTime time;
    int m_levelTime;
    QTimer *levelTimer;
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

    QList<int> *specialParticles() const
    {
        return m_specialParticles;
    }

    int remainingSpecialCharges() const
    {
        return m_remainingSpecialCharges;
    }

    void addTransferParticle();
    Selection selectedType() const
    {
        return m_selectedType;
    }

    QString version() const
    {
        return NANOPARTICLES_VERSION;
    }

    ViewMode viewMode() const
    {
        return m_viewMode;
    }

    bool levelUpgrade() const
    {
        return m_levelUpgrade;
    }

    bool isSlowMotionEnabled() const
    {
        return m_isSlowMotionEnabled;
    }

signals:
    void highestLevelChanged(int);
    void levelChanged(int);
    void levelTimeChanged(int);
    void gameStateChanged(GameState);
    void gameModeChanged(GameMode);

    void remainingPositiveChargesChanged(int arg);

    void remainingNegativeChargesChanged(int arg);

    void remainingSpecialChargesChanged(int arg);

    void selectedTypeChanged(Selection arg);

    void versionChanged(QString arg);

    void viewModeChanged(ViewMode arg);

    void levelUpgradeChanged(bool arg);

    void slowMotionEnabledChanged(bool arg);

public slots:
    void enableSlowMotion(int time);
    void disableSlowMotion();
    void advance();
    void continueGame();
    void pauseGame();
    void retryGame();
    void exitGame();
    void updateLevelTime();
    void gameOver();
    void minimizeToDashboard();
    void levelUp();
    void levelDown();
    void clickedDashboardButton();
    void setLevel(int level);

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

    void setRemainingSpecialCharges(int arg)
    {
        if (m_remainingSpecialCharges != arg) {
            m_remainingSpecialCharges = arg;
            emit remainingSpecialChargesChanged(arg);
        }
    }

    void setSelectedType(Selection arg)
    {
        if (m_selectedType != arg) {
            m_selectedType = arg;
            emit selectedTypeChanged(arg);
        }
    }
    void setViewMode(ViewMode arg);

    void setLevelUpgrade(bool arg)
    {
        if (m_levelUpgrade != arg) {
            m_levelUpgrade = arg;
            emit levelUpgradeChanged(arg);
        }
    }

    void setSlowMotionEnabled(bool arg)
    {
        if (m_isSlowMotionEnabled != arg) {
            m_isSlowMotionEnabled = arg;
            emit slowMotionEnabledChanged(arg);
        }
    }

private:
    void addEnemies();
    int m_highestLevel;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int m_level;
    GameState m_gameState;
    GameMode m_gameMode;
    float m_dt; // time difference in seconds
    double m_timeFactor;

    QTimer advanceTimer;

    bool firstStep;

    QGraphicsTextItem *timerText;
    QGraphicsTextItem *levelText;
    QGraphicsTextItem *menuTitleText;
    QGraphicsTextItem *remainingPositiveChargesText;
    QGraphicsTextItem *remainingNegativeChargesText;

    QGraphicsPixmapItem *backgroundImage;

    // QML Components
    QGraphicsObject *aboutDialog;
    QGraphicsObject *mainMenu;

    // Particles
    Particle *player;

    // charges
    int m_remainingPositiveCharges;
    int m_remainingNegativeCharges;

    void updateRemainingChargeText();

    QSettings settings;

    QList<int> *m_specialParticles;
    int m_remainingSpecialCharges;
    Selection m_selectedType;
    QString m_version;
    ViewMode m_viewMode;
    bool m_levelUpgrade;
    bool m_isSlowMotionEnabled;

    //Time variables
    int currentTime;
    int lastFrameTime;
    int lastSpecialSpawnTime;
    int frameNumber;
    double dtSum;
};
Q_DECLARE_METATYPE(GameScene::GameMode)
Q_DECLARE_METATYPE(GameScene::GameState)
Q_DECLARE_METATYPE(GameScene::Selection)
Q_DECLARE_METATYPE(GameScene::ViewMode)


#endif // GAMESCENE_H
