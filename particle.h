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

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);
    void setCharge(float charge) {this->charge = charge;}
    void setVelocity(QVector2D velocity) {this->_velocity = velocity;}
    QVector2D velocity() {return _velocity;}

protected:
    void advance(int step);

private:
    double charge;
    QVector2D _velocity;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;

};

#endif // PARTICLE_H
