#ifndef PARTICLE_H
#define PARTICLE_H
#include <QGraphicsItem>

class Particle : public QGraphicsItem
{
public:
    Particle();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    QRectF boundingRect() const;
    void setCharge(float charge) {this->charge = charge;}

protected:
    void advance(int step);

private:
    QImage particleImage;
    float charge;
    QPointF velocity;

};

#endif // PARTICLE_H
