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
#include "geographyMapping.h"
#include "settingVV.h"



class ColorItem:public QGraphicsRectItem
{
public:
    ColorItem(QGraphicsItem * parent = nullptr );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setRows(unsigned value);
    //! пересчитать размер прямоугольника
    void calcResize();
    void setRowText(QString str,int row);
private:
    QColor color;
    QVector<QGraphicsSimpleTextItem* > lines;
};


//! основной графический узел
class GraphNode:public QGraphicsItem,public QObject
{
public:
    GraphNode(QGraphicsItem *parent = nullptr):QGraphicsItem(parent)
    {
        // координаты положения объекта в географической СК
        lat         = 0;
        lon         = 0;
        use_russian = true;

    }
    enum TypeGraphNode
    {
        E_MAP         = UserType + 1,
        E_OBJ_G       = UserType + 2,
        E_OBJ_V       = UserType + 3,
        E_AERODROM    = UserType + 4,
        E_BEACON_RSBN = UserType + 5,
        E_BEACON_VOR  = UserType + 6,
        E_HELLICOPTER = UserType + 8,
        E_UNSET       = UserType + 7,
        E_SYSCOORD    = UserType + 9,
        E_INFO        = UserType + 10,
        E_ROUTE       = UserType + 11,
        E_POINT       = UserType + 12,
        E_PPM         = UserType + 13,
        E_LABEL       = UserType + 14,
        E_CLOUD       = UserType + 15,
        E_FOG         = UserType + 16
    };

    double lat;
    double lon;
    //! строка для географическх координат
    QString sLat;
    QString sLon;
    //! координаты положения объекта в прямоугольной СК относительно левого края карты
//    double xMap;
//    double zMap;
    virtual void setZoomLevel(int value)
    {
        zoom = value;
    }

    //! уровень детализации на котором отображается объект
    int zoom;

    //! задать имя объекта
    virtual void setName(QString name_)
    {
        name = name_;
    }
    //! имя объекта
    QString name;
    virtual void setDirection(QPointF){}
    //! сохранение параметров в XML файл
    virtual void saveXML(QDomDocument &domDocument,QDomElement &ele)
    {
        Q_UNUSED(domDocument);
        Q_UNUSED(ele);
    }
    //! сохранение параметров в XML файл(формат для модели)
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele)
    {
        Q_UNUSED(domDocument);
        Q_UNUSED(ele);
    }
    virtual int type() const
    {
        return E_UNSET;
    }
    //! признак для выбора единиц измерения
    bool use_russian;
};


//! класс для вращения объекта
class RotateObject:public  QGraphicsPixmapItem
{
public:
    RotateObject(const QString &fileName, QGraphicsItem *parent = nullptr): QGraphicsPixmapItem(fileName, parent)
    {
        graphNode = nullptr;
        QPixmap pic(fileName);
        pic = pic.scaledToWidth(240);
        setPixmap(pic);
        QTransform transRotate;
        transRotate.translate((parent->boundingRect().width()-pic.width())*0.5, -pic.height());
        setTransform(transRotate);
        setFlags(QGraphicsItem::ItemIsSelectable);
        setVisible(false);
    }
    void setGraphNode(GraphNode* tempNode){graphNode=tempNode;}
protected:
//    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//    {
//        QRectF rect(boundingRect());
//        rect.setWidth(rect.width()*200);
//        rect.setHeight(rect.height()*200);
//        renderer()->render(painter, rect);
//        QGraphicsSvgItem::paint(painter, option, widget);
//    }
    virtual void mouseMoveEvent (QGraphicsSceneMouseEvent* event)
    {
        QPointF point = mapToScene(mapToParent(event->pos()));
        if(graphNode != nullptr)
            graphNode->setDirection(point);
//        QGraphicsSvgItem::mouseMoveEvent(event);
        QGraphicsPixmapItem::mouseMoveEvent(event);
    }
    virtual void keyPressEvent(QKeyEvent *event)
    {
        Q_UNUSED(event);
    }
private:
    GraphNode *graphNode;
};

//! основной базовый класс для всех объектов ВЦ, НЦ и т.д
class ObjectGraphNode:public GraphNode
{
    Q_OBJECT
public:
    ObjectGraphNode(ObjectGraphNode* clone,QGraphicsItem *parent = nullptr);
    ObjectGraphNode(QString fileName_,QGraphicsItem *parent = nullptr);
    //! задать имя
    virtual void setName(QString name_)
    {
        GraphNode::setName(name_);
        colorName->setRowText(name_,0);
    }
    void setVisibleRotateRect(bool value)
    {
        rotate->setVisible(value);
    }
    //! обновить траекторию
    void refreshTrajectory(int zoom);
    //!
    virtual void setPsi(double value)
    {
        psi=value;
    }
    //! признак того, что объект может перемещаться по сцене
    void setMovingObject(bool move)
    {
        moving = move;
        setFlag(QGraphicsItem::ItemIsMovable,false);
        setFlag(QGraphicsItem::ItemIsSelectable,false);
    }
    bool isMoving(){return moving;}

