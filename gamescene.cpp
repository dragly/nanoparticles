#include "gamescene.h"
#include "particle.h"
#include <QDebug>

GameScene::GameScene(QObject *parent) :
        QGraphicsScene(parent)
{
    level = 1;
    gameState = GameRunning;
    dt = 0;
    firstStep = true;

    setSceneRect(0, 0, 400, 300);
    setItemIndexMethod(QGraphicsScene::NoIndex);

    startLevel(1);

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(advance()));
    timer.start(10);
    time.start();
}

void GameScene::startLevel(int level) {
    this->level = level;
    items().clear();
    for(int i=0; i<level; i++) {
        Particle *particle = new Particle();
        particle->setPos(20,30);
        particle->setCharge(3.0);
        addItem(particle);
        Particle *particleA = new Particle();
        particleA->setPos(30,40);
        particleA->setCharge(-3.0);
        addItem(particleA);
        Particle *particleB = new Particle();
        particleB->setPos(40,50);
        particleB->setCharge(3.0);
        addItem(particleB);
    }
    qDebug() << "dt is " << dt;
}

void GameScene::advance() {
    currentTime = time.elapsed();
    if(firstStep) {
        dt = 0;
        firstStep = false;
    } else {
        dt = (currentTime - lastFrameTime) / 1000.0;
    }
    qDebug() << "dt is " << dt;
    QGraphicsScene::advance();
    lastFrameTime = currentTime;
}
