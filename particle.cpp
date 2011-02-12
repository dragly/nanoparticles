#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QVector2D>

#include "particle.h"
#include "gamescene.h"

#include <math.h>

const double dechargeRate = 0.01;

Particle::Particle() :
        GameObject() , charge(0), _velocity(0,0)
{
//    setSize(QRectF(0,0,2,2));

}

void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(charge > 0) {
        painter->drawImage(realsize(),gameScene()->positiveImage);
    } else if (charge < 0) {
        painter->drawImage(realsize(),gameScene()->negativeImage);
    } else {
        painter->drawImage(realsize(),gameScene()->neutralImage);
    }
}

void Particle::advance(int step) {
    Q_UNUSED(step)
    float dt = ((GameScene*)gameScene())->dt();

    QList<QGraphicsItem *> items= gameScene()->items();

    QVector2D F;
    foreach(QGraphicsItem* item, items) {
        if (item == this)
            continue;
        if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
            double particleDistances = scale() * size().width() / 2.0 + scale() * particle->size().width() / 2.0;
            QVector2D r = QVector2D(this->position() - particle->position());
            if(r.length() != 0) {
                QVector2D rn = r.normalized();
                double q1q2 = this->charge * particle->charge;
                QVector2D F_e = rn * (q1q2/(r.length()));
                QVector2D F_r;
                if(r.length() < particleDistances) {
                    F_r = -rn * 50*(r.length() - particleDistances);

                    // Decharging
                    double chargediff = fabs(charge - particle->charge);
                    if(charge > particle->charge) {
                        charge -= chargediff * dt * dechargeRate;
                        particle->charge += chargediff * dt * dechargeRate;
                    } else if(charge < particle->charge) {
                        charge += chargediff* dt * dechargeRate;
                        particle->charge -= chargediff * dt * dechargeRate;
                    }
                    if(fabs(charge) < 1.0) {
                        charge = 0.0;
                    }
                    if(fabs(particle->charge) < 1.0) {
                        particle->charge = 0.0;
                    }
                }
                F += F_e + F_r;
            }
        }
    }
    QVector2D F_d = -0.2 * velocity(); //damping
    F += F_d;
    QVector2D a = F / 1.0;
    _velocity += a * dt;
    setPosition(position() + velocity() * dt);

    // restrain edges
    double minx = gameScene()->gameRectF().left() + size().width() * scale() / 2;
    if(position().x() < minx) {
        setPosition(QVector2D(minx,position().y()));
        _velocity.setX(-_velocity.x());
    }
    double miny = gameScene()->gameRectF().top() + size().width() * scale() / 2;
    if(position().y() < miny) {
        setPosition(QVector2D(position().x(),miny));
        _velocity.setY(-_velocity.y());
    }
    double maxx = gameScene()->gameRectF().right() - size().width() * scale() / 2;
    if(position().x() > maxx) {
        setPosition(QVector2D(maxx,position().y()));
        _velocity.setX(-_velocity.x());
    }
    double maxy = gameScene()->gameRectF().bottom() - size().width() * scale() / 2;
    if(position().y() > maxy) {
        setPosition(QVector2D(position().x(),maxy));
        _velocity.setY(-_velocity.y());
    }
}
