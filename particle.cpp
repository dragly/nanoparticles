#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QVector2D>

#include "particle.h"
#include "gamescene.h"

#include <math.h>

const qreal dechargeRate = 0.2;
const qreal springConstant = 75.0;
const qreal minimumCharge = 2.5;
const qreal dampingFactor = 0.18; // a scaling of the damping force
const qreal particleMass = 1.3;
const qreal forceByLengthSquaredFactor = 0.4;
const qreal forceByLengthFactor = 0.9;

Particle::Particle() :
        GameObject() , charge(0), _velocity(0,0), _sticky(false), _particleType(ParticleSimple), _mass(particleMass), _electroSticky(false)
{
}

void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(!gameScene()) {
        qWarning() << "Particle::paint(): Cannot paint without gameScene";
        return;
    }
    if(particleType() == ParticleSimple) {
        qreal opacity = 1 - (fabs(charge) - minimumCharge)/(fabs(originalCharge)  - minimumCharge);
        if(charge > 0) {
            painter->drawImage(realsize(),gameScene()->positiveImage);
            painter->setOpacity(opacity);
            painter->drawImage(realsize(),gameScene()->neutralImage);
        } else if (charge < 0) {
            painter->drawImage(realsize(),gameScene()->negativeImage);
            painter->setOpacity(opacity);
            painter->drawImage(realsize(),gameScene()->neutralImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    } else if(particleType() == ParticlePlayer) {
        qreal opacity = (fabs(charge)-fabs(originalCharge))/fabs(originalCharge);
        if(opacity > 1.0) {
            opacity = 1.0;
        } else if(opacity < 0.0) {
            opacity = 0.0;
        }
        //        painter->setOpacity(pacity);
        painter->drawImage(realsize(),gameScene()->playerImage);
        painter->setOpacity(opacity);
        painter->drawImage(realsize(),gameScene()->playerOverchargedImage);

    } else if(particleType() == ParticleEnemy) {
        painter->drawImage(realsize(),gameScene()->enemyImage);
    }
}

void Particle::advance(int step) {
    Q_UNUSED(step)
    if(!gameScene()) {
        qWarning() << "Particle::advance(): Cannot advance without gameScene";
        return;
    }
    setPosition(nextPosition); // we calculated our next position in the last timestep, now, lets use it
    float dt = gameScene()->dt();
    QVector2D F;
    if(!sticky()) { // only calculate forces if the particle isn't sticky
        QList<QGraphicsItem *> items= gameScene()->items();
        foreach(QGraphicsItem* item, items) {
            if (item == this)
                continue;
            if(Particle* particle = qgraphicsitem_cast<Particle*>(item)) {
                double particleDistances = scale() * size().width() / 2.0 + particle->scale() * particle->size().width() / 2.0;
                QVector2D r = QVector2D(this->position() - particle->position());
                // save the length and lengthSquared to memory to avoid recalculations (with square roots!)
                double length = r.length();
                double lengthSquared = r.lengthSquared();
                if(lengthSquared != 0) {
                    QVector2D rn = r.normalized();
                    double q1q2 = this->charge * particle->charge;
                    QVector2D F_e;
                    QVector2D F_r;
                    if(length > particleDistances) {
                        if(!electroSticky()) { // only calculate electromagnetic forces if the particle isn't electrosticky
                            F_e += rn * forceByLengthSquaredFactor * (q1q2/(lengthSquared));
                            F_e += rn * forceByLengthFactor * (q1q2/(length));
                        }
                    } else {
                        F_r = -rn * springConstant * (length - particleDistances);
                        if((particle->particleType() == ParticleEnemy && particleType() == ParticlePlayer) ||
                           (particle->particleType() == ParticlePlayer && particleType() == ParticleEnemy)) { // Player-enemy crash - Game Over
                            gameScene()->gameOver();
                        }
                        // Charge exchange
                        double chargediff = fabs(charge - particle->charge); // charge difference
                        qreal dechargeRateTotal = fabs(length - particleDistances) * dechargeRate * springConstant / 60.0; // including the spring constant to avoid changing the decharge rate when the spring constant is changed
                        if(particleType() != ParticleEnemy && particle->particleType() != ParticleEnemy) { // do not decharge enemies
                            if(particleType() == ParticlePlayer) { // if either particles are the player
                                charge += fabs(particle->charge) * dt * dechargeRateTotal; // it gets some of the other's charge
                                particle->charge -= particle->charge * dt * dechargeRateTotal; // that the  other particle loses
                            } else if(particle->particleType() == ParticlePlayer) { // if either particles are the player
                                particle->charge += fabs(charge) * dt * dechargeRateTotal; // it gets some of the other's charge
                                charge -= charge * dt * dechargeRateTotal; // that the other particle loses
                            } else {
                                if(charge > particle->charge) { // the one with most charge loses charge, the other gains
                                    charge -= chargediff * dt * dechargeRateTotal;
                                    particle->charge += chargediff * dt * dechargeRateTotal;
                                } else if(charge < particle->charge) {
                                    charge += chargediff* dt * dechargeRateTotal;
                                    particle->charge -= chargediff * dt * dechargeRateTotal;
                                }
                            }
                        }
                        if(fabs(charge) < minimumCharge) {
                            charge = 0.0;
                        }
                        if(fabs(particle->charge) < minimumCharge) {
                            particle->charge = 0.0;
                        }
                    }
                    F += F_e + F_r;
                }
            }
        }
    }
    QVector2D F_d = - dampingFactor * velocity(); //damping
    F += F_d;
    QVector2D a = F / mass();
    _velocity += a * dt;
    // lets not yet set our position, but save it temporarily. This avoids calculating forces for other particles with wrong positions
    nextPosition = position() + velocity() * dt;

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


QRectF Particle::boundingRect() const {
    return realsize(false);
}
