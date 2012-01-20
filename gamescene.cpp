#include "gamescene.h"
#include "particle.h"
#include "gameview.h"

#include <QDebug>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative>

#ifdef Q_WS_MAEMO_5
#include <QtDBus/QtDBus>
#endif

/* TODO.
    // Enhanced version (2.0)
    - Add copyright/version window.
    - New enemy particles (megacharges)
    - Make it possible to place particles that are sticky after level 15 (or something)

    // Demo version (2.1)
    - Add link to Ovi Store when demo is over.

    // To be tested (2.5)
    - Draw using FullViewPortUpdate - does it give any performance improvements?
    - Use health bar instead immediate death.
*/

// charges
const qreal enemyCharge = -7.8;
const qreal playerCharge = 7.5;
const qreal simpleCharge = 5.0;
const qreal levelChargeFactor = 0.00036;

// game area
const qreal gameWidth = 84;

const qreal incrementChargeNum = 1.1; // will be converted to int after multiplication with level number
const qreal incrementChargeNumParty = 0.5; // will be converted to int after multiplication with level number
const int baseChargeNum = 4;
const int baseChargeNumPositiveParty = 20;
const int baseChargeNumNegativeParty = 1;
// time
const int baseTime = 10;
const qreal timeIncrement = 0.9;
const int baseTimeParty = 30;
const qreal timeIncrementParty = 2;
const qreal normalTimeFactor = 1.0;
const qreal slowMotionTimeFactor = 0.2;

GameScene::GameScene(GameView *parent) :
    QGraphicsScene(parent),
    m_isSlowMotionEnabled(false)
{
    qRegisterMetaType<GameScene::GameMode>("GameMode");
    qRegisterMetaType<GameScene::GameState>("GameState");
#ifdef OS_IS_ANDROID
    qDebug() << "Force syncing settings";
    settings.sync();
#endif
    if(isDemo()) {
        qDebug() << "This is the demo version";
    } else {
        qDebug() << "This is the full version";
    }
    m_gameState = GameRunning;
    m_dt = 0;
    firstStep = true;

    setSelectedType(ParticlePositive);

    // load images
    selectionImage = QImage(":/images/selection-overlay.png");
    positiveImage = QImage(":/images/particle-positive.png");
    negativeImage = QImage(":/images/particle-negative.png");
    neutralImage = QImage(":/images/particle-neutral.png");
    playerImage = QImage(":/images/particle-player.png");
    playerOverchargedImage = QImage(":/images/particle-player-overcharged.png");
    glowingImage = QImage(":/images/particle-glowing.png");
    enemyImage = QImage(":/images/particle-enemy.png");
    slowMotionImage = QImage(":/images/particle-slow-motion.png");
    repellentImage = QImage(":/images/particle-repellent.png");
    transferImage = QImage(":/images/particle-transfer.png");

    setSceneRect(0, 0, 854, 480); // just for init, should be chosen by the platform
    setItemIndexMethod(QGraphicsScene::NoIndex);

    // Background image
    QPixmap backgroundPixmap(":/images/background.png");
    backgroundImage = this->addPixmap(backgroundPixmap);

    // Main menu
    QDeclarativeEngine *engine = new QDeclarativeEngine;
#ifdef OS_IS_ANDROID
    qDebug() << "Setting base URL for Android to /";
    engine->setBaseUrl(QUrl::fromLocalFile("/"));
#endif
    QDeclarativeComponent mainMenuComponent(engine, QUrl::fromLocalFile(adjustPath("qml/MainMenu.qml")));

    engine->rootContext()->setContextProperty("contextGameScene", this);
    mainMenu = qobject_cast<QGraphicsObject *>(mainMenuComponent.create());
    qDebug() << "Component errors:\n" << mainMenuComponent.errors();
    qDebug() << "End component errors";
    // Connect the QML back to this class
    addItem(mainMenu);
    mainMenu->setZValue(1000);
    m_specialParticles = new QList<int>;

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
    connect(levelTimer, SIGNAL(timeout()), SLOT(updateLevelTime()));

    // Set up animations
    timeFactorAnimation = new QPropertyAnimation(this, "timeFactor");
    qDebug() << "Highest level is" << m_level;

    setGameState(GameStarted);

    // just init all in the resize() function
    resized();

    // Start level and start timers
    //    startLevel(level());

    connect(&advanceTimer, SIGNAL(timeout()), SLOT(advance()));
    advanceTimer.start(10);
    time.start();
    qDebug() << "Timers started!";

    // dashboard button
#if (defined(OS_IS_DESKTOP_LINUX) || defined(Q_OS_WIN32) || defined(Q_OS_MAC) || defined(Q_WS_MAEMO_5))
    qDebug() << "Show dashboard button";
    mainMenu->setProperty("dashboardButtonVisible", true);
#else
    qDebug() << "Don't show dashboard button";
    mainMenu->setProperty("dashboardButtonVisible", false);
#endif

    // load settings
    setGameMode((GameMode)settings.value("gameMode", ModeClassic).toInt());
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
    qDebug() << "GameScene::resized(): called";
    //menuBackgroundRect->setRect(toFp(0), toFp(0), toFp(100), toFp(100));
    mainMenu->setProperty("width", width());
    mainMenu->setProperty("height", height());

    QPixmap backgroundPixmap(":/images/background.png");
    QPixmap scaledBackgroundPixmap;
    // check the ratio of the pixmap against the ratio of our scene
    if((qreal)backgroundPixmap.width() / (qreal)backgroundPixmap.height() > (qreal)width() / (qreal)height()) {
        scaledBackgroundPixmap = backgroundPixmap.scaledToHeight(height());
    } else {
        scaledBackgroundPixmap = backgroundPixmap.scaledToWidth(width());
    }
    this->removeItem(backgroundImage);
    delete backgroundImage;
    backgroundImage = addPixmap(scaledBackgroundPixmap);
    backgroundImage->setZValue(-1000);
}

