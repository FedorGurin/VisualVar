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
        setScale(0.1);
    }
    virtual int type() const
    {
        return E_BEACON;
    }
    virtual void connectToObj(ObjectGraphNode *obj){}
};
