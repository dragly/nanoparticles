#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

#include "particle.h"
#include "gamescene.h"

#include <math.h>

Particle::Particle()
{
    particleImage = QImage(":/images/particle-positive.png");
}

void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->drawImage(QRectF(0,0,75,75),particleImage);
}

QRectF Particle::boundingRect() const {
    return QRectF(20.,20.,20.,20.);
}

void Particle::advance(int step) {
    float dt = ((GameScene*)scene())->dt;

    QList<QGraphicsItem *> items= scene()->items();

    QPointF F;
    foreach(QGraphicsItem* item, items) {
        if (item == this)
            continue;
        if(Particle* particle = (Particle*)item) {
            QPointF r = this->pos() - particle->pos();
            double lengthSquared = pow(r.x(), 2) + pow(r.y(), 2);
            double length = sqrt(lengthSquared);
            if(length != 0) {
                QPointF rn = r / length;
                double q1q2 = this->charge * particle->charge;
                QPointF F_e = rn * (q1q2/length);
                qDebug() << "F_e" << F_e;
                QPointF F_r = rn * (fabs(q1q2)/lengthSquared); // repulsion
                qDebug() << "F_r" << F_r;
                F += F_e + F_r;
            }
        }
    }
    QPointF a = F / 1.0;
    velocity += a * dt;
    setPos(pos() + velocity * dt);
}
