#include "gamescene.h"
#include "particle.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

GameScene::GameScene(QObject *parent) :
        QGraphicsScene(parent)
{
    level = 1;
    gameState = GameRunning;
    _dt = 0;
    firstStep = true;

    // load images
    positiveImage = QImage(":/images/particle-positive.svg");
    negativeImage = QImage(":/images/particle-negative.svg");
    neutralImage = QImage(":/images/particle-neutral.svg");

    setSceneRect(0, 0, 400, 300); // just for init
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
        for(int j=0; j<0; j++) {
            Particle *particle = new Particle();
            particle->setPos(((double)qrand()/(double)RAND_MAX)*50,((double)qrand()/(double)RAND_MAX)*40);

            int fortegn;
            if(qrand() % 2) {
                fortegn = 1;
            } else {
                fortegn = -1;
            }
            particle->setCharge(fortegn * ((double)qrand()/(double)RAND_MAX) * 4.0);
            addItem(particle);
        }
    }
}

void GameScene::advance() {
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

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    int fortegn = -1;
    if(event->button() == Qt::LeftButton) {
        if(event->modifiers() & Qt::ShiftModifier) {
            fortegn = -1;
        } else {
            fortegn = 1;
        }
    } else if(event->button() == Qt::RightButton) {
        fortegn = -1;
    }
    Particle *particle = new Particle();
    addItem(particle);
    QVector2D position = QVector2D(particle->fromFp(event->scenePos().x()),particle->fromFp(event->scenePos().y()));
    particle->setPosition(position);
    particle->setCharge(fortegn * 2.0);
}

float GameScene::dt() {
    if(_dt < 0.05)
        return _dt;
    else
        return 0.05;
}
