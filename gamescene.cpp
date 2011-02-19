#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>

/* TODO
    - Button indicator (what have I selected?).
    - Save highest achieved level, and make this the highest possible level to select with the buttons.
    - Always start at the highest level.
    - Create a countdown timer before the game starts - this gives the player a chance to make a strategy.
    - Fix font on Symbian.
    - Start a new project from this to find the error that causes rectangles and text to be drawn infrequently.
    - Add menutimer (avoid quick clicks)

    // Finishing
    - Add copyright/version window.
    - Publish to Ovi Store

    // To be tested (1.5)
    - Test with Meego SDK
    - Test with Android NDK (what about OpenGL? - how is the performance without it?)
    - Draw using FullViewPortUpdate - does it give any performance improvements?

    // Enhanced version (2.0)
    - New particles to play with
    - New enemy particles (megacharges)
    - Dummy particles (just in the way and sticky, but not dangerous)
    - Make it possible to place particles that are sticky after level 15 (or something)
*/
// scaling
const qreal globalScale = 2.5;
// charges
const qreal enemyCharge = -5.8;
const qreal playerCharge = 5.5;
const qreal simpleCharge = 3.25;
const qreal levelChargeFactor = 0.0015;

// game area
const qreal gameWidth = 84;

const qreal incrementChargeNum = 1.25; // will be converted to int after multiplication with level number
const int baseChargeNum = 4;
// time
const int baseTime = 10;
const qreal timeIncrement = 1.8;

// z values
const int zInGameMenu = 91;
const int zInGameMenuNumbers = 92;
const int zInGameBackground = 90;
const int zMainMenu = 101;
const int zMainMenuBackground = 100;

// in game gui
const qreal positiveButtonY = 35;
const qreal negativeButtonY = 55;
const qreal timerTextFontSize = 8;
const qreal instructionTextFontSize = 5;
const qreal chargesLeftFontSize = 6;
const qreal timerTextY = 75;

GameScene::GameScene(QObject *parent) :
        QGraphicsScene(parent)
{
    level = 1;
    _gameState = GameRunning;
    _dt = 0;
    firstStep = true;

    selectedParticleType = ParticlePositive;

    // load images
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
    positiveButton->setPosition(QVector2D(92,positiveButtonY));
    positiveButton->setScale(16);
    positiveButton->setButtonType(Button::ButtonPositive);
    positiveButton->setZValue(zInGameMenu);

    negativeButton = new Button();
    addItem(negativeButton);
    negativeButton->setScale(16);
    negativeButton->setPosition(QVector2D(92,negativeButtonY));
    negativeButton->setButtonType(Button::ButtonNegative);
    negativeButton->setZValue(zInGameMenu);

    pauseGameButton = new Button();
    prepareButton(pauseGameButton);
    pauseGameButton->setScale(15);
    pauseGameButton->setPosition(QVector2D(92,10));
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
    menuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    menuBackgroundRect->show();
    menuBackgroundRect->setOpacity(0.7);
    menuBackgroundRect->setZValue(zMainMenuBackground);
//    menuBackgroundBlur.setBlurRadius(toFp(2));
//    menuBackgroundRect->setGraphicsEffect(&menuBackgroundBlur);

    continueButton = new Button();
    prepareButton(continueButton);
    continueButton->setScale(30);
    continueButton->setPosition(QVector2D(50,50));
    continueButton->setImage(":/images/button-continue.png");

    retryButton = new Button();
    prepareButton(retryButton);
    retryButton->setPosition(QVector2D(70,50));
    retryButton->setScale(14);
    retryButton->hide();
    retryButton->setImage(":/images/button-retry.png");

    exitButton = new Button();
    prepareButton(exitButton);
    exitButton->setPosition(QVector2D(92,10));
    exitButton->setImage(":/images/button-exit.png");
    // next/prev level
    prevLevelButton = new Button();
    prepareButton(prevLevelButton);
    prevLevelButton->setPosition(QVector2D(60,75));
    prevLevelButton->setImage(":/images/button-leveldown.png");

    nextLevelButton = new Button();
    prepareButton(nextLevelButton);
    nextLevelButton->setPosition(QVector2D(40,75));
    nextLevelButton->setImage(":/images/button-levelup.png");

    // menu text
    QFont menuFont;
    QColor menuFontColor(250,250,250,245);
    menuFont.setFamily("NovaSquare");
    // menu title text
    menuTitleText = addText("Reaktor", menuFont);
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
    connect(exitButton, SIGNAL(clicked()), SLOT(exitGame()));
    // next/prev level
    connect(nextLevelButton, SIGNAL(clicked()), SLOT(clickedNextLevelButton()));
    connect(prevLevelButton, SIGNAL(clicked()), SLOT(clickedPrevLevelButton()));

    // just init all in the resize() function
    resized();

    // Start level and start timers
    startLevel(1);

    QObject::connect(&timer, SIGNAL(timeout()), SLOT(advance()));
    timer.start(10);
    time.start();
    qDebug() << "Timers started!";
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
    menuBackgroundRect->setRect(toFp(0), toFp(0), toFp(100), toFp(100));
    gameMenuBackgroundRect->setRect(toFp(gameWidth), toFp(0), toFp(100 - gameWidth), toFp(100));
    timerText->setPos(toFp(gameWidth,false),toFp(timerTextY,true));
    timerText->setTextWidth(toFp(100 - gameWidth));
    instructionText->setPos(toFp(5),toFp(5));
    instructionText->setTextWidth(toFp(gameWidth));
    remainingPositiveChargesText->setPos(toFp(gameWidth,false),toFp(positiveButtonY - chargesLeftFontSize * 0.8,true));
    remainingPositiveChargesText->setTextWidth(toFp(100 - gameWidth));
    remainingNegativeChargesText->setPos(toFp(gameWidth,false),toFp(negativeButtonY - chargesLeftFontSize * 0.8,true));
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
    qDebug() << levelTime;
    if(levelTime < 1) {
        pauseGame();
        menuTitleText->setHtml("<center>Level up!</center>");
        menuTitleText->show();
        startLevel(level + 1);
    }
}

