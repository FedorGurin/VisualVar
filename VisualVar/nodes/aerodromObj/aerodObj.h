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


#include "../globalFunc/gl_func.h"
#include "../globalFunc/UnitsMeasure/IUnits.h"
#include "../mppm/CommonEngineData.h"
#include "formsettingaerodrom.h"
#include "settingVV.h"
#include "nodeVisual.h"
#include "nodes/ourObj/ourObj.h"


//! класс аэродрома
class AerodromObject:public ObjectGraphNode
{
     Q_OBJECT
public:
    AerodromObject(QString name,int num,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {
        setScale(0.1);
        setAcceptHoverEvents(true);
        d=0.0;fi=0.0;

        numIndex=num;

        lineToAircraft=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
        colorItemD=new ColorItem(lineToAircraft);
        colorItemD->setScale(10);
        colorItemD->setRows(1);
        colorItemD->setRowText("d="+QString::number(d),0);

        QPen penA(Qt::SolidLine);
        penA.setColor(Qt::gray);
        lineToAircraft->setPen(penA);

        formSetting = new FormSettingAerodrom();
        formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    }
 virtual void updateDToAircraft()
    {
        //пересчет линии до вертолета
        if(lineToAircraft->isVisible()){
            QPointF pointEnd=mapFromItem(itemSvg,itemSvg->transformOriginPoint());
            QPointF pointStart=mapFromItem(aircraft->itemSvg,aircraft->itemSvg->transformOriginPoint());
            QLineF line(pointStart,pointEnd);
            lineToAircraft->setLine(line);
        }
    }
       virtual void connectToObj(ObjectGraphNode *obj) {}
protected:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
public:
    void setAircraft(AircraftObject *air)
    {
        aircraft=air;
        connect(aircraft,SIGNAL(isModifyPsi()),this,SLOT(slotIsModifyPsi()));
        connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this,SLOT(slotIsModifyPosition()));
        connect(aircraft,SIGNAL(sigHoverEnterEvent(bool)),this,SLOT(slotEnterLeaveCur(bool)));
    }
//signals:
    /*void isModifyPsi(void);
    void isModifyPosition(QPointF,QPointF);
    void sigHoverEnterEvent(bool);*/

    FormSettingAerodrom *formSetting;
    virtual int type() const
    {
        return E_AERODROM;
    }
signals:
     void isModifyPosition(QPointF,TGeoPoint);
public slots:
    void slotIsModifyPsi(void);
    void slotIsModifyPosition();
    void slotEnterLeaveCur(bool);

private:
     AircraftObject *aircraft;

     QGraphicsLineItem *lineToAircraft;
     ColorItem *colorItemD;

     int numIndex;

     double d;
     double fi;
};