    virtual void setDirection(QPointF pos /*координаты в СК сцены*/)
    {
        QPointF centerRotate = mapToScene(itemSvg->transformOriginPoint());
        QPointF point = pos - centerRotate;
        double alfa = check360(atan2(point.x(),-point.y())*180.0/M_PI);
        itemSvg->setRotation(alfa);

        setPsi(unitAngle->convert(itemSvg->rotation(),"deg",currentUnitTransPsi));
        //setPsi(unitAngle->convert(itemSvg->rotation(),"deg","deg"));
        //setPsi(itemSvg->rotation());
        isRotated();
    }

    static void initObjectsProperty()
    {
        if(unitAngle    == nullptr)    unitAngle    = new TObjectProperty("./xml/factors.xml","Angle");
        if(unitSpeed    == nullptr)    unitSpeed    = new TObjectProperty("./xml/factors.xml","Velocity, Speed");
        if(unitLength   == nullptr)    unitLength   = new TObjectProperty("./xml/factors.xml","Length");
        if(unitExp      == nullptr)    unitExp      = new TObjectProperty("./xml/factors.xml","Multipliers");
    }

    static TObjectProperty *unitAngle;
    static TObjectProperty *unitSpeed;
    static TObjectProperty *unitLength;
    static TObjectProperty *unitExp;
    virtual QRectF boundingRect() const
    {
        return itemSvg->boundingRect();
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    QPointF posC()
    {
        return mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    }
    virtual void setPosC(QPointF &point)
    {
        QPointF tempPoint = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
        QPointF dP = tempPoint-pos();

        setPos(point - dP);
    }
    //! задание новой позиции граф. элемента
    virtual void setPosC(qreal dx,qreal dy);

    virtual void isRotated()
    {
    }
    virtual void setEnable(bool f)
    {
        enable = f;
    }
    virtual bool isEnable()
    {
        return enable;
    }
    virtual void setAllInfo(bool value)
    {
        allInfo = value;
        colorItem->setVisible(value);
    }
    virtual void updateDToAircraft()//для целей
    {
    }

    bool isAllInfo(void)
    {
        return allInfo;
    }

    double currentPsi(){return psi;}


    virtual void connectToObj(ObjectGraphNode *obj) = 0;

    //! показать траекторию
    void setVisibleTraj(bool value);
    //! текущее время
    void setTime(double);
    //! удалить траекторию
    void clearTraj();
//private:
    //! объект трансформации
    QTransform transItem;
    QTransform transName;
    //! хранение траектории в виде геоточек(независимо от уровня детализации)
    QList<TGeoPoint> trajGeoPoints;
    //! хранение траектории в виде линии для заданного уровня детализации
    //! при смене уровня детализация нужно пересчитать эти линии
    QVector<QGraphicsLineItem* > traj;
    //! имя файла
    QString fileName;

    //! бинарные метаданные
    QByteArray binMetaData;
    //! объект для преобразования курса
    TObjectUnit *currentUnitTransPsi;
    TObjectUnit *currentExpTransPsi;
    //! базовый объект
    GraphNode *map;
    //! указатель на элемент с картнкой для данного элемента
    QGraphicsSvgItem *itemSvg;
    //! подпись к данному элементу
    QGraphicsSimpleTextItem *nameItem;
    //! цветной прямоугольник
    ColorItem *colorItem;
    //! название
    ColorItem *colorName;
    //! цвет траектории
    Qt::GlobalColor colorTraj;
    //! элемент для вращения объекта
    RotateObject *rotate;
    //! признак того, что объект может двигаться
    bool moving;
    //! разрешение на отрисовку траектории
    bool trajectory;
    //! курс объекта(градусы)
    double psi;
    //! индекс(номер цели)
    int index;
    //! текущее время
    double currentTime;
    //! прошедшая отметка времени для отрисовки траектории
    double lastTimeForTraj;
    //! указать номер цели
    void setIndex(int index_)
    {
        index=index_;
    }
private:
    //! разрешение объекта для отрисовки
    bool enable;
    //! признак того, что объект должен отображать максимум информации
    bool allInfo;
};



class SystemCoordObject:public ObjectGraphNode
{
public:
    SystemCoordObject(QString name,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {
        QRectF rect=itemSvg->boundingRect();
        itemSvg->setTransformOriginPoint(QPointF(0.0,rect.height()));
    }
    virtual int type() const
    {
        return E_SYSCOORD;
    }
};




class RulerObject:public ObjectGraphNode
{
public:
    RulerObject(QString nameFile,QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
    {
        setVisibleRotateRect(false);
    }
private:

};

class LabelObject:public ObjectGraphNode
{
public:
    LabelObject(QString name_,QString nameFile,QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
    {
        name=name_;
        setZValue(9);
        QRectF rect=itemSvg->boundingRect();
        setVisibleRotateRect(false);
        setScale(0.5);

        transItem.translate(0.0,rect.height()+10);
        transItem.scale(2.0,2.0);
        colorItem->setTransform(transItem);
        colorItem->setRows(1);
        colorItem->setRowText(name,0);

        setFlag(QGraphicsItem::ItemIsMovable,false);
        itemSvg->setFlag(QGraphicsItem::ItemIsMovable,false);

        setAcceptHoverEvents(true);

//        numIndex=num;

//        lineToAircraft=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
//        colorItemD=new ColorItem(lineToAircraft);
//        colorItemD->setScale(10);
//        colorItemD->setRows(1);
//        colorItemD->setRowText("d="+QString::number(d),0);

//        QPen penA(Qt::SolidLine);
//        penA.setColor(Qt::gray);
//        lineToAircraft->setPen(penA);

//        formSetting=new FormSettingAerodrom();
//        formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    }


    virtual void setPosC(QPointF &point)
    {
        QPointF tempPoint=mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
        QPointF dP=tempPoint-pos();
        dP.setY(2*dP.y());
        setPos(point-dP);
    }

    virtual void setPosC(qreal dx,qreal dy)
    {
        QPointF point(dx,dy);

        QPointF tempPoint=mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
        QPointF dP=tempPoint-pos();
        dP.setY(2*dP.y());
        setPos(point-dP);
    }
    void setLat(double value)
    {
        lat=value;
    }
    void setLon(double value)
    {
        lon=value;
    }
       virtual void connectToObj(ObjectGraphNode *obj) {}
public:
    virtual int type() const
    {
        return E_LABEL;
    }
private:
    QString name;
    QGraphicsSimpleTextItem* textItem;
};

//! класс информационного объекта
class InfoObject:public ObjectGraphNode
{
public:
    InfoObject(QString name,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {
        setScale(0.06);
    }
    virtual void connectToObj(ObjectGraphNode *obj) {}
    virtual int type() const
    {
        return E_INFO;
    }
};
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
        return E_BEACON_RSBN;
    }
    virtual void connectToObj(ObjectGraphNode *obj){}
};
class RouteObject:public ObjectGraphNode
{
public:
    RouteObject(QString name,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {
        d=-1;
        setZValue(9);
        setVisibleRotateRect(false);
        //! значит маршрута нет, существует только одна точка
        routeLeft=0;
        routeRight=0;

        setScale(0.5);

        lineToRight=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
        lineToRight->setVisible(false);

        colorItemD=new ColorItem(lineToRight);
        //colorItem->translate(0.0,rect.height()+10);
        colorItemD->setScale(2);
        colorItemD->setRows(1);
        colorItemD->setRowText("d="+QString::number(d),0);

        QRectF rect=itemSvg->boundingRect();
        colorItemSumD=new ColorItem(this);
        transItem.translate(0.0,rect.height()+10);
        transItem.scale(2.0,2.0);
        colorItem->setTransform(transItem);

        colorItemSumD->setRows(1);
        colorItemSumD->setRowText("Общее расстояние: "+QString::number(d)+tr(" метров"),0);
        colorItemSumD->setVisible(false);

        refresh();
    }
       virtual void connectToObj(ObjectGraphNode *obj) {}
    virtual void setZoomLevel(int value)
    {
        ObjectGraphNode::setZoomLevel(value);
        refresh();
    }

    //! маршрут левее
    void setLeftRoute(RouteObject *route)
    {
        routeLeft=route;
        refresh();
    }

    virtual void setPosC(QPointF &point)
    {
        QPointF tempPoint=mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
        QPointF dP=tempPoint-pos();
        dP.setY(2*dP.y());
        setPos(point-dP);
    }

    virtual void setPosC(qreal dx,qreal dy)
    {
        QPointF point(dx,dy);

        QPointF tempPoint=mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
        QPointF dP=tempPoint-pos();
        dP.setY(2*dP.y());
        setPos(point-dP);
    }
    void refresh();
    void refreshLine();
    void calcD();
    //! расчитать длинну всего маршрута
    double calcAllRoute();
    double dist(){return d;}

    //! маршрут правее
    void setRightRoute(RouteObject *route)
    {
        routeRight=route;

        QPen penA(Qt::SolidLine);
        penA.setColor(Qt::yellow);
        penA.setWidth(6);
        lineToRight->setPen(penA);
        lineToRight->setVisible(true);
        colorItemSumD->setVisible(false);
        calcD();
        refreshLine();
    }
    virtual int type() const
    {
        return E_ROUTE;
    }
protected:

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
private:
    RouteObject *routeLeft;
    RouteObject *routeRight;

    QGraphicsLineItem *lineToRight;

    ColorItem *colorItemD;
    ColorItem *colorItemSumD;
    double d;
};
class PPMObject:public ObjectGraphNode
{
public:
    PPMObject(QString name,QGraphicsItem *parent):ObjectGraphNode(name,parent)
    {

    }
    virtual int type() const
    {
        return E_PPM;
    }
};
//! вертикальная шкала для отображение высоты у объектов
class VerticalScale:public QGraphicsRectItem
{
public:
    VerticalScale(const QRect rect,QGraphicsItem *parent=0):QGraphicsRectItem(parent)
    {
        setRect(rect.width()-40,0.0,rect.width()-10,rect.height());
    }

private:

};

//! вертикальная шкала для отображение высоты у объектов
class ScaleLine:public QGraphicsLineItem
{
public:
    ScaleLine(const QRect rect, QGraphicsItem *parent = nullptr) : QGraphicsLineItem(parent)
    {
        verticalDir = false;
        m_length = 100;
        m_margin = 15;
        m_dl = 3;
        text = new QGraphicsTextItem(this);
        line1 = new QGraphicsLineItem(this);
        line2 = new QGraphicsLineItem(this);
        QPen spen(pen()); spen.setWidth(2); setPen(spen);
        line1->setPen(spen);
        line2->setPen(spen);
        QFont font(text->font()); font.setBold(true);
        text->setFont(font);
        updateScaleLine(rect, 100);
    }

