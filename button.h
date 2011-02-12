#ifndef BUTTON_H
#define BUTTON_H

class GameScene;

#include <QGraphicsItem>
#include <QVector2D>
#include "gameobject.h"

class Button : public GameObject
{
    Q_OBJECT
public:
    Button();

    enum { Type = UserType + 2 };

    enum { ButtonNegative, ButtonPositive };

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    int buttonType() {
        return _buttonType;
    }
    void setButtonType(int type) {
        _buttonType = type;
    }

signals:
    void clicked();
public slots:

private:
    GameScene *gameScene() {return (GameScene*)scene();}

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    int _buttonType;
};

#endif // BUTTON_H
