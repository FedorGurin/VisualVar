#ifndef NODEVISUAL_VAR_H_
#define NODEVISUAL_VAR_H_

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

#include "formsettingforaircraft.h"
#include "formsettingforairtarget.h"
#include "formsettingforgroundtargets.h"
#include "../globalFunc/gl_func.h"
#include "../globalFunc/UnitsMeasure/IUnits.h"
#include "../mppm/CommonEngineData.h"
#include "geographyMapping.h"
#include "formsettingaerodrom.h"
#include "settingVV.h"

namespace VisualVariant
{


class ColorItem:public QGraphicsRectItem
{
public:
    ColorItem(QGraphicsItem * parent = 0 );
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
    GraphNode(QGraphicsItem *parent = 0):QGraphicsItem(parent)
    {
        //! координаты положения объекта в географической СК
        lat         = 0;
        lon         = 0;
        use_russian = true;
        //! координаты положения объекта в прямоугольной СК относительно левого края карты
//        xMap=0;
//        zMap=0;
    }
    enum TypeGraphNode
    {
        E_MAP         = UserType + 1,
        E_OBJ_G       = UserType + 2,
        E_OBJ_V       = UserType + 3,
        E_AERODROM    = UserType + 4,
        E_BEACON_RSBN = UserType + 5,
        E_BEACON_VOR  = UserType + 6,
        E_AIRCRAFT    = UserType + 8,
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
    //! координаты положения объекта в географической СК
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
        zoom=value;
    }

    //! уровень детализации на котором отображается объект
    int zoom;

    //! задать имя объекта
    virtual void setName(QString name_)
    {
        name=name_;
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
//! поток для подгрузки карт
class ThreadLoadMaps:public QThread
{
    Q_OBJECT
public:
    ThreadLoadMaps(QObject* parent=0);
    //! добавить тайл
    void addTile(QString path,
                 int i,
                 int j,
                 double pixX,
                 double pixY,
                 QString ext);
    //! загрузка тайлов
    void startLoadTile(QString pref_map,
                       QString ext_map,
                       QString pref_layer,
                       QString ext_layer,
                       int tileX0,
                       int tileX1,
                       int tileY0,
                       int tileY2);
protected:
    virtual void run();
signals:
    void createNewPixmapItem(QByteArray,int,int);
    //! сигнал о том, что все тайлы загружены
    void finishedLoadAllTile();
private:
    GraphNode *graphNode;

    QString path_map;
    QString ext_map;
    QString path_layer;
    QString ext_layer;
    //! координаты тайла
    int tileX0;
    int tileX1;
    int tileY0;
    int tileY1;
};

//! географическая система координат
class GeographySysCoord:public GraphNode
{
    Q_OBJECT
public:

    enum TypeMAP
    {
        NO_MAP,
        GOOGLE_SAT,
        GOOGLE_MAP,
        GOOGLE_LAND,
        YANDEX_SAT,
        YANDEX_MAP,
        BING_SAT,
        GOOGLE_SAT_EARTH,
        NOKIA_MAP,
        NOKIA_HYB
    };
    enum TypeLayer
    {
        NO_LAYER,
        GOOGLE_HYB,
        YANDEX_HYB
    };

    GeographySysCoord(QGraphicsItem *parent=0):GraphNode(parent)
    {
        itemMapNew.clear();
        itemMapOld.clear();
        ptrItemMapNew  = &itemMapNew;
        ptrItemMapOld  = &itemMapOld;
        currentZoom    = 3;
        typeMap        = NO_MAP;
        typeLayer      = NO_LAYER;
        dirMaps        = readParamFromXMLFile(nameMainFile,"VV","DirWithMap");
        threadLoadMaps = new ThreadLoadMaps(this);
        connect(threadLoadMaps,
                SIGNAL(createNewPixmapItem(QByteArray,int,int)),this,
                SLOT(slotCreatePixmapItem(QByteArray,int,int)));

        connect(threadLoadMaps,
                SIGNAL(finishedLoadAllTile()),this,
                SLOT(slotFinishedLoadAllTile()));
    }

    void addTile(QString prefix,
                 int i,
                 int j,
                 double pixX,
                 double pixY,
                 QString ext=".jpg");

    void setTypeMap(TypeMAP value)
    {
        typeMap = value;
    }
    void setTypeMap(QString value);
    void setTypeLayer(TypeLayer value)
    {
        typeLayer = value;
    }
    void setTypeLayer(QString value);
    virtual QRectF boundingRect() const
    {
        int wh = 2<<(currentZoom-1);
        return QRectF(0.0,0.0,wh*256,wh*256);
    }
    void setZoomLevel(int z, QRectF rectView);
    void pixelXYToTileXY(double pixX, double pixY, int &tileX, int &tileY);
    void tileXYToPixelXY(int tileX,int tileY,double &pixX,  double &pixY);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }
    void addItemToScene(QGraphicsPixmapItem *item)
    {
        item->setZValue(0);
        ptrItemMapNew->push_back(item);
    }
    void addToScene(QGraphicsPixmapItem *item)
    {
        item->setZValue(0);
        ptrItemMapNew->push_back(item);
    }
    virtual int type() const
    {
        return E_MAP;
    }
    TypeMAP     isTypeMap(){return typeMap;}
    QString     strTypeMap();
    TypeLayer   isTypeLayer(){return typeLayer;}
    QString     strTypeLayer();
public slots:
    void slotCreatePixmapItem(QByteArray pixmap,int pixX,int pixY);
    void slotFinishedLoadAllTile();
private:
    //! указатели на список загруженных тайлы
    QList<QGraphicsPixmapItem *> itemMapOld;
    QList<QGraphicsPixmapItem *> itemMapNew;
    QList<QGraphicsPixmapItem *> *ptrItemMapNew;
    QList<QGraphicsPixmapItem *> *ptrItemMapOld;
    //! тип карты
    TypeMAP typeMap;
    //! тип дополнительного слоя
    TypeLayer typeLayer;
    //! текущий масштаб
    int currentZoom;
    //! базовая папка с тайлами
    QString dirMaps;
    //! поток для загрузки тайлов
    ThreadLoadMaps *threadLoadMaps;
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
        if(unitAngle    == nullptr)    unitAngle=new TObjectProperty("./xml/factors.xml","Angle");
        if(unitSpeed    == nullptr)    unitSpeed=new TObjectProperty("./xml/factors.xml","Velocity, Speed");
        if(unitLength   == nullptr)   unitLength=new TObjectProperty("./xml/factors.xml","Length");
        if(unitExp      == nullptr)      unitExp=new TObjectProperty("./xml/factors.xml","Multipliers");
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
    GeographySysCoord *map;
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

    TypeGraphNode m_type = E_AIRCRAFT;
};

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