void GameScene::updateLevelTime() {
    setLevelTime(levelTime() - 1);
    if(levelTime() < 1) {
        setLevelUpgrade(true);
        setGameState(GamePaused);
        if(isDemo() && m_level >= 8) {

        } else {
            setLevel(m_level + 1);
        }
        if(highestLevel() < level()) {
            setHighestLevel(level());
        }
        if(gameMode() == GameScene::ModeClassic) {
            if(settings.value("highestLevel", 1).toInt() < level()) {
                settings.setValue("highestLevel", level());
            }
        } else {
            if(settings.value("highestLevelParty", 1).toInt() < level()) {
                settings.setValue("highestLevelParty", level());
            }
        }
#ifdef OS_IS_ANDROID
        qDebug() << "Force syncing settings";
        settings.sync();
#endif
    }
}

void GameScene::continueGame() {
    if(gameState() == GameOver) {
        setLevel(m_level);
    }


    // start timer
    levelTimer->start();
    // start the main timer as well (start using CPU again)
    advanceTimer.start();
    qDebug() << "Game continued";
}

void GameScene::pauseGame() {
    qDebug() << "Game paused";
    bool wasGameRunning = false;
    if(gameState() == GameRunning) {
        menuTime.restart(); // start the timer that will avoid clicking on the quit button after level up or failed
        wasGameRunning = true;
    }
    // pause timer
    levelTimer->stop();
    // Stop the timer to save CPU power (hopefully this won't lock up anything else)
    advanceTimer.stop();
    // end animation
}

void GameScene::gameOver() {
    setGameState(GameOver);
}

void GameScene::retryGame() {
    setLevel(m_level);
    setGameState(GameRunning);
}

void GameScene::exitGame() {
    QApplication::quit();
}

void GameScene::minimizeToDashboard() {

}

void GameScene::levelUp()
{
    if(level() < highestLevel()) {
        setLevel(level() + 1);
    }
}

void GameScene::levelDown()
{
    if(level() > 1) {
        setLevel(level() - 1);
    }
}

