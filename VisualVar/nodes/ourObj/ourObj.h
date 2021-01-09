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
#include "formAircraft.h"

//! класс с вертолета
class AircraftObject:public ObjectGraphNode
{
    Q_OBJECT
public:
    //! обычный конструктор
    AircraftObject(QString nameI,
                   QString nameFile,
                   QGraphicsItem *parent);
    //! создание объекта в соответствии с переданным объектом(операция клонирования)
    AircraftObject(AircraftObject *aircraft_,
                   QString nameFile,
                   QGraphicsItem *parent);
    //! тип объекта
    virtual int type() const
    {
        return m_type;//AIRCRAFT;
    }
    void setType(TypeGraphNode t)
    {
        m_type = t;
    }
    //! сохранить свои параметры в файл
    virtual void saveXML(QDomDocument &domDocument,QDomElement &ele);
    //! сохранить данные в xml понятный VxWorks
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele);
    //! формирование запросов
    void getRequest(QString prefix,TCommonRequest *request);
    //! загрузка данных из файла XML
    void loadXML(QDomElement node);
    //! вращение объекта
    virtual void isRotated()
    {
        formSetting->setV(vc);
        formSetting->setH(y);
        //formSetting->setPsi(psi);
        formSetting->setTeta(teta);
        emit isModifyPsi();
        ObjectGraphNode::isRotated();
    }
    //! задать широту
    void setLat(double value)
    {
        lat=value;
        formSetting->slotLatDouble(lat);
    }
    void moveObjToLatLon(){}
    //! задать долготу
    void setLon(double value)
    {
        lon=value;
        formSetting->slotLonDouble(lon);
    }
    virtual void connectToObj(ObjectGraphNode* ){}
    //! текущие значения паарметров
    double currentV()       {return vc          ;}
    double currentY()       {return y           ;}
    double currentTeta()    {return teta        ;}

    double currentVy()      {return vy          ;}
    bool currentStart()     {return startEarth  ;}
    bool currentPrVy()      {return prVy        ;}

    //! текущие ед. измерения порядок числа
    QString curMessV();
    QString curExpV();

    QString curMessY();
    QString curExpY();

    QString curMessPsi();
    QString curExpPsi();

    QString curMessTeta();
    QString curExpTeta();

    QString curMessVy();
    QString curExpVy();

    QString curMessDelta_hc();
    QString curExpDelta_hc();

    QString curMessAlfa_c();
    QString curExpAlfa_c();

    double x_ust(){return x_ut;}
    double z_ust(){return z_ut;}
    void setX_ust(double t){x_ut = t;}
    void setZ_ust(double t){z_ut = t;}

    //! задать текущие ед. измерения и порядок числа
    void setCurMessV(QString value);
    //! задать текущий порядок
    //void setCurExpV(QString value){}

    void setCurMessY(QString value);
    //void setCurExpY(QString value){}

    void setCurMessPsi(QString value);
    //void setCurExpPsi(QString value){}

    void setCurMessTeta(QString value);
    //void setCurExpTeta(QString value){}

    void setCurMessVy(QString value);
    //void setCurExpVy(QString value){}

    void setCurMessDelta_hc(QString value);
    //void setCurExpDelta_hc(QString value){}

    void setCurMessAlfa_c(QString value);
    //void setCurExpAlfa_c(QString value){}

    //! объекты для преобразования чисел
    TObjectUnit *currentUnitTransV;
    TObjectUnit *currentExpTransV;

    TObjectUnit *currentUnitTransY;
    TObjectUnit *currentExpTransY;

    TObjectUnit *currentUnitTransTeta;
    TObjectUnit *currentExpTransTeta;

    TObjectUnit *currentUnitTransVy;
    TObjectUnit *currentExpTransVy;

    TObjectUnit *currentUnitTransDelta_hc;
    TObjectUnit *currentExpTransDelta_hc;

    TObjectUnit *currentUnitTransAlfa_c;
    TObjectUnit *currentExpTransAlfa_c;

    void initMessureItem()
    {
        qDebug("start NodeVisual.h: initMessureItem()\n");
        currentUnitTransV    = unitSpeed->find("m/s");
        currentExpTransV     = unitExp->find("");

        currentUnitTransY    = unitLength->find("m");
        currentExpTransY     = unitExp->find("");

        currentUnitTransPsi  = unitAngle->find("deg");
        currentExpTransPsi   = unitExp->find("");

        currentUnitTransTeta = unitAngle->find("deg");
        currentExpTransTeta  = unitExp->find("");

        currentUnitTransVy   = unitSpeed->find("m/s");
        currentExpTransVy    = unitExp->find("");

        currentUnitTransDelta_hc = unitLength->find("m");
        currentExpTransDelta_hc  = unitExp->find("");

        currentUnitTransAlfa_c   = unitAngle->find("deg");
        currentExpTransAlfa_c    = unitExp->find("");
        qDebug("end NodeVisual.h: initMessureItem()\n");
    }    
