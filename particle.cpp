#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QVector2D>

#include "particle.h"
#include "gamescene.h"

#include <math.h>

const double dechargeRate = 0.01;

Particle::Particle()
{
    scale = 5.0;
    setSize(QRectF(0,0,2,2));
}

void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    GameScene* gameScene = (GameScene*)scene();
    if(charge > 0) {
        painter->drawImage(realsize(),gameScene->positiveImage);
    } else if (charge < 0) {
        painter->drawImage(realsize(),gameScene->negativeImage);
    } else {
        painter->drawImage(realsize(),gameScene->neutralImage);
    }
}

QRectF Particle::boundingRect() const {
    double width = (QRectF(0,0,2,2).width()) / 100 * scene()->width();
    double height = (QRectF(0,0,2,2).height()) / 100 * scene()->width();
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

void Particle::advance(int step) {
    float dt = ((GameScene*)scene())->dt();

    QList<QGraphicsItem *> items= scene()->items();

    QVector2D F;
    foreach(QGraphicsItem* item, items) {
        if (item == this)
            continue;
        if(Particle* particle = (Particle*)item) {
            double particleDistances = size().width() / 2.0 + particle->size().width() / 2.0;
            QVector2D r = QVector2D(this->position() - particle->position());
            double lengthSquared = pow(r.x(), 2) + pow(r.y(), 2);
            double length = sqrt(lengthSquared);
            if(length != 0) {
                QVector2D rn = r / length;
                double q1q2 = this->charge * particle->charge;
                QVector2D F_e = rn * (q1q2/(length));
                QVector2D F_r;
                if(length < particleDistances) {
                    F_r = -rn * 50*(length - particleDistances);

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
    if(position().x() < 0 + size().width()) {
        setPosition(QVector2D(0 + size().width(),position().y()));
        _velocity.setX(-_velocity.x());
    }
    if(position().y() < 0 + size().width()) {
        setPosition(QVector2D(position().x(),0 + size().width()));
        _velocity.setY(-_velocity.y());
    }
    if(position().x() > 100 - size().width()) {
        setPosition(QVector2D(100 - size().width(),position().y()));
        _velocity.setX(-_velocity.x());
    }
    if(position().y() > scene()->height() / scene()->width() * 100 - size().width()) {
        setPosition(QVector2D(position().x(),scene()->height() / scene()->width() * 100 - size().width()));
        _velocity.setY(-_velocity.y());
    }
}
QRectF Particle::realsize() {
    double width = toFp(_size.width());
    double height = toFp(_size.height());
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

double Particle::toFp(double number) {
    return number / 100 * scene()->width();
}

double Particle::fromFp(double number) {
    return number * 100 / scene()->width();
}

void Particle::setPosition(QVector2D position) {
     this->_position = position;
     setPos(toFp(position.x()),toFp(position.y()));
}
