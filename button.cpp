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
        if(gameScene()->remainingPositiveCharges > 0) {
            painter->drawImage(realsize(),gameScene()->positiveImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    } else {
        if(gameScene()->remainingNegativeCharges > 0) {
            painter->drawImage(realsize(),gameScene()->negativeImage);
        } else {
            painter->drawImage(realsize(),gameScene()->neutralImage);
        }
    }
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    emit clicked();
}
