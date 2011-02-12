#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>

/* TODO
    - Create a system for levels.
    - Create level menu (and figure out how to format this).
    - Save highest achieved level, and make this the highest possible level to select in the level menu.
    - (The above could be level up/down, with up disabled at the highest level).
    - Create a timer to show how much longer the player needs to survive.
    - Create a countdown timer before the game starts - this gives the player a chance to make a strategy.
    - Make it possible to place particles that are sticky after level 5
    - Add license text.
    - Test on Maemo and Symbian
    - Publish to Ovi Store
    - Test with Meego SDK
    - Test with Android NDK
*/

const qreal globalScale = 3;
const qreal enemyCharge = -6;
const qreal playerCharge = 6;
const qreal simpleCharge = 3;

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
    positiveButton->setPosition(QVector2D(90,5));
    positiveButton->setScale(7);
    positiveButton->setButtonType(Button::ButtonPositive);

    negativeButton = new Button();
    addItem(negativeButton);
    negativeButton->setScale(7);
    negativeButton->setPosition(QVector2D(90,15));
    negativeButton->setButtonType(Button::ButtonNegative);

    pauseGameButton = new Button();
    prepareButton(pauseGameButton);
    pauseGameButton->setScale(7);
    pauseGameButton->setImage(":/images/button-pause.png");
    QFont font;
    font.setFamily("NovaSquare");
    font.setPixelSize(20);
    timerText = addText("x",font);
    timerText->setDefaultTextColor(QColor(250,250,250,220));
    // end add in-game menu

    // Add menu
    menuBackgroundRect = addRect(0,0,1,1,QPen(Qt::black),QBrush(Qt::black));
    menuBackgroundRect->setVisible(true);
    menuBackgroundRect->setOpacity(0.9);
    menuBackgroundRect->setZValue(98);
    menuBackgroundBlur.setBlurRadius(toFp(2));
    menuBackgroundRect->setGraphicsEffect(&menuBackgroundBlur);

    continueGameButton = new Button();
    prepareButton(continueGameButton);
    continueGameButton->setPosition(QVector2D(20,20));
    continueGameButton->setImage(":/images/button-continue.png");

    retryGameButton = new Button();
    prepareButton(retryGameButton);
    retryGameButton->setPosition(QVector2D(30,20));
    retryGameButton->hide();
    retryGameButton->setImage(":/images/button-retry.png");

    exitGameButton = new Button();
    prepareButton(exitGameButton);
    exitGameButton->setPosition(QVector2D(40,20));
    exitGameButton->setImage(":/images/button-exit.png");
    // next/prev level
    prevLevelGameButton = new Button();
    prepareButton(prevLevelGameButton);
    prevLevelGameButton->setPosition(QVector2D(20,30));
    prevLevelGameButton->setImage(":/images/button-leveldown.png");

    nextLevelButton = new Button();
    prepareButton(nextLevelButton);
    nextLevelButton->setPosition(QVector2D(30,30));
    nextLevelButton->setImage(":/images/button-levelup.png");

    // menu text
    QFont menuFont;
    QColor menuColor(250,250,250,220);
    menuFont.setFamily("NovaSquare");
    menuFont.setPixelSize(30);
    gameOverText = addText("Game Over!", menuFont);
    gameOverText->hide();
    gameOverText->setZValue(99);
    gameOverText->setDefaultTextColor(menuColor);
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
    connect(continueGameButton, SIGNAL(clicked()), SLOT(continueGame()));
    connect(pauseGameButton, SIGNAL(clicked()), SLOT(pauseGame()));
    connect(retryGameButton, SIGNAL(clicked()), SLOT(retryGame()));
    connect(exitGameButton, SIGNAL(clicked()), SLOT(exitGame()));
    // next/prev level
    connect(nextLevelButton, SIGNAL(clicked()), SLOT(clickedNextLevelButton()));
    connect(prevLevelGameButton, SIGNAL(clicked()), SLOT(clickedPrevLevelButton()));

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
    button->setScale(7);
    button->setZValue(99);
    button->setButtonType(Button::StandardButton);
}

void GameScene::resized() {
    foreach(QGraphicsItem* item, items()) {
        if(GameObject* button = (GameObject*)item) {
            button->setPosition(button->position());
        }
    }
    menuBackgroundRect->setRect(toFp(5),
                                toFp(5),
                                toFp(95),
                                toFp(height()/width() * 95));
    timerText->setPos(toFp(87),toFp(height()/width() * 45));
    gameOverText->setPos(toFp(5),toFp(5));
    pauseGameButton->setPosition(QVector2D(90,height()/width() * 80));
}

void GameScene::updateTime() {
    levelTime--;
    timerText->setPlainText(QString::number(levelTime));
    qDebug() << levelTime;
    if(levelTime < 1) {
        pauseGame();
        startLevel(level + 1);
    }
}

void GameScene::continueGame() {
    timerText->setPlainText(QString::number(levelTime));
    if(gameState() == GameOver) {
        startLevel(level);
    }
    setGameState(GameRunning);
    // hide main menu
    menuBackgroundRect->hide();
    continueGameButton->hide();
    retryGameButton->hide();
    exitGameButton->hide();
    nextLevelButton->hide();
    prevLevelGameButton->hide();
    // hide main menu text
    gameOverText->hide();

    // start timer
    levelTimer->start();
    qDebug() << "Game started";
}

void GameScene::pauseGame() {
    if(gameState() == GameRunning) {
        setGameState(GamePaused);
    }
    if(gameState() != GameStarted) {
        retryGameButton->show();
    }
    if(gameState() != GameOver) {
        continueGameButton->show();
    }
    menuBackgroundRect->show();
    // show main menu
    nextLevelButton->show();
    prevLevelGameButton->show();
    exitGameButton->show();
    // pause timer
    levelTimer->stop();
    qDebug() << "Game paused";
}

void GameScene::gameOver() {
    setGameState(GameOver);
    menuBackgroundRect->show();
    gameOverText->show();
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
    levelTime = 5 + 2*level;

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
    remainingPositiveCharges = 2 + 1 * level;
    remainingNegativeCharges = 2 + 1 * level;

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
        while(left == 1 && top == 1) {
            left = qrand() % 3;
            top = qrand() % 3;
        }
        // make sure that no enemies spawn in the middle region (close to the player)
        /*
         Game regions. The x marks the player.
          _ _ _
         |_|_|_|
         |_|x|_|
         |_|_|_|

         */
        qDebug() << gameRectF() << gameRectF().left() + gameRectF().width()/3.0 * left << gameRectF().width()/3.0 << left;
        QRectF spawnRect(gameRectF().left() + gameRectF().width()/3.0 * left, gameRectF().top() + gameRectF().height()/3.0 * top, gameRectF().width()/3.0, gameRectF().height()/3.0);
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

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    //Create particles
    qDebug() << event->scenePos().x() << gameRectF().right() * width() / 100;
    if(_gameState == GameRunning){
        if(event->scenePos().x() < gameRectF().right() * width() / 100) {
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
                    remainingNegativeCharges--;
                } else {
                    remainingPositiveCharges--;
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



double GameScene::toFp(double number) {
    return number / 100 * width();
}

double GameScene::fromFp(double number) {
    return number * 100 / width();
}
