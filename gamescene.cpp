#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative>

#ifdef Q_WS_MAEMO_5
#include <QtDBus/QtDBus>
#endif

/* TODO.
    // Finishing
    - Add copyright/version window.
    - Publish to Ovi Store
    - Add window focus lost event for Symbian

    // Demo version (1.1)
    - Add link to Ovi Store when demo is over.

    // To be tested (1.5)
    - Test with Meego SDK
    - Test with Android NDK (what about OpenGL? - how is the performance without it?)
    - Draw using FullViewPortUpdate - does it give any performance improvements?

    // Enhanced version (2.0)
    - New particles to play with
    - New enemy particles (megacharges)
    - Dummy particles (just in the way and sticky, but not dangerous)
    - Make it possible to place particles that are sticky after level 15 (or something)
    - Use health bar instead immediate death.
*/
// scaling
const qreal globalScale = 2.5;
// charges
const qreal enemyCharge = -7.8;
const qreal playerCharge = 7.5;
const qreal simpleCharge = 5.0;
const qreal levelChargeFactor = 0.00036;

// game area
const qreal gameWidth = 84;

const qreal incrementChargeNum = 1.1; // will be converted to int after multiplication with level number
const int baseChargeNum = 4;
// time
const int baseTime = 10;
const qreal timeIncrement = 0.9;

// z values
const int zInGameMenu = 91;
const int zInGameMenuNumbers = 92;
const int zInGameBackground = 90;
const int zMainMenu = 101;
const int zMainMenuBackground = 100;

// in game gui
const qreal instructionTextFontSize = 7;

GameScene::GameScene(QObject *parent) :
    QGraphicsScene(parent)
{
#ifdef Q_OS_ANDROID
    qDebug() << "Force syncing settings";
    settings.sync();
#endif
    if(isDemo()) {
        qDebug() << "This is the demo version";
    } else {
        qDebug() << "This is the full version";
    }
    _gameState = GameRunning;
    _dt = 0;
    firstStep = true;

    selectedParticleType = ParticlePositive;

    // load settings
    setLevel(settings.value("highestLevel", 1).toInt());
    setHighestLevel(m_level);
    qDebug() << "Highest level is" << m_level;

    // load images
    selectionImage = QImage(":/images/selection-overlay.png");
    positiveImage = QImage(":/images/particle-positive.png");
    negativeImage = QImage(":/images/particle-negative.png");
    neutralImage = QImage(":/images/particle-neutral.png");
    playerImage = QImage(":/images/particle-player.png");
    playerOverchargedImage = QImage(":/images/particle-player-overcharged.png");
    enemyImage = QImage(":/images/particle-enemy.png");

    setSceneRect(0, 0, 800, 600); // just for init
    setItemIndexMethod(QGraphicsScene::NoIndex);

    // timer text (level time left)
    QFont font;
#ifndef OS_IS_ANDROID
    font.setFamily("NovaSquare");
#endif
    // instruction text
    instructionText = addText("",font);
    instructionText->setDefaultTextColor(QColor(250,250,250,220));
    instructionText->setZValue(zMainMenu);
    instructionText->hide();
    // end add in-game menu

    //    menuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    //    menuBackgroundBlur.setBlurRadius(toFp(2));
    //    menuBackgroundRect->setGraphicsEffect(&menuBackgroundBlur);


#ifdef Q_WS_MAEMO_5
    dashboardButton = new Button();
    prepareButton(dashboardButton);
    dashboardButton->setPosition(dashboardButtonPosition);
    dashboardButton->setImage(":/images/button-dashboard.png");
    dashboardButton->setScale(14);
#endif

    // Main menu
    QDeclarativeEngine *engine = new QDeclarativeEngine;
#ifdef Q_OS_ANDROID
    qDebug() << "Setting base URL for Android to /";
    engine->setBaseUrl(QUrl::fromLocalFile("/"));
#endif
    QDeclarativeComponent mainMenuComponent(engine, QUrl::fromLocalFile(adjustPath("qml/MainMenu.qml")));

    engine->rootContext()->setContextProperty("gameScene", this);
    mainMenu = qobject_cast<QGraphicsObject *>(mainMenuComponent.create());
    qDebug() << "Component errors:\n" << mainMenuComponent.errors();
    qDebug() << "End component errors";
    // Connect the QML back to this class
    addItem(mainMenu);
    mainMenu->setZValue(1000);

#ifdef OS_IS_HARMATTAN
    mainMenu->setProperty("exit.visible", false);
#endif

#ifdef OS_IS_SYMBIAN
    qreal screenWidth = QApplication::desktop()->screenGeometry().width();
    qreal screenHeight = QApplication::desktop()->screenGeometry().height();
    if(screenWidth  > screenHeight) { // Symbian hack
        mainMenu->setProperty("width", screenWidth);
        mainMenu->setProperty("height", screenHeight);
    } else {
        mainMenu->setProperty("width", screenHeight);
        mainMenu->setProperty("height", screenWidth);
    }
#endif

    // set up timer
    levelTimer = new QTimer(this);
    levelTimer->setInterval(1000);
    connect(levelTimer, SIGNAL(timeout()), SLOT(updateTime()));
    // instruction timer
    instructionTimer = new QTimer(this);
    connect(instructionTimer, SIGNAL(timeout()), SLOT(toggleInstructionText()));
    // end set up timers

    setGameState(GameStarted);

#ifdef Q_WS_MAEMO_5
    connect(dashboardButton, SIGNAL(clicked()), SLOT(minimizeToDashboard()));
#endif


    // just init all in the resize() function
    resized();

    // Start level and start timers
    startLevel(level());

    QObject::connect(&timer, SIGNAL(timeout()), SLOT(advance()));
    timer.start(10);
    time.start();
    qDebug() << "Timers started!";
}

