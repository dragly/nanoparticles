#include "button.h"
#include "gamescene.h"
#include <QPainter>
#include <QDebug>

Button::Button() :
        GameObject(), _buttonType(ButtonPositive)
{
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(_buttonType == ButtonPositive) {
        painter->drawImage(realsize(),gameScene()->positiveImage);
    } else {
        painter->drawImage(realsize(),gameScene()->negativeImage);
    }
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    emit clicked();
}