    void updateScaleLine(QRectF rect, float zoom)
    {
        setVisibleScaleLine(true);
        if(verticalDir){
            QLineF l(rect.topLeft().x()+m_margin, rect.topLeft().y() + (rect.height()-m_length)*0.5,
                     rect.topLeft().x()+m_margin, rect.topLeft().y() + (rect.height()+m_length)*0.5);
            setLine(l);
            line1->setLine(l.p1().x()-m_dl, l.p1().y(),
                           l.p1().x()+m_dl, l.p1().y());
            line2->setLine(l.p1().x()-m_dl, l.p2().y(),
                           l.p1().x()+m_dl, l.p2().y());
            text->setPos(line().p2().x()+m_dl, line().p2().y()-m_length*0.5-10);
        }
        else{
            QLineF l(rect.bottomRight().x() -m_margin-m_length, rect.bottomRight().y()-m_margin,
                     rect.bottomRight().x() -m_margin,          rect.bottomRight().y()-m_margin);
            setLine(l);
            line1->setLine(l.p1().x(), l.p1().y()-m_dl,
                           l.p1().x(), l.p1().y()+m_dl);
            line2->setLine(l.p2().x(), l.p1().y()-m_dl,
                           l.p2().x(), l.p1().y()+m_dl);
            text->setPos(line().p1().x()+m_length*0.25-10, line().p2().y()-m_dl-20);
        }
        double lat1, lon1, lat2, lon2;
        pixelXYToLatLong(line().p1(),zoom-1,lat1,lon1);
        pixelXYToLatLong(line().p2(),zoom-1,lat2,lon2);
        double dlat = verticalDir ? (lat1 - lat2) : (lon2 - lon1);
        dlat *= 111111;//в метры
        text->setPlainText(QString::number(dlat,'f', 1));
    }
    void setVisibleScaleLine(bool b)
    {
        setVisible(b);
        text->setVisible(b);
        line1->setVisible(b);
        line2->setVisible(b);
    }
private:
    QGraphicsTextItem* text;
    QGraphicsLineItem* line1;
    QGraphicsLineItem* line2;
    int m_length;
    int m_margin;
    int m_dl;

    bool verticalDir;// true - вертикальная шкала слева, false - горизонтальная шкала справа
};

