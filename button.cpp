#include "button.h"
#include "gamescene.h"
#include <QPainter>
#include <QDebug>

Button::Button() :
    GameObject(), _buttonType(ButtonPositive)
{
    setButtonType(StandardButton);
    _selected = false;
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(!gameScene()) {
        qWarning() << " Button::paint(): Cannot paint without gameScene";
        return;
    }
    if(buttonType() == ButtonPositive) {
        if(gameScene()->remainingPositiveCharges > 0) {
            painter->drawImage(realsize(true),gameScene()->positiveImage);
        } else {
            painter->drawImage(realsize(true),gameScene()->neutralImage);
        }
    } else if(buttonType() == ButtonNegative) {
        if(gameScene()->remainingNegativeCharges > 0) {
            painter->drawImage(realsize(true),gameScene()->negativeImage);
        } else {
            painter->drawImage(realsize(true),gameScene()->neutralImage);
        }
    } else {
        painter->drawImage(realsize(true),buttonImage);
    }
    if(_selected) {
        painter->drawImage(realsize(true),gameScene()->selectionImage);
    }
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    emit clicked();
}

void Button::setSelected(bool selected) {
    _selected = selected;
    update();
}

QRectF Button::boundingRect() const {
    return realsize(true);
}