    virtual void saveXML(QDomDocument &domDocument,QDomElement &ele,bool circleVariant=false);
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele,bool circleVariant=false);
    //! формирование запросов
    void getRequest(QString prefix,TCommonRequest *request,bool circleVariant=false,int numIndex=-1);

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
    double currentNg_z()        {return ng_z;}
    bool   currentPrTypeCode()  {return prCodeLen;}
    double currentLength()      {return length;}
    double curFireTime()        {return fireTime;}
    double curSpeedAfterFire()  {return speedAfterFire;}
    double curHostId()          {return hostId;}
    double curIndFireTar()      {return indexFireTarget;}

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

    void setFireTime(double value)
    {
        fireTime = value;
    }
    void setSpeedAfterFire(double value)
    {
        speedAfterFire = value;
    }
    void setHostId(int value)
    {
        hostId = value;
    }
    void setIndFireTar(int value)
    {
        indexFireTarget = value;
    }

    void setTeta(double value)
    {
        teta=value;
        formSetting->setTeta(teta);
        colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);
    }

    void setNg_z(double ng_z_)
    {
        ng_z=ng_z_;
    }
    void setCode(int c,QString str)
    {
        code_prev = code;
        code=c;
        codeStr=str;
        formSetting->setCode(code);
        formSetting->setPrCodeLength(prCodeLen);
        colorItem->setRowText(tr("Тип объекта=")+codeStr,3);
    }
    void setCode(int c)
    {
        code_prev = code;
        code=c;
        //codeStr=formSetting->typeObject(c);
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
    bool prCodeLen;         // признак задания длины или кода цели
    int code;               // тип объекта(длина объекта)
    double length;          // длина цели
    float ng_z;             // горизонтальная составляющая перегрузки
    int Beg_num_target;//
    double fireTime;        // время отделения, сек
    double speedAfterFire;  //скорость после отделения, м/с
    unsigned hostId;        //число
    unsigned indexFireTarget;//число

    int code_prev;
    QString codeStr;// название объекта полученного по коду
};

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
    virtual void saveXMLForModel(QDomDocument &domDocument,QDomElement &ele,bool circleVariant=false);
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

////! класс облачности
//class CloudObject:public ObjectGraphNode
//{
//     Q_OBJECT
//public:
//    CloudObject(QString name,int num,QGraphicsItem *parent):ObjectGraphNode(name,parent)
//    {
//        setScale(0.1);
//        setAcceptHoverEvents(true);
//        d=0.0;fi=0.0;

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

//    }
// virtual void updateDToAircraft()
//    {
//        //пересчет линии до вертолета
//        if(lineToAircraft->isVisible()){
//            QPointF pointEnd=mapFromItem(itemSvg,itemSvg->transformOriginPoint());
//            QPointF pointStart=mapFromItem(aircraft->itemSvg,aircraft->itemSvg->transformOriginPoint());
//            QLineF line(pointStart,pointEnd);
//            lineToAircraft->setLine(line);
//        }
//    }
//protected:

//    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
//    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
//public:
//    void setAircraft(AircraftObject *air)
//    {
//        aircraft=air;
//        connect(aircraft,SIGNAL(isModifyPsi()),this,SLOT(slotIsModifyPsi()));
//        connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this,SLOT(slotIsModifyPosition()));
//        connect(aircraft,SIGNAL(sigHoverEnterEvent(bool)),this,SLOT(slotEnterLeaveCur(bool)));
//    }
////signals:
//    /*void isModifyPsi(void);
//    void isModifyPosition(QPointF,QPointF);
//    void sigHoverEnterEvent(bool);*/

//    FormSettingAerodrom *formSetting;
//    virtual int type() const
//    {
//        return E_AERODROM;
//    }
//signals:
//     void isModifyPosition(QPointF,TGeoPoint);
//public slots:
//    void slotIsModifyPsi(void);
//    void slotIsModifyPosition();
//    void slotEnterLeaveCur(bool);

//private:
//     AircraftObject *aircraft;

//     QGraphicsLineItem *lineToAircraft;
//     ColorItem *colorItemD;

//     int numIndex;

//     double d;
//     double fi;
//};

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

        formSetting=new FormSettingAerodrom();
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
    virtual void setZoomLevel(int value)
    {
        ObjectGraphNode::setZoomLevel(value);
        refresh();
    }
    void setLat(double value)
    {
        lat=value;
    }
    void setLon(double value)
    {
        lon=value;
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

/*
//! Класс описывающий систему координат
class SystemCoordNode:public GraphNode
{
public:
    SystemCoordNode(GraphNode *parent):GraphNode(parent)
    {
    }
    //! задать углы поворота СК относительно родительской
    double setAngleRotate(double psi,
                          double gamma,
                          double tetta){}

    //! рассчитать результирующую матрицу
    void calcResultMatrix(void){}
    virtual int type() const
    {
        return SYSCOORD;
    }
private:
    QMatrix3x3 matrixOrientResult;

    QMatrix3x3 matrixOrientPsi;
    QMatrix3x3 matrixOrientGamma;
    QMatrix3x3 matrixOrientTetta;


};
//! Класс описывающий систему координат
class MapNode:public GraphNode
{
public:
    MapNode(GraphNode *parent):GraphNode(parent)
    {

    }
    virtual int type() const
    {
        return MAP;
    }
};
class ObjectGraphNode:public GraphNode
{
public:
    ObjectGraphNode(ObjectGraphNode *parent):GraphNode(parent)
    {
        itemSvg=0;
    }
    enum TypeObjectNode
    {
        AIRCRAFT    = UserType+5,
        TARGET      = UserType+6,
        BEACON      = UserType+7,
        OTHER       = UserType+8
    };

    virtual int subType() const
    {
        return OTHER;
    }

    virtual int type() const
    {
        return OBJECT;
    }
    void setItemSvg(QGraphicsSvgItem *item)
    {
        itemSvg=item;
    }
private:
    QGraphicsSvgItem *itemSvg;

};
//! класс с вертолета
class AircraftObject:public ObjectGraphNode
{
public:
    AircraftObject(ObjectGraphNode *parent):ObjectGraphNode(parent)
    {
        QGraphicsSvgItem* itemSvg=new QGraphicsSvgItem("./res/mkm.svg",this);
        setItemSvg(itemSvg);
    }

    virtual int subType() const
    {
        return AIRCRAFT;
    }
};
//! класс цели
class TargetObject:public ObjectGraphNode
{
public:
    TargetObject(ObjectGraphNode *parent):ObjectGraphNode(parent)
    {

    }
    enum TypeTargetObject
    {
        AIR_T    = UserType+9,
        GROUND_T = UserType+10,
        OTHER_T  = UserType+11
    };

    virtual int targetType() const
    {
        return OTHER_T;
    }

};
//! класс воздушная цель
class AirTarget:public TargetObject
{
public:
    AirTarget(TargetObject *parent):TargetObject(parent)
    {
        QGraphicsSvgItem* itemSvg=new QGraphicsSvgItem("./res/target.svg",this);
        setItemSvg(itemSvg);
    }
    virtual int targetType() const
    {
        return AIR_T;
    }

};
//! класс наземная цель
class GroundTarget:public TargetObject
{
public:
    GroundTarget(TargetObject *parent):TargetObject(parent)
    {

    }
    virtual int targetType() const
    {
        return GROUND_T;
    }
};
//! класс маяка
class BeaconObject:public ObjectGraphNode
{
public:
    BeaconObject(ObjectGraphNode *parent):ObjectGraphNode(parent)
    {

    }
    virtual int subType() const
    {
        return BEACON;
    }
};
*/
}
#endif // NODE_H
