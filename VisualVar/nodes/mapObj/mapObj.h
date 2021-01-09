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

#include "nodeVisual.h"

//! поток для подгрузки карт
class ThreadLoadMaps:public QThread
{
    Q_OBJECT
public:
    ThreadLoadMaps(QObject* parent = nullptr);
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
    /*! сигнал о готовности добавления одного тайла
    * \param byteArray - загруженное изображение тайла
    * \param X - координата X тайла относительно сцены
    * \param Y - координата Y тайла относительно сцены
    */
    void createNewPixmapItem(QByteArray byteArray,int X,int Y);
    //! сигнал о том, что все тайлы загружены
    void finishedLoadAllTile();
private:
    GraphNode *graphNode;

    QString path_map;
    QString ext_map;
    QString path_layer;
    QString ext_layer;
    //! координаты прямоугольного тайла
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

    GeographySysCoord(QGraphicsItem *parent = nullptr):GraphNode(parent)
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
    //! указатели на список загруженных тайлов на предыдущем шаге
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

