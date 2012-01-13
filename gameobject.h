#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QGraphicsObject>
#include <QVector2D>
class GameScene;

class GameObject : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GameObject(GameScene *gameScene);
    QRectF realsize(bool useSmallest = false) const;
    QRectF size() const {return _size;}
    void setPosition(QVector2D position);
    QVector2D position() {return _position;}
    GameScene *gameScene() {
        return m_gameScene;
    }
    void setGameScene(GameScene *gameScene) {
        m_gameScene = gameScene;
    }

signals:

public slots:

protected:
    const QRectF _size;
    QVector2D _position;
    QVector2D nextPosition;
    GameScene *m_gameScene;
};

#endif // GAMEOBJECT_H