void GameScene::clickedDashboardButton()
{
#if defined Q_WS_MAEMO_5
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusMessage message = QDBusMessage::createSignal("/","com.nokia.hildon_desktop","exit_app_view");
    connection.send(message);
#elif (defined Q_OS_LINUX || defined Q_OS_MAC || defined Q_OS_WIN32) && !defined OS_IS_HARMATTAN && !defined OS_IS_ANDROID
    if(viewMode() == ViewNormal) {
        setViewMode(ViewFullScreen);
    } else {
        setViewMode(ViewNormal);
    }
#else
    qDebug() << "Dashboard button should not have been shown...";
#endif
}

void GameScene::setGameState(GameState gameState) {
    qDebug() << "Game state set " << gameState;
    // If the gameState was game over, restart the level
    if(m_gameState == GameOver && gameState == GameRunning) {
        setLevel(level());
    }
    this->m_gameState = gameState;
    if(gameState == GameStarted) {
        pauseGame();
    }
    if(gameState == GamePaused) {
        pauseGame();
    }
    if(gameState == GameOver) {
        pauseGame();
    }
    if(gameState == GameRunning) {
        setLevelUpgrade(false);
        continueGame();
    }
    if(gameState == GameInstructionPause) {
        pauseGame();
    }
    emit gameStateChanged(gameState);
}

void GameScene::setLevel(int level) {
    m_level = level;
    qDebug() << "Starting level" << level;

    lastSpecialSpawnTime = 0;
    setTimeFactor(normalTimeFactor);

    foreach(QGraphicsItem *item, items()) {
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            removeItem(particle);
            delete particle; // Delete the particle completely. Should (hopefully) not be referenced anywhere else.
        }
    }

    if(gameMode() == ModeClassic && level == 1) {

        setRemainingPositiveCharges(20);
        setRemainingNegativeCharges(20);

        setLevelTime(27);
    } else if(gameMode() == ModeClassic) {
        setLevelTime(baseTime + (int)(timeIncrement * level));
        setRemainingPositiveCharges(baseChargeNum + (int)(incrementChargeNum * level));
        setRemainingNegativeCharges(baseChargeNum + (int)(incrementChargeNum * level));
    } else if(gameMode() == ModeParty) {
        setLevelTime(baseTimeParty + (int)(timeIncrementParty * level));
        setRemainingPositiveCharges(baseChargeNumPositiveParty + (int)(incrementChargeNumParty * level));
        setRemainingNegativeCharges(baseChargeNumNegativeParty + (int)(incrementChargeNumParty * level));
        specialParticles()->clear();
        if(level == 14) {
            specialParticles()->append(Particle::ParticleTransfer);
        }
        setRemainingSpecialCharges(specialParticles()->count());
    } else {
        qWarning() << "Some unknown mode is set!";
    }

    // set text of remaining charges
    updateRemainingChargeText();

    // add player
    player = new Particle(this);
    addItem(player);
    player->setCharge(playerCharge * (1 + levelChargeFactor * pow((double)level,2)));
    player->setParticleType(Particle::ParticlePlayer);
    if(gameMode() == ModeClassic && level == 1) {
        player->setPosition(QVector2D(gameRectF().width() * 0.9,gameRectF().height() * 0.9));
    } else {
        player->setPosition(QVector2D(gameRectF().width() / 2,gameRectF().height() / 2));
    }

    // add enemies
    addEnemies();

    // Notify everyone about the level change
    emit levelChanged(level);
}

void GameScene::enableSlowMotion(int time)
{
    qDebug() << "Slow motion enabled";
    setSlowMotionEnabled(true);
    QTimer::singleShot(time, this, SLOT(disableSlowMotion()));
    timeFactorAnimation->setDuration(1000);
    timeFactorAnimation->setStartValue(normalTimeFactor);
    timeFactorAnimation->setEndValue(slowMotionTimeFactor);
    timeFactorAnimation->start();
}

void GameScene::disableSlowMotion()
{
    qDebug() << "Slow motion disabled";
    setSlowMotionEnabled(false);
    timeFactorAnimation->setDuration(1000);
    timeFactorAnimation->setStartValue(slowMotionTimeFactor);
    timeFactorAnimation->setEndValue(normalTimeFactor);
    timeFactorAnimation->start();
}

