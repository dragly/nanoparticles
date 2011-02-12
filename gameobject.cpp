#include "gameobject.h"

GameObject::GameObject() :
    QGraphicsObject(), _size(-1,-1,1,1), _position(0,0)
{
}

QRectF GameObject::realsize() {
    QRectF _size = QRectF(-1,-1,1,1);
    double width = gameScene()->toFp(_size.width());
    double height = gameScene()->toFp(_size.height());
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

GameScene* GameObject::gameScene() {
    return (GameScene*)scene();
}

void GameObject::setPosition(QVector2D position) {
     this->_position = position;
     setPos(gameScene()->toFp(position.x()),gameScene()->toFp(position.y()));
}

QRectF GameObject::boundingRect() const {
    QRectF _size = QRectF(-1,-1,1,1);
    double width = (_size.width()) / 100 * scene()->width();
    double height = (_size.height()) / 100 * scene()->width();
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}
