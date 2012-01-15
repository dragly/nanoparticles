#ifndef PARTICLE_H
#define PARTICLE_H
#include <QGraphicsItem>
#include <QVector2D>
#include "gameobject.h"
#include "gamescene.h"

class Particle : public GameObject
{
public:
    Particle(GameScene *gameScene);

    enum { Type = UserType + 1 };
    enum ParticleType { ParticleSimple,
                        ParticlePlayer,
                        ParticleEnemy,
                        ParticleSlowMotion,
                        ParticleRepellent,
                        ParticleGlowing,
                        ParticleTransfer
                      };

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);
    double charge() {return m_charge; }
    void setCharge(float charge) {this->m_charge = charge; originalCharge = charge;}
    void setVelocity(QVector2D velocity) {this->_velocity = velocity;}
    QVector2D velocity() {return _velocity;}

    int particleType() {return m_particleType; }
    void setParticleType(int particleType);

    bool sticky() {return _sticky;}
    void setSticky(bool sticky) {
        this->_sticky = sticky;
    }

    bool electroSticky() {return _electroSticky;}
    void setElectroSticky(bool electroSticky) {
        this->_electroSticky = electroSticky;
    }

    qreal mass() {return _mass;}
    void setMass(qreal mass) {
        this->_mass = mass;
    }

    QRectF boundingRect() const;

    bool matchParticles(Particle *particle1, Particle *particle2, ParticleType type1, ParticleType type2);
    bool hasCollidedWithPlayer;
    int createdTime() {
        return m_createdTime;
    }
    void setCreatedTime(int createdTime) {
        m_createdTime = createdTime;
    }
    void setDueTime(int dueTime) {
        originalDueTime = dueTime;
        m_dueTime = dueTime;
    }
    int dueTime() {
        return m_dueTime;
    }

protected:
    void advance(int step);

private:
    double m_charge;
    QVector2D _velocity;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    int m_particleType;
    qreal _mass;
    bool _electroSticky;
    bool _sticky;
    qreal originalCharge;
    int m_createdTime;
    int m_dueTime;
    int originalDueTime;
    int originalScale;

};

#endif // PARTICLE_H