void GameScene::continueGame() {
    timerText->setHtml("<center>" + QString::number(levelTime) + "</center>");
    if(gameState() == GameOver) {
        startLevel(level);
    }
    if(level == 1 || level == 2) {
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
    if(gameState() == GameRunning) {
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
    nextLevelButton->show();
    prevLevelButton->show();
    exitButton->show();
    // pause timer
    levelTimer->stop();
    qDebug() << "Game paused";
}

void GameScene::gameOver() {
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

void GameScene::clickedNextLevelButton() {
    startLevel(level + 1);
}

void GameScene::clickedPrevLevelButton() {
    startLevel(level - 1);
}

void GameScene::clickedNegativeButton() {
    selectedParticleType = ParticleNegative;
}

void GameScene::clickedPositiveButton() {
    selectedParticleType = ParticlePositive;
}

void GameScene::setGameState(int gameState) {
    this->_gameState = gameState;
    if(gameState == GameStarted) {
        pauseGame();
    }
}

void GameScene::startLevel(int level) {
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

    // Start instructions if on level 1 or 2
    if(level == 1 || level == 2) {
        instructionTimer->setInterval(2000);
    }

    // set number of charges to use
    if(level == 1 || level == 2) {
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
     |10|__|__|6_|
     |4_|8_|12|2_|

     */
    int areaNumber = 1;
    for(int i=0; i<level-1; i++) { // no enemies in level 1
        Particle *enemy = new Particle();
        addItem(enemy);
        // should the particle spawn at the topleft, topright, bottomleft or bottomright?
        int left;
        int top;
        switch(areaNumber) {
        case 1:
            left = 0;
            top = 0;
            break;
        case 2:
            left = 3;
            top = 3;
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
            top = 2;
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
        qDebug() << gameRectF() << gameRectF().left() + gameRectF().width()/4.0 * left << gameRectF().width()/4.0 << left;
        QRectF spawnRect(gameRectF().left() + gameRectF().width()/4.0 * left, gameRectF().top() + gameRectF().height()/4.0 * top, gameRectF().width()/4.0, gameRectF().height()/4.0);
        qDebug() << "spawnRect" << spawnRect;
        // now, let's find a random position within this region
        qreal xrand = (qreal)qrand()/(qreal)RAND_MAX;
        qreal yrand = (qreal)qrand()/(qreal)RAND_MAX;
        enemy->setPosition(QVector2D(spawnRect.left() + xrand*spawnRect.width(),spawnRect.top() + yrand*spawnRect.height()));
        qDebug() << enemy->position();
        enemy->setParticleType(Particle::ParticleEnemy);
        enemy->setSticky(true);
        enemy->setCharge(enemyCharge * (1 + levelChargeFactor * pow((double)level,2)));
        enemy->setScale(1.35 * globalScale);
        areaNumber++;
        if(areaNumber > 12) {
            areaNumber = 1;
        }
    }
    qDebug() << "level started";
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
    qDebug() << "MousePress at" << fromFp(event->scenePos().x()) << gameRectF().right();
    if(gameState() == GameInstructionPause) {
        if(instructionTime.elapsed() > 750) {
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
        QGraphicsScene::mousePressEvent(event);
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
                instructionText->setHtml(tr("<center><p>Welcome!</p><p>You are the green charge.<br>Try to move the green charge by placing<br>other charges anywhere on the map.</p></center>"));
                break;
            case 2:
                instructionText->setHtml(tr("<center>You can also select blue charges by clicking on the blue button to the right.</center>"));
                break;
            case 3:
                instructionText->setHtml(tr("<center>The red charges push away the green one, while the blue charges attract it.</center>"));
                break;
            case 4:
                instructionText->setHtml(tr("<center>Just be aware.<br />The green charge gets stronger when it crashes with the others.</center>"));
                break;
            case 5:
                instructionText->setHtml(tr("<center>Alright! Let's kick things up a notch.<br/>We're moving on to the next level!</center>"));
                levelTime = 2;
                break;
            default:
                instructionText->setHtml("");
                instructionTimer->stop();
                toggleInstructionText();
                break;
            }
        } else if(level == 2) {
            switch(instructionNumber) {
            case 1:
                instructionText->setHtml(tr("<center>You should try to avoid the purple charges.<br />They are deadly to the green one.<br />Almost like antimatter.</center>"));
                break;
            case 2:
                instructionText->setHtml(tr("<center>The time is shown in the lower right corner.</center>"));
                break;
            case 3:
                instructionText->setHtml(tr("<center>When the time runs out,<br/>you will move on to the next quantum state!<br/>You might call them levels if you like.</center>"));
                break;
            case 4:
                instructionText->setHtml(tr("<center>Well, it's up to you from here.<br/>I'll leave you with the next level.</center>"));
                levelTime = 2;
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
