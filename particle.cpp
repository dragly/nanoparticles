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
    setSize(QRectF(0,0,2,2));
}

void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(charge > 0) {
        painter->drawImage(realsize(),gameScene()->positiveImage);
    } else if (charge < 0) {
        painter->drawImage(realsize(),gameScene()->negativeImage);
    } else {
        painter->drawImage(realsize(),gameScene()->neutralImage);
    }
}

QRectF Particle::boundingRect() const {
    double width = (QRectF(0,0,2,2).width()) / 100 * scene()->width();
    double height = (QRectF(0,0,2,2).height()) / 100 * scene()->width();
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

void Particle::advance(int step) {
    float dt = ((GameScene*)gameScene())->dt();

    QList<QGraphicsItem *> items= gameScene()->items();

    QVector2D F;
    foreach(QGraphicsItem* item, items) {
        if (item == this)
            continue;
        if(Particle* particle = (Particle*)item) {
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
    double maxy = gameScene()->height() / gameScene()->width() * gameScene()->gameRectF().bottom() - size().width() * scale() / 2;
    if(position().y() > maxy) {
        setPosition(QVector2D(position().x(),maxy));
        _velocity.setY(-_velocity.y());
    }
}
QRectF Particle::realsize() {
    double width = toFp(_size.width());
    double height = toFp(_size.height());
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

double Particle::toFp(double number) {
    return number / 100 * gameScene()->width();
}

double Particle::fromFp(double number) {
    return number * 100 / gameScene()->width();
}

void Particle::setPosition(QVector2D position) {
     this->_position = position;
     setPos(toFp(position.x()),toFp(position.y()));
}
