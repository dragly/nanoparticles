#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeComponent>

/* TODO.
    // Finishing
    - Add copyright/version window.
    - Publish to Ovi Store

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
const QVector2D positiveButtonPosition(92,35);
const QVector2D negativeButtonPosition(92,55);
const QVector2D pauseGameButtonPosition(92,10);
const QVector2D continueButtonPosition(50,50);
const QVector2D retryButtonPosition(70,50);
const QVector2D aboutDialogButtonPosition(92,35);
const QVector2D exitButtonPosition(92,10);
const QVector2D prevLevelButtonPosition(60,75);
const QVector2D nextLevelButtonPosition(40,75);
const qreal negativeButtonY = 55;
const qreal timerTextFontSize = 8;
const qreal instructionTextFontSize = 5;
const qreal chargesLeftFontSize = 6;
const qreal timerTextY = 75;


GameScene::GameScene(QObject *parent) :
    QGraphicsScene(parent)
{
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
    level = settings.value("highestLevel", 1).toInt();
    qDebug() << "Highest level is" << level;

    // load images
    selectionImage = QImage(":/images/selection-overlay.png");
    positiveImage = QImage(":/images/particle-positive.png");
    negativeImage = QImage(":/images/particle-negative.png");
    neutralImage = QImage(":/images/particle-neutral.png");
    playerImage = QImage(":/images/particle-player.png");
    playerOverchargedImage = QImage(":/images/particle-player-overcharged.png");
    enemyImage = QImage(":/images/particle-enemy.png");

    setSceneRect(0, 0, 400, 300); // just for init
    setItemIndexMethod(QGraphicsScene::NoIndex);
    // Add in-game menu
    positiveButton = new Button();
    addItem(positiveButton);
    positiveButton->setPosition(positiveButtonPosition);
    positiveButton->setScale(16);
    positiveButton->setButtonType(Button::ButtonPositive);
    positiveButton->setZValue(zInGameMenu);
    positiveButton->setSelected(true);

    negativeButton = new Button();
    addItem(negativeButton);
    negativeButton->setScale(16);
    negativeButton->setPosition(negativeButtonPosition);
    negativeButton->setButtonType(Button::ButtonNegative);
    negativeButton->setZValue(zInGameMenu);

    pauseGameButton = new Button();
    prepareButton(pauseGameButton);
    pauseGameButton->setScale(15);
    pauseGameButton->setPosition(pauseGameButtonPosition);
    pauseGameButton->setImage(":/images/button-pause.png");
    pauseGameButton->setZValue(zInGameMenu);

    // timer text (level time left)
    QFont font;
    font.setFamily("NovaSquare");
    timerText = addText("",font);
    timerText->setDefaultTextColor(QColor(250,250,250,220));
    timerText->setZValue(zInGameMenu);
    // instruction text
    instructionText = addText("",font);
    instructionText->setDefaultTextColor(QColor(250,250,250,220));
    instructionText->setZValue(zMainMenu);
    instructionText->hide();
    // instruction text
    levelText = addText("",font);
    levelText->setDefaultTextColor(QColor(250,250,250,220));
    levelText->setZValue(zMainMenu);
    levelText->hide();

    remainingPositiveChargesText = addText("",font);
    remainingPositiveChargesText->setDefaultTextColor(QColor(250,250,250,220));
    remainingPositiveChargesText->setZValue(zInGameMenu);

    remainingNegativeChargesText = addText("",font);
    remainingNegativeChargesText->setDefaultTextColor(QColor(250,250,250,220));
    remainingNegativeChargesText->setZValue(zInGameMenu);
    // end add in-game menu

    // Game menu background
    gameMenuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    gameMenuBackgroundRect->show();
    gameMenuBackgroundRect->setOpacity(0.7);
    gameMenuBackgroundRect->setZValue(zInGameBackground);
    // Main menu background
    QPixmap menuBackgroundPixmap(":/images/blackback.png");
    menuBackgroundRect = addPixmap(menuBackgroundPixmap);
    menuBackgroundRect->setScale(1000);

//    menuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    menuBackgroundRect->show();
    menuBackgroundRect->setOpacity(0.7);
    menuBackgroundRect->setZValue(zMainMenuBackground);
    //    menuBackgroundBlur.setBlurRadius(toFp(2));
    //    menuBackgroundRect->setGraphicsEffect(&menuBackgroundBlur);

    continueButton = new Button();
    prepareButton(continueButton);
    continueButton->setScale(30);
    continueButton->setPosition(continueButtonPosition);
    continueButton->setImage(":/images/button-continue.png");

    retryButton = new Button();
    prepareButton(retryButton);
    retryButton->setPosition(retryButtonPosition);
    retryButton->setScale(14);
    retryButton->hide();
    retryButton->setImage(":/images/button-retry.png");

    aboutDialogButton = new Button();
    prepareButton(aboutDialogButton);
    aboutDialogButton->setPosition(aboutDialogButtonPosition);
    aboutDialogButton->setScale(14);
    aboutDialogButton->hide();
    aboutDialogButton->setImage(":/images/button-info.png");

    exitButton = new Button();
    prepareButton(exitButton);
    exitButton->setPosition(exitButtonPosition);
    exitButton->setImage(":/images/button-exit.png");
    // next/prev level
    prevLevelButton = new Button();
    if(level <= 1) {
        prevLevelButton->hide();
    }
    prepareButton(prevLevelButton);
    prevLevelButton->setPosition(prevLevelButtonPosition);
    prevLevelButton->setImage(":/images/button-leveldown.png");

    nextLevelButton = new Button();
    nextLevelButton->hide();
    prepareButton(nextLevelButton);
    nextLevelButton->setPosition(nextLevelButtonPosition);
    nextLevelButton->setImage(":/images/button-levelup.png");

    // About dialog
    QDeclarativeEngine *engine = new QDeclarativeEngine;
    QDeclarativeComponent component(engine, QUrl::fromLocalFile("qml/AboutDialog.qml"));
    aboutDialog = qobject_cast<QGraphicsObject *>(component.create());
    addItem(aboutDialog);
    aboutDialog->hide();
    aboutDialog->setProperty("opacity", 0);
    aboutDialog->setZValue(10000);
    qreal screenWidth = QApplication::desktop()->screenGeometry().width();
    qreal screenHeight = QApplication::desktop()->screenGeometry().height();
    if(screenWidth  > screenHeight) { // Symbian hack
        aboutDialog->setProperty("width", screenWidth);
        aboutDialog->setProperty("height", screenHeight);
    } else {
        aboutDialog->setProperty("width", screenHeight);
        aboutDialog->setProperty("height", screenWidth);
    }
    // menu text
    QFont menuFont;
    QColor menuFontColor(250,250,250,245);
    menuFont.setFamily("NovaSquare");
    // menu title text
    if(isDemo()) {
        menuTitleText = addText("Reaktor demo", menuFont);
    } else {
        menuTitleText = addText("Reaktor", menuFont);
    }
    menuTitleText->setHtml("<center>Reaktor</center>");
    menuTitleText->setDefaultTextColor(menuFontColor);
    menuTitleText->setZValue(zMainMenu);
    // end add menu

    // set up timer
    levelTimer = new QTimer(this);
    levelTimer->setInterval(1000);
    connect(levelTimer, SIGNAL(timeout()), SLOT(updateTime()));
    // instruction timer
    instructionTimer = new QTimer(this);
    connect(instructionTimer, SIGNAL(timeout()), SLOT(toggleInstructionText()));
    // end set up timers

    setGameState(GameStarted);

    // add button signal connections
    connect(positiveButton, SIGNAL(clicked()), SLOT(clickedPositiveButton()));
    connect(negativeButton, SIGNAL(clicked()), SLOT(clickedNegativeButton()));
    connect(continueButton, SIGNAL(clicked()), SLOT(continueGame()));
    connect(pauseGameButton, SIGNAL(clicked()), SLOT(pauseGame()));
    connect(retryButton, SIGNAL(clicked()), SLOT(retryGame()));
    connect(aboutDialogButton, SIGNAL(clicked()), SLOT(showAboutDialog()));
    connect(exitButton, SIGNAL(clicked()), SLOT(exitGame()));
    // next/prev level
    connect(nextLevelButton, SIGNAL(clicked()), SLOT(clickedNextLevelButton()));
    connect(prevLevelButton, SIGNAL(clicked()), SLOT(clickedPrevLevelButton()));

    // just init all in the resize() function
    resized();

    // Start level and start timers
    startLevel(level);

    QObject::connect(&timer, SIGNAL(timeout()), SLOT(advance()));
    timer.start(10);
    time.start();
    qDebug() << "Timers started!";
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

void GameScene::prepareButton(Button *button) {
    addItem(button);
    button->setScale(16);
    button->setZValue(zMainMenu);
    button->setButtonType(Button::StandardButton);
}

void GameScene::resized() {
    foreach(QGraphicsItem* item, items()) {
        if(GameObject* button = (GameObject*)item) {
            if(button == negativeButton || button == positiveButton || button == pauseGameButton || button == exitButton) {
                button->setPos(toFp(button->position().x(),false),toFp(button->position().y(),true));
            } else {
                button->setPos(toFp(button->position().x()),toFp(button->position().y(),true));
            }
        }
    }
    //menuBackgroundRect->setRect(toFp(0), toFp(0), toFp(100), toFp(100));
    gameMenuBackgroundRect->setRect(toFp(gameWidth), toFp(0), toFp(100 - gameWidth), toFp(100));
    timerText->setPos(toFp(gameWidth,false),toFp(timerTextY,true));
    timerText->setTextWidth(toFp(100 - gameWidth));
    instructionText->setPos(toFp(5),toFp(5));
    instructionText->setTextWidth(toFp(gameWidth));
    remainingPositiveChargesText->setPos(toFp(gameWidth,false),toFp(positiveButtonPosition.y() - chargesLeftFontSize * 0.8,true));
    remainingPositiveChargesText->setTextWidth(toFp(100 - gameWidth));
    remainingNegativeChargesText->setPos(toFp(gameWidth,false),toFp(negativeButtonPosition.y() - chargesLeftFontSize * 0.8,true));
    remainingNegativeChargesText->setTextWidth(toFp(100 - gameWidth));
    // Reisze gui font
    QFont timerFont = menuTitleText->font();
    timerFont.setPixelSize((int)toFp(timerTextFontSize,true));
    timerText->setFont(timerFont);
    // Reisze gui font
    QFont instructionFont = instructionText->font();
    instructionFont.setPixelSize((int)toFp(instructionTextFontSize,true));
    instructionText->setFont(instructionFont);
    QFont chargesLeftFont = menuTitleText->font();
    chargesLeftFont.setPixelSize((int)toFp(chargesLeftFontSize,true));
    remainingPositiveChargesText->setFont(chargesLeftFont);
    remainingNegativeChargesText->setFont(chargesLeftFont);
    // Menu title
    menuTitleText->setPos(0,toFp(15,true));
    menuTitleText->setTextWidth(toFp(100));
    QFont menuFont = menuTitleText->font();
    menuFont.setPixelSize((int)toFp(10,true));
    menuTitleText->setFont(menuFont);
    // Level text
    levelText->setPos(toFp(0),toFp(5));
    levelText->setTextWidth(toFp(100));
    QFont levelFont = levelText->font();
    levelFont.setPixelSize((int)toFp(6,true));
    levelText->setFont(levelFont);
}

void GameScene::updateTime() {
    levelTime--;
    timerText->setHtml("<center>" + QString::number(levelTime) + "</center>");
    if(levelTime < 1) {
        pauseGame();
        if(isDemo() && level >= 8) {
            menuTitleText->setHtml("<center>End of demo.</center>");
            levelText->setHtml("<center>Buy game in <a href='http://ovi.com'>Ovi Store</a></center>");
            continueButton->hide();
            nextLevelButton->hide();
        } else {
            menuTitleText->setHtml("<center>Level up!</center>");
            startLevel(level + 1);
        }
        menuTitleText->show();
        if(settings.value("highestLevel", 1).toInt() < level) {
            settings.setValue("highestLevel", level);
        }
    }
}

void GameScene::continueGame() {
    timerText->setHtml("<center>" + QString::number(levelTime) + "</center>");
    if(gameState() == GameOver) {
        startLevel(level);
    }
    if(level == 1) {
        instructionTimer->start();
    }
    setGameState(GameRunning);
    // show pause button
    pauseGameButton->show();
    negativeButton->show();
    positiveButton->show();
    timerText->show();
    remainingPositiveChargesText->show();
    remainingNegativeChargesText->show();
    // hide main menu
    aboutDialogButton->hide();
    levelText->hide();
    menuBackgroundRect->hide();
    continueButton->hide();
    retryButton->hide();
    exitButton->hide();
    nextLevelButton->hide();
    prevLevelButton->hide();
    // hide main menu text
    menuTitleText->hide();

    // start timer
    levelTimer->start();
    qDebug() << "Game started";
}

void GameScene::pauseGame() {
    bool wasGameRunning = false;
    if(gameState() == GameRunning) {
        menuTime.restart(); // start the timer that will avoid clicking on the quit button after level up or failed
        wasGameRunning = true;
        setGameState(GamePaused);
        menuTitleText->setHtml("<center>Paused</center>");
        menuTitleText->show();
    }
    if(gameState() != GameStarted) {
        retryButton->show();
    }
    if(gameState() != GameOver) {
        continueButton->show();
    }
    instructionTimer->stop();
    menuBackgroundRect->show();
    // hide pause button
    pauseGameButton->hide();
    negativeButton->hide();
    positiveButton->hide();
    timerText->hide();
    remainingPositiveChargesText->hide();
    remainingNegativeChargesText->hide();

    // show main menu
    levelText->show();
    if(settings.value("highestLevel",1).toInt() > level) {
        nextLevelButton->show();
    } else {
        nextLevelButton->hide();
    }
    if(level > 1) {
        prevLevelButton->show();
    } else {
        prevLevelButton->hide();
    }
    aboutDialogButton->show();
    exitButton->show();
    // pause timer
    levelTimer->stop();
    qDebug() << "Game paused";
    // Animation
    // avoid animation if we are just changing levels. I.e., only animate if the game was running earlier.
    if(wasGameRunning) {
        animateMenuIn();
    }

    // end animation
}

void GameScene::showAboutDialog() {
    aboutDialog->show();
    aboutDialog->setProperty("opacity", 1);
}

void GameScene::gameOver() {
    animateMenuIn();
    setGameState(GameOver);
    menuBackgroundRect->show();
    menuTitleText->setHtml("<center>Level Failed!</center>");
    menuTitleText->show();
    pauseGame();

    qDebug() << "Game over";
}

void GameScene::retryGame() {
    startLevel(level);
    continueGame();
}

void GameScene::exitGame() {
    QApplication::quit();
}

void GameScene::animateMenuIn() {
    QList<QObject*> animatedObjectsIn;
    animatedObjectsIn.append(exitButton);
    animatedObjectsIn.append(nextLevelButton);
    animatedObjectsIn.append(prevLevelButton);
    //    animatedObjectsIn.append(menuBackgroundRect);
    animatedObjectsIn.append(menuTitleText);
    animatedObjectsIn.append(retryButton);
    animatedObjectsIn.append(continueButton);
    animatedObjectsIn.append(levelText);
    animatedObjectsIn.append(aboutDialogButton);
    QList<QObject*> animatedObjectsOut;

    foreach(QObject* animObject, animatedObjectsIn) {
        QPropertyAnimation *animation = new QPropertyAnimation(animObject, "opacity", this);
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation->setDuration(800);
        animation->setEasingCurve(QEasingCurve::InOutSine);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void GameScene::clickedNextLevelButton() {
    startLevel(level + 1);
    pauseGame();
}

void GameScene::clickedPrevLevelButton() {
    startLevel(level - 1);
    pauseGame();
}

void GameScene::clickedNegativeButton() {
    selectedParticleType = ParticleNegative;
    qDebug() << "Clicked negative";
    positiveButton->setSelected(false);
    negativeButton->setSelected(true);
}

void GameScene::clickedPositiveButton() {
    qDebug() << "Clicked positive";
    selectedParticleType = ParticlePositive;
    positiveButton->setSelected(true);
    negativeButton->setSelected(false);
}

void GameScene::setGameState(int gameState) {
    this->_gameState = gameState;
    if(gameState == GameStarted) {
        pauseGame();
    }
}

void GameScene::startLevel(int level) {
    qDebug() << "Starting level" << level;

    qreal enemyChargeLevel = enemyCharge * (1 + levelChargeFactor * pow((double)level,2));

    instructionNumber = 1; // if there are instructions, start with the first one

    this->level = level;

    foreach(QGraphicsItem *item, items()) {
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            removeItem(particle);
        }
    }

    // reset buttons
    positiveButton->setEnabled(true);
    negativeButton->setEnabled(true);

    // set level text
    levelText->setHtml("<center>Level " + QString::number(level) + "</center>");

    // Start instructions if on level 1
    if(level == 1) {
        instructionTimer->setInterval(2000);
    }

    // set number of charges to use
    if(level == 1) {
        remainingPositiveCharges = 20;
        remainingNegativeCharges = 20;

        // reset time
        levelTime = 50;
    } else {
        // reset time
        levelTime = baseTime + (int)(timeIncrement * level);
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
    remainingNegativeChargesText->setHtml("<center>" + QString::number(remainingNegativeCharges) + "</center>");
    remainingPositiveChargesText->setHtml("<center>" + QString::number(remainingPositiveCharges) + "</center>");
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
        negativeButton->update();
    }
}

void GameScene::removePositiveCharge() {
    remainingPositiveCharges--;
    if(remainingPositiveCharges < 1) {
        positiveButton->update();
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
                particle->setCharge((fortegn * simpleCharge * (1 + levelChargeFactor * pow((double)level,2))));
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
        menuBackgroundRect->hide();
        instructionTimer->setInterval(8000);
    } else  {
        qDebug() << "Showing running" << level << instructionNumber;
        instructionTime.restart();
        levelTimer->stop();
        setGameState(GameInstructionPause);
        instructionText->show();
        menuBackgroundRect->show();
        instructionTimer->setInterval(10000);
        if(level == 1) {
            switch(instructionNumber) {
            case 1:
                instructionText->setHtml(tr("<center><p>Welcome!</p><p>You are the green charge.<br>Avoid hitting the purple charges,<br>they are deadly to the green one.<br>Try to move the green charge by placing<br>other charges anywhere on the map.</p></center>"));
                break;
            case 2:
                instructionText->setHtml(tr("<center>You can also select blue charges<br>by clicking on the blue button to the right.<br>The red charges push away the green one,<br>while the blue charges attract it.</center>"));
                break;
            case 3:
                instructionText->setHtml(tr("<center>The time is shown in the lower right corner.<br>When the time runs out,<br/>you will move on to the next quantum state!<br/>You might call them levels if you like.</center>"));
                levelTime = 5;
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