QString GameScene::adjustPath(const QString &path)
{
#ifdef Q_OS_ANDROID
    return path;
#endif
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    QString pathInInstallDir;
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    pathInInstallDir = QString::fromAscii("%1/../%2").arg(applicationDirPath, path);

    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
#endif
    return path;
}

bool GameScene::isDemo() {
    // Yes, I know this is far from bullet proof, and that I
    // should use the define check everywhere instead of letting crackers
    // easily modify this variable in memory. But hey, this is an open source game.
    // They could just have rebuilt the source if they wanted to :)
#ifdef ISDEMO
    return true;
#else
    return false;
#endif
}

void GameScene::resized() {
    //menuBackgroundRect->setRect(toFp(0), toFp(0), toFp(100), toFp(100));
    mainMenu->setProperty("width", width());
    mainMenu->setProperty("height", height());

    instructionText->setPos(toFp(5),toFp(5));
    instructionText->setTextWidth(toFp(gameWidth));
    // Reisze gui font
    QFont instructionFont = instructionText->font();
    instructionFont.setPixelSize((int)toFp(instructionTextFontSize,true));
    instructionText->setFont(instructionFont);
}

void GameScene::updateTime() {
    setLevelTime(levelTime() - 1);
    if(levelTime() < 1) {
        pauseGame();
        if(isDemo() && m_level >= 8) {

        } else {
            startLevel(m_level + 1);
        }
        if(highestLevel() < level()) {
            setHighestLevel(level());
        }
        if(settings.value("highestLevel", 1).toInt() < m_level) {
            settings.setValue("highestLevel", m_level);
        }
#ifdef Q_OS_ANDROID
    qDebug() << "Force syncing settings";
    settings.sync();
#endif
    }
}

void GameScene::continueGame() {
    if(gameState() == GameOver) {
        startLevel(m_level);
    }
    if(m_level == 1) {
        instructionTimer->start();
    }
    setGameState(GameRunning);

#ifdef Q_WS_MAEMO_5
    // TODO: Reimplement dashboard button hiding in QML
#endif

    // start timer
    levelTimer->start();
    // start the main timer as well (start using CPU again)
    timer.start();
    qDebug() << "Game started";
}

void GameScene::pauseGame() {
    bool wasGameRunning = false;
    if(gameState() == GameInstructionPause) {
        toggleInstructionText(); // Make sure we hide all text and everything if we just got paused.
    }
    if(gameState() == GameRunning) {
        menuTime.restart(); // start the timer that will avoid clicking on the quit button after level up or failed
        wasGameRunning = true;
        setGameState(GamePaused);
        // show main menu
        mainMenu->show();
        mainMenu->setProperty("state", "paused");
    }
    if(gameState() != GameStarted) {

    }
    if(gameState() == GameOver) {
        // show main menu
        mainMenu->show();
        mainMenu->setProperty("state", "gameOver");
    }
    instructionTimer->stop();

#ifndef OS_IS_HARMATTAN
    //    exitButton->show();
#endif

#ifdef Q_WS_MAEMO_5
    dashboardButton->show();
#endif
    // pause timer
    levelTimer->stop();
    qDebug() << "Game paused";
    // Stop the timer to save CPU power (hopefully this won't lock up anything else)
    timer.stop();
    // end animation
}

void GameScene::gameOver() {
    setGameState(GameOver);
    pauseGame();

    qDebug() << "Game over";
}

void GameScene::retryGame() {
    startLevel(m_level);
    continueGame();
}

void GameScene::exitGame() {
    QApplication::quit();
}

