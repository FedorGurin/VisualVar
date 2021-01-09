#pragma once

#define _USE_MATH_DEFINES

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


#include "formsettingfog.h"
#include "../globalFunc/gl_func.h"
#include "../globalFunc/UnitsMeasure/IUnits.h"
#include "../mppm/CommonEngineData.h"
#include "geographyMapping.h"
#include "settingVV.h"
#include "nodeVisual.h"

//! класс тумана
class FogObject:public ObjectGraphNode
{
     Q_OBJECT
public:
    FogObject(QString name_,QGraphicsItem *parent):ObjectGraphNode(name_,parent)
    {
        setZValue(10);
        setScale(0.1);
        setAcceptHoverEvents(true);

        settings = new FormFog();
        settings->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);
        QRectF rect1 = boundingRect();
        setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
        itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    }
    virtual void connectToObj(ObjectGraphNode *obj){}
signals:
    void isModifyPosition(QPointF,TGeoPoint);
protected:
    virtual void mouseMoveEvent (QGraphicsSceneMouseEvent   *event);
public:
    FormFog *settings;
    virtual int type() const
    {
        return E_FOG;
    }

};