void GameScene::addEnemies() {
    /*
     Available regions. The player starts in the center, and all the "touching" regions are not allowed for enemies. Only the edges are available to the enemies.
     _____________
     |1 |11|7 |3 |
     |5_|__|__|9_|
     |10|__|__|2_|
     |4_|8_|12|6_|

     */
    int areaNumber = 1;
    int numberOfEnemies = 1;
    qreal enemyChargeLevel = enemyCharge * (1 + levelChargeFactor * pow((double)level(),2));
    if(gameMode() == ModeClassic) {
        numberOfEnemies = level();
    } else if(gameMode() == ModeParty) {
        numberOfEnemies = 3 + level();
    }
    for(int i=0; i<numberOfEnemies; i++) { // 1 new enemy per level
        Particle *enemy = new Particle(this);
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
        QRectF spawnRect(gameRectF().left() + gameRectF().width()/4.0 * left,
                         gameRectF().top() + gameRectF().height()/4.0 * top,
                         gameRectF().width()/4.0,
                         gameRectF().height()/4.0);
        // now, let's find a random position within this region
        qreal xrand = (qreal)qrand()/(qreal)RAND_MAX;
        qreal yrand = (qreal)qrand()/(qreal)RAND_MAX;
        enemy->setPosition(QVector2D(spawnRect.left() + xrand*spawnRect.width(), spawnRect.top() + yrand*spawnRect.height()));
        enemy->setParticleType(Particle::ParticleEnemy);
        if(gameMode() == ModeClassic) {
            if(level() < 7) {
                enemy->setSticky(true);
            } else if(level() < 15) {
                enemy->setElectroSticky(true);
            }
        } else {
            enemy->setSticky(false);
            enemy->setElectroSticky(false);
        }
        enemy->setMass(26.0);
        enemy->setCharge(enemyChargeLevel);
        areaNumber++;
        if(areaNumber > 12) {
            areaNumber = 1;
        }
    }
}

void GameScene::addTransferParticle() {
    specialParticles()->append(Particle::ParticleTransfer);
    setRemainingSpecialCharges(specialParticles()->count());
}

void GameScene::updateRemainingChargeText() {

}

void GameScene::advance() {
    if(gameState() == GameRunning) {
        currentTime = time.elapsed();
        if(firstStep) {
            m_dt = 0;
            firstStep = false;
        } else {
            m_dt = timeFactor() * (currentTime - lastFrameTime) / 1000.0;
        }

        if(gameMode() == ModeParty) {
            checkAddSpecialParticle();
        }
        QGraphicsScene::advance();
        lastFrameTime = currentTime;
    }
}

void GameScene::checkAddSpecialParticle() {
    foreach(QGraphicsItem *item, items()) {
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            // Remove overdue charges
            if(particle->dueTime() < 0 &&
                    (particle->particleType() == Particle::ParticleSimple ||
                     particle->particleType() == Particle::ParticleSlowMotion ||
                     particle->particleType() == Particle::ParticleRepellent ||
                     particle->particleType() == Particle::ParticleTransfer ||
                     particle->particleType() == Particle::ParticleGlowing)
                    ) {
                if(particle->particleType() == Particle::ParticleSimple) {
                    if(particle->charge() > 0) {
                        setRemainingPositiveCharges(remainingPositiveCharges()+1);
                    } else {
                        setRemainingNegativeCharges(remainingNegativeCharges()+1);
                    }
                }
                removeItem(particle);
                delete particle;
            }
        }
    }

    if(currentTime - lastSpecialSpawnTime > 5000) {
        qDebug() << "Spawning special!";
        Particle *particle = new Particle(this);
        addItem(particle);
        int maxParticleType;
        if(level() < 7) {
            maxParticleType = 1;
        } else if (level() < 14) {
            maxParticleType = 2;
        } else {
            maxParticleType = 3;
        }

        int particleType = (int) ( maxParticleType * (qreal)qrand()/(qreal)RAND_MAX);
        qDebug() << "Spawning type " << particleType;
        switch(particleType) {
        case 0:
            particle->setParticleType(Particle::ParticleRepellent);
            particle->setSticky(true);
            break;
        case 1:
            particle->setParticleType(Particle::ParticleSlowMotion);
            break;
        case 2:
            particle->setParticleType(Particle::ParticleTransfer);
            particle->setSticky(true);
            break;
        }
        particle->setCharge(0);
        QVector2D position = player->position();
        while((position - player->position()).lengthSquared() < player->size().width() * 3) {
            qreal xrand = (qreal)qrand()/(qreal)RAND_MAX;
            qreal yrand = (qreal)qrand()/(qreal)RAND_MAX;
            position = QVector2D(xrand * gameRectF().width(),yrand * gameRectF().height());
        }
        particle->setPosition(position);
        lastSpecialSpawnTime = currentTime;
    }
}

