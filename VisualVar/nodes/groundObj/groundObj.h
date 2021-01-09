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


#include "formsettingforgroundtargets.h"

#include "../globalFunc/gl_func.h"
#include "../globalFunc/UnitsMeasure/IUnits.h"
#include "../mppm/CommonEngineData.h"
#include "geographyMapping.h"

#include "settingVV.h"
#include "nodeVisual.h"
#include "nodes/ourObj/ourObj.h"

//! класс наземная цель
class GroundObj:public ObjectGraphNode
{
     Q_OBJECT
public:
    //! обычный констурктор
    GroundObj(QString name_,
                       QString nameFile,
                       QGraphicsItem *parent);

    //! для операции клонирования
    GroundObj(GroundObj   *groundTarget, /*наземная цель*/
                       AircraftObject       *aircraft,     /*носителя*/
                       QGraphicsItem        *parent);      /*ссылка на родительский элемент*/

    virtual void isRotated()
    {
        formSetting->setV(v);

        formSetting->setPsi(psi);
        ObjectGraphNode::isRotated();
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
    QString currentCodeStr(){return codeStr;}
    int currentCode(){return code;}
    double currentV(){return v;}
    double currentZ(){return z;}
    double currentX(){return x;}
    double currentLength(){return length;}
    bool currentPrCodeLength(){return prCodeLen;}
    double currentD(){return d;}
    double currentSKS(){return sks;}
    //double currentV(){return v;}
    virtual int type() const
    {
        return E_OBJ_G;
    }
    //! формирование запросов
    void getRequest(QString prefix,TCommonRequest *request,int numIndex=-1);

    FormSettingForGroundTargets *formSetting;
    void refresh()
    {
        slotIsModifyPsi();
        slotIsModifyPosition();
    }
    void setLat(double value)
    {
        lat=value;
        formSetting->setLatDouble(lat);
    }
    void setXt(double x_)
    {
        x=x_;
    }
    void setZt(double z_)
    {
        z=z_;
    }
    void setLon(double value)
    {
        lon=value;
        formSetting->setLonDouble(lon);
    }
    void setAircraft(AircraftObject *air)
    {
        aircraft=air;
        connect(aircraft,SIGNAL(isModifyPsi()),this,SLOT(slotIsModifyPsi()));
        connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this,SLOT(slotIsModifyPosition()));
        connect(aircraft,SIGNAL(sigHoverEnterEvent(bool)),this,SLOT(slotEnterLeaveCur(bool)));
    }
    virtual void saveXML(QDomDocument &domDocument,QDomElement &ele);
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele);
    void loadXML(QDomElement tempNode);
signals:
    void isModifyPosition(QPointF,TGeoPoint);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);


public slots:
    void slotIsModifyPsi();
    void slotIsModifyPosition();
    void slotEnterLeaveCur(bool);

public:
    //! текущие едю измерения
    QString curMessV()          {return currentUnitTransV->unicode_id;}
    QString curMessPsi()        {return currentUnitTransPsi->unicode_id;}
    QString curMessD()          {return currentUnitTransD->unicode_id;}
    QString curMessLength()     {return currentUnitTransLength->unicode_id;}
    QString curMessX()          {return currentUnitTransX->unicode_id;}
    QString curMessZ()          {return currentUnitTransZ->unicode_id;}

    void setCurMessV(QString value)
    {
        TObjectUnit* newUnitTrans=unitSpeed->find(value);
        v=unitSpeed->convert(v,currentUnitTransV,newUnitTrans);
        currentUnitTransV=newUnitTrans;
    }
    void setCurMessPsi(QString value)
    {
        TObjectUnit* newUnitTrans=unitAngle->find(value);
        psi=unitAngle->convert(psi,currentUnitTransPsi,newUnitTrans);
        currentUnitTransPsi=newUnitTrans;
    }
    void setCurMessD(QString value)
    {
        TObjectUnit* newUnitTrans=unitLength->find(value);
        d=unitLength->convert(d,currentUnitTransD,newUnitTrans);
        currentUnitTransD=newUnitTrans;
    }
    void setCurMessLength(QString value)
    {
        TObjectUnit* newUnitTrans=unitLength->find(value);
        length=unitLength->convert(length,currentUnitTransLength,newUnitTrans);
        currentUnitTransLength=newUnitTrans;
    }
    void setCurMessX(QString value)
    {
        TObjectUnit* newUnitTrans=unitLength->find(value);
        x=unitLength->convert(x,currentUnitTransX,newUnitTrans);
        currentUnitTransX=newUnitTrans;
    }
    void setCurMessZ(QString value)
    {
        TObjectUnit* newUnitTrans=unitLength->find(value);
        z=unitLength->convert(z,currentUnitTransZ,newUnitTrans);
        currentUnitTransZ=newUnitTrans;
    }
    //! объекты для преобразования чисел
    TObjectUnit *currentUnitTransD;
    //TObjectUnit *currentUnitTransPsi;
    TObjectUnit *currentUnitTransV;
    TObjectUnit *currentUnitTransLength;
    TObjectUnit *currentUnitTransX;
    TObjectUnit *currentUnitTransZ;

    void initMessureItem()
    {
        qDebug("start NodeVisual.h: initMessureItem()\n");
        currentUnitTransV=unitSpeed->find("m/s");
        currentUnitTransPsi=unitAngle->find("deg");
        currentUnitTransD=unitLength->find("m");
        currentUnitTransLength=unitLength->find("m");
        currentUnitTransX=unitLength->find("m");
        currentUnitTransZ=unitLength->find("m");
        qDebug("end NodeVisual.h: initMessureItem()\n");
    }
public slots:
    void slotLonToX(double);
    void slotLatToZ(double);

    void setCode(int c,QString str)
    {
        code=c;
        codeStr=str;
        formSetting->setCode(code);
        colorItem->setRowText(tr("Тип объекта=")+codeStr,1);
        //textCode->setText(tr("Тип=")+str);
    }
    void setCode(int c)
    {
        code=c;
        formSetting->setCode(c);
        formSetting->setPrCodeLength(prCodeLen);
    }
    void setLength(double value)
    {
        length=value;
    }
    void setPrCodeLength(bool value)
    {
        prCodeLen=value;
    }
    void setSKS(int value)
    {
        sks=value;
    }
    void setV(double value)
    {
        v=value;
        QRectF rect=itemSvg->boundingRect();
        lineFSpeed.setLine(rect.width()+20,
                           rect.height(),
                           rect.width()+20,rect.height()*(1-v/(formSetting->maxV()-formSetting->minV()+1))+1);
        lineSpeed->setLine(lineFSpeed);
    }
    void setEnable(bool f)
    {
        ObjectGraphNode::setEnable(f);
        lineNotEnable->setVisible(!isEnable());
        formSetting->setEnableTarget(isEnable());
    }
    virtual void setPsi(double value)
    {
        psi=value;
        itemSvg->setRotation(unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    }

private:
    AircraftObject      *aircraft;
    QGraphicsLineItem   *lineSpeed;
    QGraphicsLineItem   *lineToAircraft;
    ColorItem           *colorItemD;
    QGraphicsLineItem   *lineNotEnable;
    QLineF lineFSpeed;

    double d;
    double v;       // скорость цели
    int sks;        // 1- относительно вертолета, 0 - относительно модельной СК
    double x;       // координата x
    double z;       // координата z
    bool prCodeLen; // признак задания длины или кода цели
    int code;       // тип объекта(длина объекта)
    double length;  // длина цели
    QString codeStr;// имя типа объекта

};

