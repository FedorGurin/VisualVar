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

#include <QGraphicsScene>
#include <QThread>
#include <math.h>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>

#include "nodeVisual.h"
#include "formsettingforairtarget.h"
#include "nodes/ourObj/ourObj.h"
//! класс воздушная цель
class AirObj:public ObjectGraphNode
{
    Q_OBJECT
public:
    //! обычный конструктор
    AirObj(QString name_,          /* имя объекта*/
                    QString nameFile,       /* имя файла  */
                    QGraphicsItem *parent); /* указатель на родителя*/

    //! для операции клонирования
    AirObj(AirObj *airTarget, /* воздушная цель*/
                    AircraftObject  *aircraft,  /* носителя*/
                    QGraphicsItem   *parent);   /* указатель на родителя*/

    virtual void isRotated()
    {
        formSetting->setV(v);
        formSetting->setH(y);
        formSetting->setPsi(psi);
        formSetting->setFi(fi);
        formSetting->setD(d);
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

    void setAircraft(AircraftObject *air)
    {
        aircraft=air;
        //connect(aircraft,SIGNAL(isModifyPsi()),this,SLOT(slotIsModifyPsi()));
        connect(aircraft,SIGNAL(isModifyPsi()),this,SLOT(slotFi()));
        connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this,SLOT(slotMovePos()));
        //connect(aircraft,SIGNAL(isModifyPosition(QPointF,QPointF)),this,SLOT(slotIsModifyPosition()));
        connect(aircraft,SIGNAL(sigHoverEnterEvent(bool)),this,SLOT(slotEnterLeaveCur(bool)));
    }

    virtual void saveXML(QDomDocument &domDocument,QDomElement &ele);
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele);
    //! формирование запросов
    void getRequest(QString prefix,TCommonRequest *request,int numIndex=-1);

    void loadXML(QDomElement tempNode);

    FormSettingForAirTarget *formSetting;

    void setLat(double value,QString strValue="")
    {
        Q_UNUSED(strValue);
        lat=value;
        formSetting->setLatDouble(lat);
    }

    void setLon(double value,QString strValue="")
    {
        Q_UNUSED(strValue);

        lon=value;
        formSetting->setLonDouble(lon);
    }

    QString currentCodeStr(){return codeStr;}
    int currentCode()    {return code;}
    int prevCode()       {return code_prev;}

    double currentV()           {return v;}
    double currentY()           {return y;}
    double currentD()           {return d;}
    double currentFi()          {return fi;}
    double currentTeta()        {return teta;}

    //! текущие ед. измерения
    QString curMessV();
    QString curMessY();
    QString curMessPsi();
    QString curMessTeta();
    QString curMessFi();
    QString curMessD();
    QString curMessLength();
    QString curMessTime();

    //! задать ед. измерения
    void setCurMessV(QString value);
    void setCurMessY(QString value);
    void setCurMessPsi(QString value);
    void setCurMessTeta(QString value);
    void setCurMessFi(QString value);
    void setCurMessD(QString value);
    void setCurMessLength(QString value);
    //! объекты для преобразования чисел
    TObjectUnit *currentUnitTransFi;
    TObjectUnit *currentUnitTransY;
    TObjectUnit *currentUnitTransD;
    TObjectUnit *currentUnitTransTeta;
    TObjectUnit *currentUnitTransV;
    TObjectUnit *currentUnitTransLength;
    TObjectUnit *currentUnitTransTime;

    void initMessureItem()
    {
        qDebug("start NodeVisual.h: initMessureItem()\n");
        currentUnitTransV       = unitSpeed->find("m/s");
        currentUnitTransY       = unitLength->find("m");
        currentUnitTransPsi     = unitAngle->find("deg");
        currentUnitTransTeta    = unitAngle->find("deg");
        currentUnitTransD       = unitLength->find("m");
        currentUnitTransLength  = unitLength->find("m");
        currentUnitTransFi      = unitAngle->find("deg");
        currentUnitTransTime    = unitAngle->find("sec");
        qDebug("end NodeVisual.h: initMessureItem()\n");
    }
public slots:
    void slotLonToX(double);
    void slotLatToZ(double);

    void setTeta(double value)
    {
        teta=value;
        formSetting->setTeta(teta);
        colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);
    }


    void setCode(int c,QString str)
    {
        code_prev = code;
        code=c;
        codeStr=str;
        formSetting->setCode(code);

        colorItem->setRowText(tr("Тип объекта=")+codeStr,3);
    }
    void setCode(int c)
    {
        code_prev = code;
        code=c;
        //codeStr=formSetting->typeObject(c);
        formSetting->setCode(c);

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
    virtual void setPsi(double value)
    {
        psi=value;
        itemSvg->setRotation(unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    }
    void setFi(double value);

    void setY(double value)
    {
        y=value;
        colorItem->setRowText(tr("Высота=")+QString::number(y),1);
        //textValueH->setText(tr("Высота=")+QString::number(y));
    }
    void setD(double value);
    //! отключение цели
    void setEnable(bool f)
    {
        ObjectGraphNode::setEnable(f);
        lineNotEnable->setVisible(!isEnable());
        ellipseNotEnable->setVisible(!isEnable());
        formSetting->setEnableTarget(isEnable());
    }
    void refresh()
    {
        slotIsModifyPsi();
        slotIsModifyPosition();
    }

    void slotIsModifyPsi();
    void slotIsModifyPosition();
    void slotEnterLeaveCur(bool);
    void slotFi();
    void slotMovePos();

    virtual int type() const{return E_OBJ_V;}
signals:
    void isModifyPosition(QPointF,TGeoPoint);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent   *event);
    virtual void mouseMoveEvent (QGraphicsSceneMouseEvent   *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent   *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent   *event);
private:
    AircraftObject          *aircraft;
    QGraphicsLineItem       *lineSpeed;
    QGraphicsLineItem       *lineToAircraft;
    ColorItem               *colorItemD;
    //! отрисовка перечеркивания при исключении объекта из варианта
    QGraphicsLineItem       *lineNotEnable;
    QGraphicsEllipseItem    *ellipseNotEnable;
    QLineF lineFSpeed;

    double fi;      // угол на цель град(-180,180)
    double d;       // дальность (метры)
    double y;       // высота (метры)
    double v;               // скорость (м/с)
    double teta;            // угол тангажа (град -90,90)
    int code;               // тип объекта(длина объекта)
    int code_prev;
    QString codeStr;// название объекта полученного по коду
};
