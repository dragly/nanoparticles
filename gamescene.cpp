#include "gamescene.h"
#include "particle.h"
#include "button.h"

#include <QDebug>

/* TODO
    - Create a system for levels.
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
const qreal enemyCharge = -4;
const qreal playerCharge = 4;
const qreal simpleCharge = 2;

GameScene::GameScene(QObject *parent) :
        QGraphicsScene(parent)
{
    level = 1;
    gameState = GameRunning;
    _dt = 0;
    firstStep = true;

    selectedParticleType = ParticlePositive;

    // load images
    positiveImage = QImage(":/images/particle-positive.svg");
    negativeImage = QImage(":/images/particle-negative.svg");
    neutralImage = QImage(":/images/particle-neutral.svg");
    playerImage = QImage(":/images/particle-player.svg");
    playerOverchargedImage = QImage(":/images/particle-player-overcharged.svg");
    enemyImage = QImage(":/images/particle-enemy.svg");

    setSceneRect(0, 0, 400, 300); // just for init
    setItemIndexMethod(QGraphicsScene::NoIndex);
    // Add buttons
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
    // end add buttons

    // Add menu
    menuBackgroundRect = addRect(20,20,200,200,QPen(Qt::black),QBrush(Qt::black));
    menuBackgroundRect->setVisible(true);
    menuBackgroundRect->setOpacity(0.5);
    menuBackgroundRect->setZValue(99);
    // end add menu

    gameState = GamePaused;

    // add signals
    QObject::connect(positiveButton, SIGNAL(clicked()), SLOT(clickedPositiveButton()));
    QObject::connect(negativeButton, SIGNAL(clicked()), SLOT(clickedNegativeButton()));

    // Start level and start timers
    startLevel(1);

    QObject::connect(&timer, SIGNAL(timeout()), SLOT(advance()));
    timer.start(10);
    time.start();
}

void GameScene::clickedPositiveButton() {
    selectedParticleType = ParticlePositive;
}

void GameScene::clickedNegativeButton() {
    selectedParticleType = ParticleNegative;
}

void GameScene::resized() {
    foreach(QGraphicsItem* item, items()) {
        if(GameObject* button = (GameObject*)item) {
            button->setPosition(button->position());
        }
    }
}

void GameScene::startLevel(int level) {
    this->level = level;
    items().clear();

    // reset buttons
    positiveButton->setEnabled(true);
    negativeButton->setEnabled(true);

    // set number of charges to use
    remainingPositiveCharges = 5 * level;
    remainingNegativeCharges = 5 * level;

    // add player
    Particle *player = new Particle();
    addItem(player);
    player->setCharge(playerCharge);
    player->setParticleType(Particle::ParticlePlayer);
    player->setPosition(QVector2D(gameRectF().width() / 2,gameRectF().height() / 2));
    player->setScale(1.2 * globalScale);

    // add enemies
    for(int i=0; i<level*3; i++) {
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
    if(gameState == GameRunning) {
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
    if(gameState == GameRunning){
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
