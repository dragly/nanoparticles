#include "gameobject.h"
#include "gamescene.h"

GameObject::GameObject(GameScene *gameScene) :
    QGraphicsObject(), _size(-1,-1,1,1), _position(0,0)
{
    setGameScene(gameScene);
}

QRectF GameObject::realsize(bool useSmallest) const {
    GameScene* _gameScene = (GameScene*)scene();
    QRectF _size = QRectF(-1,-1,1,1);
    double width;
    double height;
    width = _gameScene->toFp(_size.width(),useSmallest);
    height = _gameScene->toFp(_size.height(),useSmallest);

    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

void GameObject::setPosition(QVector2D position) {
    this->_position = position;
    this->nextPosition = position;
    if(gameScene()) {
        setPos(gameScene()->toFp(position.x()),gameScene()->toFp(position.y()));
    } else {
        qWarning() << "GameObject::setPosition: Cannot set position without scene.";
    }
}