void GameScene::minimizeToDashboard() {
#ifdef Q_WS_MAEMO_5
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusMessage message = QDBusMessage::createSignal("/","com.nokia.hildon_desktop","exit_app_view");
    connection.send(message);
#else
    qDebug() << "This function is not implemented on anything but Maemo5.";
#endif
}

void GameScene::clickedNextLevelButton() {
    startLevel(m_level + 1);
    pauseGame();
}

void GameScene::clickedPrevLevelButton() {
    startLevel(m_level - 1);
    pauseGame();
}

void GameScene::clickedNegativeButton() {
    selectedParticleType = ParticleNegative;
    qDebug() << "Clicked negative";
}

void GameScene::clickedPositiveButton() {
    qDebug() << "Clicked positive";
    selectedParticleType = ParticlePositive;
}

void GameScene::setGameState(int gameState) {
    this->_gameState = gameState;
    if(gameState == GameStarted) {
        pauseGame();
    }
}

void GameScene::startLevel(int level) {
    qDebug() << "Starting level" << level;

    mainMenu->setProperty("level",level);

    qreal enemyChargeLevel = enemyCharge * (1 + levelChargeFactor * pow((double)level,2));

    instructionNumber = 1; // if there are instructions, start with the first one

    setLevel(level);

    foreach(QGraphicsItem *item, items()) {
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            removeItem(particle);
        }
    }


    // Start instructions if on level 1
    if(level == 1) {
        instructionTimer->setInterval(2000);
    }

    // set number of charges to use
    if(level == 1) {
        remainingPositiveCharges = 20;
        remainingNegativeCharges = 20;

        // reset time
        setLevelTime(50);
    } else {
        // reset time
        setLevelTime(baseTime + (int)(timeIncrement * level));
        remainingPositiveCharges = baseChargeNum + (int)(incrementChargeNum * level);
        remainingNegativeCharges = baseChargeNum + (int)(incrementChargeNum * level);
    }

    // set text of remaining charges
    updateRemainingChargeText();

    // add player
    Particle *player = new Particle();
    addItem(player);
    player->setCharge(playerCharge * (1 + levelChargeFactor * pow((double)level,2)));
    player->setParticleType(Particle::ParticlePlayer);
    player->setPosition(QVector2D(gameRectF().width() / 2,gameRectF().height() / 2));
    player->setScale(1.3 * globalScale);

    // add enemies
    /*
     Available regions. The player starts in the center, and all the "touching" regions are not allowed for enemies. Only the edges are available to the enemies.
     _____________
     |1 |11|7 |3 |
     |5_|__|__|9_|
     |10|__|__|2_|
     |4_|8_|12|6_|

     */
    int areaNumber = 1;
    for(int i=0; i<level; i++) { // 1 new enemy per level
        Particle *enemy = new Particle();
        addItem(enemy);
        // should the particle spawn at the topleft, topright, bottomleft or bottomright?
        int left = 0;
        int top = 0;
        switch(areaNumber) {
        case 1:
            left = 0;
            top = 0;
            break;
        case 2:
            left = 3;
            top = 2;
            break;
        case 3:
            left = 3;
            top = 0;
            break;
        case 4:
            left = 0;
            top = 3;
            break;
        case 5:
            left = 0;
            top = 1;
            break;
        case 6:
            left = 3;
            top = 3;
            break;
        case 7:
            left = 2;
            top = 0;
            break;
        case 8:
            left = 1;
            top = 3;
            break;
        case 9:
            left = 3;
            top = 1;
            break;
        case 10:
            left = 0;
            top = 2;
            break;
        case 11:
            left = 1;
            top = 0;
            break;
        case 12:
            left = 2;
            top = 3;
            break;
        }

        // make sure that no enemies spawn in the middle region (close to the player)
        QRectF spawnRect(gameRectF().left() + gameRectF().width()/4.0 * left, gameRectF().top() + gameRectF().height()/4.0 * top, gameRectF().width()/4.0, gameRectF().height()/4.0);
        // now, let's find a random position within this region
        qreal xrand = (qreal)qrand()/(qreal)RAND_MAX;
        qreal yrand = (qreal)qrand()/(qreal)RAND_MAX;
        enemy->setPosition(QVector2D(spawnRect.left() + xrand*spawnRect.width(),spawnRect.top() + yrand*spawnRect.height()));
        enemy->setParticleType(Particle::ParticleEnemy);
        if(level < 7) {
            enemy->setSticky(true);
        } else if(level < 15) {
            enemy->setElectroSticky(true);
        }
        enemy->setMass(26.0);
        enemy->setCharge(enemyChargeLevel);
        enemy->setScale(1.35 * globalScale);
        areaNumber++;
        if(areaNumber > 12) {
            areaNumber = 1;
        }
    }
}

