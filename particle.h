#ifndef PARTICLE_H
#define PARTICLE_H
#include <QGraphicsItem>
#include <QVector2D>
#include "gameobject.h"
#include "gamescene.h"

class Particle : public GameObject
{
public:
    Particle();

    enum { Type = UserType + 1 };
    enum { ParticleSimple, ParticlePlayer, ParticleEnemy };

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);
    void setCharge(float charge) {this->charge = charge; originalCharge = charge;}
    void setVelocity(QVector2D velocity) {this->_velocity = velocity;}
    QVector2D velocity() {return _velocity;}

    int particleType() {return _particleType; }
    void setParticleType(int particleType) { this->_particleType = particleType; }

    bool sticky() {return _sticky;}
    void setSticky(bool sticky) {
        this->_sticky = sticky;
    }

    QRectF boundingRect() const;

protected:
    void advance(int step);

private:
    double charge;
    QVector2D _velocity;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    bool _sticky;
    int _particleType;
    qreal originalCharge;

};

#endif // PARTICLE_H