float GameScene::dt() {
    if(m_dt < 0.05)
        return m_dt;
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

void GameScene::setGameMode(GameScene::GameMode gameMode)
{
    qDebug() << "Game mode changed to " << gameMode;
    m_gameMode = gameMode;
    settings.setValue("gameMode", gameMode);
    if(gameMode == GameScene::ModeClassic) {
        setLevel(settings.value("highestLevel", 1).toInt());
    } else {
        setLevel(settings.value("highestLevelParty", 1).toInt());
    }
    setHighestLevel(level());
    emit gameModeChanged(gameMode);
}

void GameScene::setViewMode(ViewMode arg)
{
    settings.setValue("viewMode", arg);
    if (m_viewMode != arg) {
        if(parent() != 0) { // Make sure we know about our GameView parent
            if(arg == ViewFullScreen ) {
                ((GameView*)parent())->showFullScreen();
            } else {
                ((GameView*)parent())->showNormal();
            }
        }
        m_viewMode = arg;
        emit viewModeChanged(arg);
    }
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    //Create particles
    if(gameState() == GameRunning){
        if(fromFp(event->scenePos().x()) < gameRectF().right()) {
            QVector2D clickPosition = QVector2D(fromFp(event->scenePos().x()),fromFp(event->scenePos().y()));
            if(selectedType() == ParticlePositive || selectedType() == ParticleNegative) {
                int fortegn = -1;
                if(event->button() == Qt::LeftButton) {
                    if(event->modifiers() & Qt::ShiftModifier || selectedType() == ParticleNegative) {
                        fortegn = -1;
                    } else {
                        fortegn = 1;
                    }
                } else if(event->button() == Qt::RightButton) {
                    fortegn = -1;
                }
                bool okayInsert = false;
                if(fortegn == -1 && remainingNegativeCharges() > 0) {
                    okayInsert = true;
                } else if(fortegn == 1 && remainingPositiveCharges() > 0){
                    okayInsert = true;
                }
                if(okayInsert) {
                    if(fortegn == -1) {
                        setRemainingNegativeCharges(remainingNegativeCharges() - 1);
                    } else {
                        setRemainingPositiveCharges(remainingPositiveCharges() - 1);
                    }
                    Particle *particle = new Particle(this);
                    addItem(particle);
                    particle->setParticleType(Particle::ParticleSimple);
                    particle->setPosition(clickPosition);
                    particle->setCharge((fortegn * simpleCharge * (1 + levelChargeFactor * pow((double)m_level,2))));
                    // update the UI text showing number of remaining charges
                    updateRemainingChargeText();
                }
            } else if (selectedType() == ParticleSpecial) {
                qDebug() << specialParticles()->count();
                if(!specialParticles()->isEmpty()) {
                    Particle::ParticleType particleType = (Particle::ParticleType)specialParticles()->first();
                    specialParticles()->removeFirst();
                    if(particleType == Particle::ParticleTransfer) {
                        player->setPosition(clickPosition);
                        player->setVelocity(QVector2D());
                    }
                    m_remainingSpecialCharges = specialParticles()->count();
                    emit remainingSpecialChargesChanged(m_remainingSpecialCharges);
                }
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
