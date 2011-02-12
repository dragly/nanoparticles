#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QGraphicsObject>
#include "gamescene.h"

class GameObject : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GameObject();
    QRectF realsize();
    GameScene *gameScene();
    QRectF size() const {return _size;}
    void setPosition(QVector2D position);
    QVector2D position() {return _position;}

    QRectF boundingRect() const;

signals:

public slots:

protected:
    const QRectF _size;
    QVector2D _position;
};

#endif // GAMEOBJECT_H