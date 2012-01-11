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
    GameObject() ,
    m_charge(0),
    _velocity(0,0),
    _sticky(false),
    _particleType(ParticleSimple),
    _mass(particleMass),
    _electroSticky(false),
    collidingWithPlayer(false),
    m_createdTime(0)
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
        qreal opacity = 1 - (fabs(m_charge) - minimumCharge)/(fabs(originalCharge)  - minimumCharge);
        if(m_charge > 0) {
            painter->drawImage(realsize(),gameScene()->positiveImage);
            painter->setOpacity(opacity);
            painter->drawImage(realsize(),gameScene()->neutralImage);
        } else if (m_charge < 0) {
            painter->drawImage(realsize(),gameScene()->negativeImage);
            painter->setOpacity(opacity);
            painter->drawImage(realsize(),gameScene()->neutralImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    } else if(particleType() == ParticlePlayer) {
        qreal opacity = (fabs(m_charge)-fabs(originalCharge))/fabs(originalCharge);
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
    } else if(particleType() == ParticleSlowMotion) {
        painter->drawImage(realsize(),gameScene()->neutralImage);
    }
}

void Particle::advance(int step) {
    Q_UNUSED(step)
    if(!gameScene()) {
        qWarning() << "Particle::advance(): Cannot advance without gameScene";
        return;
    }
    if(createdTime() == 0) {
        setCreatedTime(gameScene()->currentTime);
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
                    double q1q2 = this->m_charge * particle->m_charge;
                    QVector2D F_e;
                    QVector2D F_r;
                    if(length > particleDistances) { // force calculation
                        if(!electroSticky()) { // only calculate electromagnetic forces if the particle isn't electrosticky
                            F_e += rn * forceByLengthSquaredFactor * (q1q2/(lengthSquared));
                            F_e += rn * forceByLengthFactor * (q1q2/(length));
                        }
                        if(this->particleType() == ParticlePlayer && particle->particleType() == ParticleSlowMotion) {
                            particle->collidingWithPlayer = false;
                        }
                    } else { // collision
                        F_r = -rn * springConstant * (length - particleDistances);
                        // Collision between player and slow motion particle
                        if(this->particleType() == ParticlePlayer && particle->particleType() == ParticleSlowMotion) {
                            if(!particle->collidingWithPlayer) {
                                gameScene()->setSlowMotion(true, 3000);
                                particle->collidingWithPlayer = true;
                            }
                        }
                        // Collision between player and enemy
                        if(matchParticles(this, particle, ParticlePlayer, ParticleEnemy)) { // Player-enemy crash - Game Over
                            gameScene()->gameOver();
                        }
                        // Charge exchange
                        double chargediff = fabs(m_charge - particle->m_charge); // charge difference
                        qreal dechargeRateTotal = fabs(length - particleDistances) * dechargeRate * springConstant / 60.0; // including the spring constant to avoid changing the decharge rate when the spring constant is changed
                        if(particleType() != ParticleEnemy && particle->particleType() != ParticleEnemy) { // do not decharge enemies
                            if(particleType() == ParticlePlayer) { // if either particles are the player
                                m_charge += fabs(particle->m_charge) * dt * dechargeRateTotal; // it gets some of the other's charge
                                particle->m_charge -= particle->m_charge * dt * dechargeRateTotal; // that the  other particle loses
                            } else if(particle->particleType() == ParticlePlayer) { // if either particles are the player
                                particle->m_charge += fabs(m_charge) * dt * dechargeRateTotal; // it gets some of the other's charge
                                m_charge -= m_charge * dt * dechargeRateTotal; // that the other particle loses
                            } else {
                                if(m_charge > particle->m_charge) { // the one with most charge loses charge, the other gains
                                    m_charge -= chargediff * dt * dechargeRateTotal;
                                    particle->m_charge += chargediff * dt * dechargeRateTotal;
                                } else if(m_charge < particle->m_charge) {
                                    m_charge += chargediff* dt * dechargeRateTotal;
                                    particle->m_charge -= chargediff * dt * dechargeRateTotal;
                                }
                            }
                        }
                        if(fabs(m_charge) < minimumCharge) {
                            m_charge = 0.0;
                        }
                        if(fabs(particle->m_charge) < minimumCharge) {
                            particle->m_charge = 0.0;
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

bool Particle::matchParticles(Particle* particle1, Particle* particle2, ParticleType type1, ParticleType type2) {
    if((particle1->particleType() == type1 && particle2->particleType() == type2) ||
            (particle1->particleType() == type2 && particle2->particleType() == type1)) { // Player-enemy crash - Game Over
        return true;
    } else {
        return false;
    }
}


QRectF Particle::boundingRect() const {
    return realsize(false);
}