signals:
    void isModifyPsi(void);
    void isModifyPosition(QPointF,TGeoPoint);
    void sigHoverEnterEvent(bool);
    void sigDoubleClick();
    void signalReleaseAircraft();
public slots:
    void slotTetaUnt(bool);
    void setVc(double value)
    {
        vc=value;

//        TObjectUnit* newUnitTrans=unitSpeed->find("m/s");
//        vc=unitSpeed->convert(value,newUnitTrans,currentUnitTransV);

        QRectF rect=itemSvg->boundingRect();
        lineFSpeed.setLine(rect.width()+20,
                           rect.height(),
                           rect.width()+20,rect.height()*(1-vc/(formSetting->maxV()-formSetting->minV())));
        lineSpeed->setLine(lineFSpeed);
    }
    virtual void setPsi(double value)
    {
        psi=value;
        itemSvg->setRotation(unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    }
    void setTeta(double value)
    {
        teta=value;
        formSetting->setTeta(teta);
    }

    void setY(double value)
    {
        y=value;
        formSetting->setH(y);
        colorItem->setRowText(tr("Высота = ")+QString::number(formSetting->currentH())+" "+
                            formSetting->currentMessureAlt(),1);
    }
    void setStartEarth(bool value)
    {
        startEarth = value;
    }
    void slotLonToX(double,QString);
    void slotLatToZ(double,QString);
    void slotLonToX(double);
    void slotLatToZ(double);
    void slotLonLatToXZ(double lat,double lon);

    void setVy(double f)
    {
        vy=f;
        formSetting->setVy(vy);
    }
    void setPrVy(bool f)
    {
        prVy=f;

        if(prVy==false) colorItem->setRowText(tr("УНТ=")+
                                                  QString::number(formSetting->currentTeta())+" "+
                                                  formSetting->currentMessureTeta(),2);
        else colorItem->setRowText(tr("Vy=")+
                                    QString::number(formSetting->currentVy())+" "+
                                    formSetting->currentMessureVy(),2);
    }

    bool isStartEarth()
    {
        return startEarth;
    }
    void refresh();
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*          event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent*           event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*          event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*          event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*    event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*        event);
private:
    FormSettingForAircraft *formSetting;
    QGraphicsLineItem *lineSpeed;
    //! подпись скорости
    QGraphicsSimpleTextItem *textValueSpeed;
    QGraphicsSimpleTextItem *textValueH;
    QGraphicsSimpleTextItem *textValueTeta;

    QLineF lineFSpeed;
    double vc;          //! скорость начальная
    double teta;        //! угол тангажа
    double y;           //! высота
    double vy;          //! вертикальная скорость
    bool   prVy;        //! признак, что задаем параметров teta, либо угол тагнажа или вертикальную скорость

    //! признак старта с земли(true - с земли, false - воздух)
    bool startEarth;

    double x_ut = 0.; // координаты относительно начала координат
    double z_ut = 0.; //

    TypeGraphNode m_type = E_HELLICOPTER;
};
