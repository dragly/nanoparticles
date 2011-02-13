#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>

/* TODO
    - Lower the decharge rate.
    - Draw using FullViewPortUpdate to avoid movement calculations on Symbian (slow)
    - Create a system for levels.
    - Create level menu (and figure out how to format this).
    - Save highest achieved level, and make this the highest possible level to select in the level menu.
    - (The above could be level up/down, with up disabled at the highest level).
    - Create a timer to show how much longer the player needs to survive.
    - Create a countdown timer before the game starts - this gives the player a chance to make a strategy.
    - Remove the background rectangle (or set opacity to 0.5).
    - Add instructions.
    - Make the first levels a bit easier.
    - Add copyright/version window.
    - Make it possible to place particles that are sticky after level 15
    - Add license text.
    - Test on Maemo and Symbian
    - Publish to Ovi Store
    - Test with Meego SDK
    - Test with Android NDK
*/
// gui
const qreal globalScale = 3;
// charges
const qreal enemyCharge = -4;
const qreal playerCharge = 4;
const qreal simpleCharge = 2.5;

const int incrementChargeNum = 1;
const int baseChargeNum = 3;
// time
const int baseTime = 10;
const int timeIncrement = 2;


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
    positiveButton->setPosition(QVector2D(92,30));
    positiveButton->setScale(12);
    positiveButton->setButtonType(Button::ButtonPositive);

    negativeButton = new Button();
    addItem(negativeButton);
    negativeButton->setScale(12);
    negativeButton->setPosition(QVector2D(92,45));
    negativeButton->setButtonType(Button::ButtonNegative);

    pauseGameButton = new Button();
    prepareButton(pauseGameButton);
    pauseGameButton->setPosition(QVector2D(92,10));
    pauseGameButton->setImage(":/images/button-pause.png");
    QFont font;
    font.setFamily("NovaSquare");
    font.setPixelSize(toFp(5));
    timerText = addText("",font);
    timerText->setDefaultTextColor(QColor(250,250,250,220));
    // end add in-game menu

    // Add menu
    menuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    menuBackgroundRect->show();
    menuBackgroundRect->setOpacity(0.9);
    menuBackgroundRect->setZValue(98);
    menuBackgroundBlur.setBlurRadius(toFp(2));
    menuBackgroundRect->setGraphicsEffect(&menuBackgroundBlur);

    continueButton = new Button();
    prepareButton(continueButton);
    continueButton->setScale(30);
    continueButton->setPosition(QVector2D(50,50));
    continueButton->setImage(":/images/button-continue.png");

    retryButton = new Button();
    prepareButton(retryButton);
    retryButton->setPosition(QVector2D(70,50));
    retryButton->setScale(10);
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
    menuTitleText->setZValue(99);
    menuTitleText->setDefaultTextColor(menuFontColor);
    // end add menu

    // set up timer
    levelTimer = new QTimer(this);
    levelTimer->setInterval(1000);
    connect(levelTimer, SIGNAL(timeout()), SLOT(updateTime()));
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
}

void GameScene::prepareButton(Button *button) {
    addItem(button);
    button->setScale(16);
    button->setZValue(99);
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
    menuBackgroundRect->setRect(toFp(0),
                                toFp(0),
                                toFp(100),
                                toFp(100));
    timerText->setPos(toFp(90,false),toFp(55,true));
    menuTitleText->setPos(0,toFp(15,true));
    menuTitleText->setTextWidth(toFp(100));
    QFont menuFont = menuTitleText->font();
    menuFont.setPixelSize((int)toFp(10,true));
    menuTitleText->setFont(menuFont);
}

void GameScene::updateTime() {
    levelTime--;
    timerText->setPlainText(QString::number(levelTime));
    qDebug() << levelTime;
    if(levelTime < 1) {
        pauseGame();
        menuTitleText->setHtml("<center>Level up!</center>");
        menuTitleText->show();
        startLevel(level + 1);
    }
}

void GameScene::continueGame() {
    timerText->setPlainText(QString::number(levelTime));
    if(gameState() == GameOver) {
        startLevel(level);
    }
    setGameState(GameRunning);
    // show pause button
    pauseGameButton->show();
    negativeButton->show();
    positiveButton->show();
    timerText->show();
    // hide main menu
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
    menuBackgroundRect->show();
    // hide pause button
    pauseGameButton->hide();
    negativeButton->hide();
    positiveButton->hide();
    timerText->hide();
    // show main menu
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

    // reset time
    levelTime = baseTime + timeIncrement * level;

    this->level = level;

    foreach(QGraphicsItem *item, items()) {
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            removeItem(particle);
        }
    }

    // reset buttons
    positiveButton->setEnabled(true);
    negativeButton->setEnabled(true);

    // set number of charges to use
    remainingPositiveCharges = baseChargeNum + incrementChargeNum * level;
    remainingNegativeCharges = baseChargeNum + incrementChargeNum * level;

    // add player
    Particle *player = new Particle();
    addItem(player);
    player->setCharge(playerCharge);
    player->setParticleType(Particle::ParticlePlayer);
    player->setPosition(QVector2D(gameRectF().width() / 2,gameRectF().height() / 2));
    player->setScale(1.2 * globalScale);

    // add enemies
    for(int i=0; i<level; i++) {
        Particle *enemy = new Particle();
        addItem(enemy);
        // should the particle spawn at the topleft, topright, bottomleft or bottomright?
        int left = 1;
        int top = 1;
        while((left == 1 && top == 1) || (left == 1 && top == 2) || (left == 2 && top == 1) || (left == 2 && top == 2)) {
            left = qrand() % 4;
            top = qrand() % 4;
        }
        // make sure that no enemies spawn in the middle region (close to the player)
        /*
         Available regions. The player starts in the center, and all the "touching" regions are not allowed for enemies. Only the edges are available to the enemies.
          _ _ _ _
         |_|_|_|_|
         |_|_|_|_|
         |_|_|_|_|
         |_|_|_|_|

         */
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
        enemy->setCharge(enemyCharge);
        enemy->setScale(1.2 * globalScale);
    }
}

void GameScene::advance() {
    if(_gameState == GameRunning) {
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
    qDebug() << fromFp(event->scenePos().x()) << gameRectF().right();
    if(_gameState == GameRunning){
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
                particle->setCharge(fortegn * simpleCharge);
                particle->setScale(1 * globalScale);
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
    double right = 80;
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
