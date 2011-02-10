#ifndef PARTICLE_H
#define PARTICLE_H
#include <QGraphicsItem>
#include <QVector2D>
#include "gamescene.h"

class Particle : public QGraphicsItem
{
public:
    Particle();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    QRectF boundingRect() const;
    void setCharge(float charge) {this->charge = charge;}
    void setVelocity(QVector2D velocity) {this->_velocity = velocity;}
    QVector2D velocity() {return _velocity;}
    void setPosition(QVector2D position);
    QVector2D position() {return _position;}
    QRectF size() {return _size;}
    QRectF realsize();
    void setSize(QRectF size) {this->_size = size;}

    double fromFp(double number);
    double toFp(double number);

protected:
    void advance(int step);

private:
    double charge;
    QVector2D _velocity;
    QVector2D _position;
    QImage negativeImage;
    QImage positiveImage;
    QImage neutralImage;
    QRectF _size;
    double blah;
    GameScene *gameScene() {return (GameScene*)scene();}

};

#endif // PARTICLE_H
