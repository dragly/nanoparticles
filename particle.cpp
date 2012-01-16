#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QVector2D>

#include <math.h>

#include "particle.h"
#include "gamescene.h"
#include "utils.h"

const qreal dechargeRate = 0.2;
const qreal springConstant = 75.0;
const qreal minimumCharge = 2.5;
const qreal edgeBounceDampPartyEnemy = 0.25;
// friction
const qreal dampingFactor = 0.18; // a scaling of the damping force
const qreal dampingFactorParty = 0.4; // a scaling of the damping force
const qreal particleMass = 1.3;
const qreal forceByLengthSquaredFactor = 0.4;
const qreal forceByLengthFactor = 0.9;

// special particles
const qreal repellentCharge = -500;
const qreal repellentDechargeRate = 200;

// scaling
const qreal globalScale = 2.5;
const qreal playerScale = 1.3;
const qreal enemyScale = 1.35;
const qreal simpleScale = 1.25;
const qreal slowMotionScale = 2.0;
const qreal repellentScale = 1.2;
const qreal glowingScale = 1.1;
const qreal transferScale = 1.2;

// party mode settings
const qreal partyDisintegrationSimpleTime = 10.0;
const qreal partyDisintegrationGlowingTime = 2.0;
const qreal partyDisintegrationSpecialTime = 20.0;

