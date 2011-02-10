#include "gamescene.h"
#include "particle.h"
#include <QDebug>

/* TODO
    - Create svgs for player and dangerous particles.
    - Create special particles for player and dangerous particles (particle type).
    - Make dangerous particles sticky (at least some of them).
    - Create a system for levels.
    - Add license text.
*/

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

    setSceneRect(0, 0, 400, 300); // just for init
    setItemIndexMethod(QGraphicsScene::NoIndex);

    startLevel(1);
    // Add buttons
    positiveButton = new QPushButton();
    positiveButton->setGeometry(width()-40,10,30,30);
    positiveButton->setStyleSheet("QPushButton {background: transparent; border-image: url(:/images/particle-positive.svg);}");;
    addWidget(positiveButton);
    negativeButton = new QPushButton();
    negativeButton->setGeometry(width()-40,50,30,30);
    negativeButton->setStyleSheet("background: transparent; border-image: url(:/images/particle-negative.svg);");;
    addWidget(negativeButton);
    // end add buttons

    QObject::connect(positiveButton, SIGNAL(clicked()), this, SLOT(clickedPositiveButton()));
    QObject::connect(negativeButton, SIGNAL(clicked()), this, SLOT(clickedNegativeButton()));


    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(advance()));
    timer.start(10);
    time.start();
}

void GameScene::clickedPositiveButton() {
    selectedParticleType = ParticlePositive;
}

void GameScene::clickedNegativeButton() {
    selectedParticleType = ParticleNegative;
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
    //Create particles
    qDebug() << event->scenePos().x() << gameRectF().right() * width() / 100;
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
        Particle *particle = new Particle();
        addItem(particle);
        QVector2D position = QVector2D(particle->fromFp(event->scenePos().x()),particle->fromFp(event->scenePos().y()));
        particle->setPosition(position);
        particle->setCharge(fortegn * 3.5);
        particle->setScale(2);
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
    gameRect.setRight(80);
    gameRect.setBottom(100);
    return gameRect;
}