void GameScene::updateRemainingChargeText() {

}

void GameScene::advance() {
    if(gameState() == GameRunning) {
        currentTime = time.elapsed();
        if(firstStep) {
            _dt = 0;
            firstStep = false;
        } else {
            _dt = (currentTime - lastFrameTime) / 1000.0;
        }
        QGraphicsScene::advance();
        lastFrameTime = currentTime;
    }
}

void GameScene::removeNegativeCharge() {
    remainingNegativeCharges--;
    if(remainingNegativeCharges < 1) {

    }
}

void GameScene::removePositiveCharge() {
    remainingPositiveCharges--;
    if(remainingPositiveCharges < 1) {

    }
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    //Create particles
    if(gameState() == GameInstructionPause) {
        if(instructionTime.elapsed() > 500) {
            toggleInstructionText();
        }
    } else if(gameState() == GameRunning){
        if(fromFp(event->scenePos().x()) < gameRectF().right()) {
            int fortegn = -1;
            if(event->button() == Qt::LeftButton) {
                if(event->modifiers() & Qt::ShiftModifier || selectedParticleType == ParticleNegative) {
                    fortegn = -1;
                } else {
                    fortegn = 1;
                }
            } else if(event->button() == Qt::RightButton) {
                fortegn = -1;
            }
            bool okayInsert = false;
            if(fortegn == -1 && remainingNegativeCharges > 0) {
                okayInsert = true;
            } else if(fortegn == 1 && remainingPositiveCharges > 0){
                okayInsert = true;
            }
            if(okayInsert) {
                if(fortegn == -1) {
                    removeNegativeCharge();
                } else {
                    removePositiveCharge();
                }
                Particle *particle = new Particle();
                addItem(particle);
                QVector2D position = QVector2D(fromFp(event->scenePos().x()),fromFp(event->scenePos().y()));
                particle->setPosition(position);
                particle->setCharge((fortegn * simpleCharge * (1 + levelChargeFactor * pow((double)m_level,2))));
                particle->setScale(1.2 * globalScale);
                // update the UI text showing number of remaining charges
                updateRemainingChargeText();
            }
        } else {
            QGraphicsScene::mousePressEvent(event);
        }
    } else {
        if(menuTime.elapsed() > 500) { // make sure we don't let the user hit any buttons before the timer has run out
            QGraphicsScene::mousePressEvent(event);
        }
    }
}

float GameScene::dt() {
    if(_dt < 0.05)
        return _dt;
    else
        return 0.05;
}

QRectF GameScene::gameRectF() {
    QRectF gameRect;
    gameRect.setLeft(0);
    gameRect.setTop(0);
    double right = gameWidth;
    double bottom = height() / width() * 100;
    gameRect.setRight(right);
    gameRect.setBottom(bottom);
    return gameRect;
}

// convert from 0-100 scale to size of screen
double GameScene::toFp(double number, bool useSmallest) const {
    if(!useSmallest || width() < height()) {
        return number / 100 * width();
    } else {
        return number / 100 * height();
    }
}

double GameScene::fromFp(double number, bool useSmallest) const {
    if(!useSmallest || width() < height()) {
        return number * 100 / width();
    } else {
        return number * 100 / height();
    }
}

void GameScene::toggleInstructionText() {
    qDebug() << "Showing instructions";
    if(gameState() == GameInstructionPause) {
        qDebug() << "Showing pause";
        setGameState(GameRunning);
        instructionText->hide();
        levelTimer->start();
        instructionTimer->setInterval(8000);
    } else  {
        qDebug() << "Showing running" << m_level << instructionNumber;
        instructionTime.restart();
        levelTimer->stop();
        setGameState(GameInstructionPause);
        instructionText->show();
        instructionTimer->setInterval(10000);
        if(m_level == 1) {
            switch(instructionNumber) {
            case 1:
                instructionText->setHtml(tr("<center><p>Welcome!</p><p>You are the green charge.<br>Avoid hitting the purple charges,<br>they are deadly to the green one.<br>Try to move the green charge by placing<br>other charges anywhere on the map.</p></center>"));
                break;
            case 2:
                instructionText->setHtml(tr("<center>You can also select blue charges<br>by clicking on the blue button to the right.<br>The red charges push away the green one,<br>while the blue charges attract it.</center>"));
                break;
            case 3:
                instructionText->setHtml(tr("<center>The time is shown in the lower right corner.<br>When the time runs out,<br/>you will move on to the next quantum state!<br/>You might call them levels if you like.</center>"));
                setLevelTime(5);
                break;
            default:
                instructionText->setHtml("");
                instructionTimer->stop();
                toggleInstructionText();
                break;
            }
        }
        instructionNumber++;
    }
}
