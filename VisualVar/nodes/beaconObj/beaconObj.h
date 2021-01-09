#pragma once

#include <QString>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QGraphicsPixmapItem>
#include <QGenericMatrix>
#include <QPen>
#include <QDomDocument>
#include <QDomElement>
#include <QPainter>
#include <QList>
#include <QApplication>
#include <QGraphicsScene>
#include <QThread>
#include <math.h>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include "nodeVisual.h"

//! класс радиомаяк
class BeaconObject:public ObjectGraphNode
{
public:
    BeaconObject(QString name,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {
        setZValue(10);
        setScale(0.2);
        setAcceptHoverEvents(true);

        //settings = new FormSettingCloud();
        //settings->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);
        QRectF rect1=boundingRect();
        setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
        itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    }

    virtual int type() const
    {
        return E_BEACON;
    }
    virtual void connectToObj(ObjectGraphNode *obj){}
};
