#include "button.h"
#include "gamescene.h"
#include <QPainter>
#include <QDebug>

Button::Button() :
        GameObject(), _buttonType(ButtonPositive)
{
    setButtonType(StandardButton);
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(buttonType() == ButtonPositive) {
        if(gameScene()->remainingPositiveCharges > 0) {
            painter->drawImage(realsize(),gameScene()->positiveImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    } else if(buttonType() == ButtonNegative) {
        if(gameScene()->remainingNegativeCharges > 0) {
            painter->drawImage(realsize(),gameScene()->negativeImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    } else {
        painter->drawImage(realsize(),buttonImage);
    }
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    emit clicked();
}