Particle::Particle(GameScene *gameScene) :
    GameObject(gameScene) ,
    m_charge(0),
    _velocity(0,0),
    m_particleType(ParticleSimple),
    _sticky(false),
    _mass(particleMass),
    _electroSticky(false),
    hasCollidedWithPlayer(false),
    m_dueTime(0)
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
        painter->drawImage(realsize(),gameScene()->slowMotionImage);
    } else if(particleType() == ParticleRepellent) {
        painter->drawImage(realsize(),gameScene()->repellentImage);
    } else if(particleType() == ParticleGlowing) {
        painter->drawImage(realsize(),gameScene()->glowingImage);
    } else if(particleType() == ParticleTransfer) {
        painter->drawImage(realsize(),gameScene()->transferImage);
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
    // Party mode stuff
    if(gameScene()->gameMode() == GameScene::ModeParty) {
        // Special charges
        if(particleType() == ParticleRepellent) {
            if(charge() < 0) {
                setCharge(charge() + dt * repellentDechargeRate);
            }
        }
        // scale by due time
        setDueTime(dueTime() - dt);
        if(dueTime() > 0) {
            setScale(originalScale * dueTime() / originalDueTime);
        }
        // Shake watches on slow motion
        if(particleType() == ParticleSlowMotion) {
            qDebug() << gameScene()->isSlowMotionEnabled();
            if(gameScene()->isSlowMotionEnabled()) {
                setRotation(rotation() + 360 * dt);
            }
        }
    }
    // Calculate forces
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
                        if(!electroSticky() && q1q2 != 0 &&// only calculate electromagnetic forces if the particle isn't electrosticky
                                !matchParticles(this, particle, ParticlePlayer, ParticleRepellent) && // repellent only acts on enemy charges
                                !matchParticles(this, particle, ParticleSimple, ParticleRepellent)) {
                            F_e += rn * forceByLengthSquaredFactor * (q1q2/(lengthSquared));
                            F_e += rn * forceByLengthFactor * (q1q2/(length));
                        }
                        if(this->particleType() == ParticlePlayer) {
                            particle->hasCollidedWithPlayer = false;
                            particle->hasCollidedWithPlayer = false;
                        }
                    } else { // collision
                        // No collision between effects and player
                        if(!matchParticles(this, particle, ParticlePlayer, ParticleGlowing) &&
                                !matchParticles(this, particle, ParticleGlowing, ParticleGlowing)) {
                            F_r = -rn * springConstant * (length - particleDistances);
                        }
                        // Collision between player and slow motion particle
                        if(this->particleType() == ParticlePlayer && particle->particleType() == ParticleSlowMotion) {
                            if(!particle->hasCollidedWithPlayer) {
                                gameScene()->enableSlowMotion(3500);
                                particle->hasCollidedWithPlayer = true;
                            }
                        }
                        // Collision between player and transfer particle
                        if(this->particleType() == ParticlePlayer && particle->particleType() == ParticleTransfer) {
                            if(!particle->hasCollidedWithPlayer) {
                                gameScene()->addTransferParticle();
                                particle->hasCollidedWithPlayer = true;
                            }
                        }
                        // Collision between player and repellent particle
                        if(this->particleType() == ParticlePlayer && particle->particleType() == ParticleRepellent) {
                            if(particle->charge() >= 0) {
                                qDebug() << "Crashed with repellent charge";
                                particle->setCharge(repellentCharge);
                                int nEffectParticles = 32;
                                for(int i = 0; i < nEffectParticles; i++) {
                                    Particle *particleEffect = new Particle(gameScene());
                                    gameScene()->addItem(particleEffect);
                                    // Spread particles out in a circle around the repellent
                                    qreal xpos = particle->position().x() + 1 + (1 + ran0() * 0.1) * (particle->scale() + scale()) * cos(2*M_PI * i / nEffectParticles);
                                    qreal ypos = particle->position().y() + 1 + (1 + ran0() * 0.1) * (particle->scale() + scale()) * sin(2*M_PI * i / nEffectParticles);
                                    qreal xvel = (7 + ran0() * 3) * cos(2*M_PI * i / nEffectParticles);
                                    qreal yvel = (7 + ran0() * 3) * sin(2*M_PI * i / nEffectParticles);
                                    particleEffect->setPosition(QVector2D(xpos,ypos));
                                    particleEffect->setVelocity(QVector2D(xvel,yvel));
                                    particleEffect->setMass(10);
                                    particleEffect->setCharge(0);
                                    particleEffect->setParticleType(ParticleGlowing);
                                }
                            }
                        }
                        // Collision between player and enemy
                        if(matchParticles(this, particle, ParticlePlayer, ParticleEnemy)) { // Player-enemy crash - Game Over
                            gameScene()->gameOver();
                        }
                        // Charge exchange
                        double chargediff = fabs(m_charge - particle->m_charge); // charge difference
                        qreal dechargeRateTotal = fabs(length - particleDistances) * dechargeRate * springConstant / 60.0; // including the spring constant to avoid changing the decharge rate when the spring constant is changed
                        if(matchParticles(this, particle, ParticleSimple, ParticlePlayer) ||
                                matchParticles(this, particle, ParticleSimple, ParticleSimple)) { // do not decharge enemies
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
    qreal localDampingFactor = 0.0;
    if(gameScene()->gameMode() == GameScene::ModeClassic) {
        localDampingFactor = dampingFactor;
    } else if(gameScene()->gameMode() == GameScene::ModeParty) {
        localDampingFactor = dampingFactorParty;
    }
    QVector2D F_d = - localDampingFactor * velocity(); //damping
    F += F_d;
    QVector2D a = F / mass();
    _velocity += a * dt;
    // lets not yet set our position, but save it temporarily. This avoids calculating forces for other particles with wrong positions
    nextPosition = position() + velocity() * dt;

    // restrain edges
    qreal edgeBounceDamp = 0.0;
    if(gameScene()->gameMode() == GameScene::ModeParty && particleType() == ParticleEnemy) {
        edgeBounceDamp = edgeBounceDampPartyEnemy;
    } else {
        edgeBounceDamp = 1.0;
    }

    double minx = gameScene()->gameRectF().left() + size().width() * scale() / 2;
    if(position().x() < minx) {
        setPosition(QVector2D(minx,position().y()));
        _velocity.setX(-_velocity.x() * edgeBounceDamp);
    }
    double miny = gameScene()->gameRectF().top() + size().width() * scale() / 2;
    if(position().y() < miny) {
        setPosition(QVector2D(position().x(),miny));
        _velocity.setY(-_velocity.y() * edgeBounceDamp);
    }
    double maxx = gameScene()->gameRectF().right() - size().width() * scale() / 2;
    if(position().x() > maxx) {
        setPosition(QVector2D(maxx,position().y()));
        _velocity.setX(-_velocity.x() * edgeBounceDamp);
    }
    double maxy = gameScene()->gameRectF().bottom() - size().width() * scale() / 2;
    if(position().y() > maxy) {
        setPosition(QVector2D(position().x(),maxy));
        _velocity.setY(-_velocity.y() * edgeBounceDamp);
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

void Particle::setParticleType(int particleType)
{
    m_particleType = particleType;
    if(gameScene()->gameMode() == GameScene::ModeParty) {
        if(particleType == ParticleSimple) {
            setDueTime(partyDisintegrationSimpleTime);
        } else if (particleType == ParticleSlowMotion ||
                   particleType == ParticleRepellent ||
                   particleType == ParticleTransfer) {
            setDueTime(partyDisintegrationSpecialTime);
        } else if(particleType == ParticleGlowing) {
            setDueTime(partyDisintegrationGlowingTime);
        } else {
            setDueTime(-1);
        }
    } else {
        setDueTime(-1);
    }
    originalDueTime = dueTime();
    // Scaling
    // TODO: Make this a bit prettier by using an array or something
    qreal myScale = 1.0;
    switch(particleType) {
    case ParticlePlayer:
        myScale = playerScale;
        break;
    case ParticleEnemy:
        myScale = enemyScale;
        break;
    case ParticleSimple:
        myScale = simpleScale;
        break;
    case ParticleSlowMotion:
        myScale = slowMotionScale;
        break;
    case ParticleRepellent:
        myScale = repellentScale;
        break;
    case ParticleGlowing:
        myScale = glowingScale;
        break;
    case ParticleTransfer:
        myScale = transferScale;
        break;
    }
    setScale(myScale * globalScale);
    originalScale = scale();
}
