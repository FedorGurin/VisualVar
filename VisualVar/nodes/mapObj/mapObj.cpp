#include "mapObj.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"



extern GeographicLib::Geocentric* earth;

const QString google_sat        = "\\sat\\z";
const QString google_map        = "\\map\\z";
const QString google_land       = "\\land\\z";
const QString google_hyb        = "\\both\\z";
const QString yandex_hyb        = "\\yahyb\\z";
const QString yandex_sat        = "\\yasat\\z";
const QString yandex_map        = "\\yamapng\\z";
const QString bing_sat          = "\\vesatbird_N\\z";
const QString google_sat_earth  = "\\vesat\\z";
const QString nokia_hyb         = "\\ovi_com_Hyb\\z";
const QString nokia_map         = "\\ovi_com_map\\z";


void GeographySysCoord::pixelXYToTileXY(double pixX, double pixY, int &tileX, int &tileY)
{
    tileX = pixX / 256;
    tileY = pixY / 256;
}
void GeographySysCoord::tileXYToPixelXY(int tileX,int tileY,double &pixX,  double &pixY)
{
    pixX = tileX * 256;
    pixY = tileY * 256;
}
static int count = 0;
void GeographySysCoord::setZoomLevel(int z, QRectF rectView)
{
    // увеличение зоны выравнивания группы тайлов
    const int ext_align = 1;

//    count++;
//    if(count>3)
//    {
//        int k =0;
//        z = qBound(1,z,20);

//        int tileX1,tileY1,maxTile = 2<<(z-2);
//        int tileX0,tileY0;

//        // загружаем все элементы попадающие в rectView с небольшим избытком
//        pixelXYToTileXY(rectView.x(),rectView.y(),tileX0,tileY0);
//        pixelXYToTileXY(rectView.x() + rectView.width(),rectView.y() + rectView.height(),tileX1,tileY1);

//        // расширим координаты тайлов и ограничим снизу и сверху
//        tileX0-=ext_align; tileX0 = qMax(tileX0,0);
//        tileY0-=ext_align; tileY0 = qMax(tileY0,0);

//        tileX1+=ext_align; tileX1 = qMin(tileX1,maxTile);
//        tileY1+=ext_align; tileY1 = qMin(tileY1,maxTile);

//        double pixX,  pixY;
//            for(int i = tileX0;i < tileX1;i++)
//            {
//                for(int j = tileY0;j < tileY1;j++)
//                {
//                    // пересчет из координат тайла в координаты сцены
//                    tileXYToPixelXY(i,j,pixX,pixY);

//                    QGraphicsPixmapItem *pixMap = ((*ptrItemMapNew)[k]);
//                    if(currentZoom > z)
//                        pixMap->setScale(pixMap->scale()/2);
//                    else if(currentZoom < z)
//                        pixMap->setScale(pixMap->scale()*2);
//                    pixMap->setPos(pixX,pixY);
//                    k++;
//                }
//            }
//        return;
//    }


    // сохранить заданный масштаб, как текущий
    currentZoom = z;
    // смена указателей на буфер
    QList<QGraphicsPixmapItem *> *ptrItemTemp = ptrItemMapOld;
    ptrItemMapOld = ptrItemMapNew;
    ptrItemMapNew = ptrItemTemp;
    // надо увеличить масштаб уже исходных объектов


    ////////////////////////////////////////////////
    // ограничим диапазон
    z = qBound(1,z,20);

    int tileX1,tileY1,maxTile = 2<<(z-2);
    int tileX0,tileY0;

    // загружаем все элементы попадающие в rectView с небольшим избытком
    pixelXYToTileXY(rectView.x(),rectView.y(),tileX0,tileY0);
    pixelXYToTileXY(rectView.x() + rectView.width(),rectView.y() + rectView.height(),tileX1,tileY1);

    // расширим координаты тайлов и ограничим снизу и сверху
    tileX0-=ext_align; tileX0 = qMax(tileX0,0);
    tileY0-=ext_align; tileY0 = qMax(tileY0,0);

    tileX1+=ext_align; tileX1 = qMin(tileX1,maxTile);
    tileY1+=ext_align; tileY1 = qMin(tileY1,maxTile);

    // строки для идентификации карты
    QString pref_map = "";
    QString ext_map  = ".jpg";
    switch(typeMap)
    {
        case YANDEX_SAT:        {pref_map = yandex_sat;               break;}
        case YANDEX_MAP:        {pref_map = yandex_map;ext_map=".png";break;}
        case GOOGLE_MAP:        {pref_map = google_map;ext_map=".png";break;}
        case GOOGLE_LAND:       {pref_map = google_land;              break;}
        case GOOGLE_SAT:        {pref_map = google_sat;              break;}
        case BING_SAT:          {pref_map = bing_sat;                 break;}
        case GOOGLE_SAT_EARTH:  {pref_map = google_sat_earth;         break;}
        case NOKIA_MAP:         {pref_map = nokia_map;ext_map=".png"; break;}
        case NOKIA_HYB:         {pref_map = nokia_hyb;ext_map=".png"; break;}
        default:                {pref_map = "";}
    };

    // для дополнительных слоев
    // строки для идентификации карты
    QString pref_layer  = "";
    QString ext_layer   = ".jpg";
    switch(typeLayer)
    {
        case YANDEX_HYB:        {pref_layer = yandex_hyb;ext_layer=".png";break;}
        case GOOGLE_HYB:        {pref_layer = google_hyb;ext_layer=".png";break;}
        default:                {pref_layer = "";}
    };
    QString path_map        = dirMaps + pref_map    + QString::number(currentZoom);
    QString path_layer      = dirMaps + pref_layer  + QString::number(currentZoom);
    if(pref_layer.isEmpty() == true)
        path_layer = "";

    // запускаем поток для загрузки тайлов
    threadLoadMaps->startLoadTile(path_map,
                                  ext_map,
                                  path_layer,
                                  ext_layer,
                                  tileX0,
                                  tileX1,
                                  tileY0,
                                  tileY1);

}
void GeographySysCoord::setTypeMap(QString value)
{
    //////////// загрузка карты//////////////////////////////////////
    if(value=="yand_sat")
        setTypeMap(GeographySysCoord::YANDEX_SAT);
    else if(value=="yand_sat")
        setTypeMap(GeographySysCoord::YANDEX_SAT);
    else if(value=="yand_map")
        setTypeMap(GeographySysCoord::YANDEX_MAP);
    else if(value=="google_map")
        setTypeMap(GeographySysCoord::GOOGLE_MAP);
    else if(value=="google_land")
        setTypeMap(GeographySysCoord::GOOGLE_LAND);
    else if(value=="google_sat")
        setTypeMap(GeographySysCoord::GOOGLE_SAT);
    else if(value=="bing_sat")
        setTypeMap(GeographySysCoord::BING_SAT);
    else if(value=="google_sat_ea")
        setTypeMap(GeographySysCoord::GOOGLE_SAT_EARTH);
    else if(value=="nokia_hyb")
        setTypeMap(GeographySysCoord::NOKIA_HYB);
    else if(value=="nokia_map")
        setTypeMap(GeographySysCoord::NOKIA_MAP);
    else
        setTypeMap(GeographySysCoord::NO_MAP);
    ///////////////////////////////////////////////////////////////////
}
void GeographySysCoord::setTypeLayer(QString value)
{
    ///////////загрузка слоя
    if(value == "google_hyb")
        setTypeLayer(GeographySysCoord::GOOGLE_HYB);
    else if(value == "yand_hyb")
        setTypeLayer(GeographySysCoord::YANDEX_HYB);
    else
        setTypeLayer(GeographySysCoord::NO_LAYER);
    ///////////////////////////////////////////////////////////////////
}
QString GeographySysCoord::strTypeMap()
{
    QString strMap = "";
    switch(isTypeMap())
    {
    case GeographySysCoord::YANDEX_SAT:         {strMap="yand_sat";     break;}
    case GeographySysCoord::YANDEX_MAP:         {strMap="yand_map";     break;}
    case GeographySysCoord::GOOGLE_MAP:         {strMap="google_map";   break;}
    case GeographySysCoord::GOOGLE_LAND:        {strMap="google_land";  break;}
    case GeographySysCoord::GOOGLE_SAT:         {strMap="google_sat";   break;}
    case GeographySysCoord::BING_SAT:           {strMap="bing_sat";     break;}
    case GeographySysCoord::GOOGLE_SAT_EARTH:   {strMap="google_sat_ea";break;}
    case GeographySysCoord::NOKIA_HYB:          {strMap="nokia_hyb";    break;}
    case GeographySysCoord::NOKIA_MAP:          {strMap="nokia_map";    break;}
    default:                                    {strMap="";}
    };
    return strMap;
}
QString GeographySysCoord::strTypeLayer()
{
    QString strLayer="";
    switch(isTypeLayer())
    {
    case GeographySysCoord::GOOGLE_HYB: {strLayer="google_hyb";break;}
    case GeographySysCoord::YANDEX_HYB: {strLayer="yand_hyb";break;}
    default:                            {strLayer="";}
    };

    return strLayer;
}
ThreadLoadMaps::ThreadLoadMaps(QObject* parent):QThread(parent)
{

}

void ThreadLoadMaps::run()
{
    int pixX,pixY;
    for(int i = tileX0;i < tileX1;i++)
    {
        for(int j = tileY0;j < tileY1;j++)
        {
            // пересчет из координат тайла в координаты сцены
            tileXYToPixelXY(i,j,pixX,pixY);

            // добавление тайлов с слоя карт
            if(path_map.isEmpty() == false)
                addTile(path_map,i,j,pixX,pixY,ext_map);

            // добавление тайлов с дополнительного слоя
            if(path_layer.isEmpty() == false)
                addTile(path_layer,i,j,pixX,pixY,ext_layer);
        }
    }
    // сигнал о полной загруке всех тайлов
    emit finishedLoadAllTile();
}
void ThreadLoadMaps::startLoadTile(QString path_map_,
                                   QString ext_map_,
                                   QString path_layer_,
                                   QString ext_layer_,
                                   int tileX0_,
                                   int tileX1_,
                                   int tileY0_,
                                   int tileY1_)
{

    if(isRunning() == false)
    {
        path_map    = path_map_;
        ext_map     = ext_map_;
        path_layer  = path_layer_;
        ext_layer   = ext_layer_;
        tileX0      = tileX0_;
        tileX1      = tileX1_;
        tileY0      = tileY0_;
        tileY1      = tileY1_;

        start(QThread::LowestPriority);
    }
}

void ThreadLoadMaps::addTile(QString path,
                             int i,
                             int j,
                             double pixX,
                             double pixY,
                             QString ext)
{
    QString fileName=path+"\\"+
            QString::number(i/1024)+"\\x"+
            QString::number(i)+"\\"+
            QString::number(j/1024)+"\\y"+
            QString::number(j)+ext;

    if(QFile::exists(fileName)==false)
        fileName = ":/png/no_tile";

    QFile file(fileName);
    bool openFile = file.open(QIODevice::ReadOnly);
    QByteArray byteArray;
    if(openFile == true)
        byteArray = file.readAll();
    else
        byteArray.clear();

    // отправляем сигнал о готовносте изображения
    emit createNewPixmapItem(byteArray,pixX,pixY);
}
void GeographySysCoord::slotFinishedLoadAllTile()
{
    // обнулим все элементы
    for(auto i:*ptrItemMapOld)
    {
        this->scene()->removeItem(i);
        delete i;
    }
    ptrItemMapOld->clear();
}
void GeographySysCoord::slotCreatePixmapItem(QByteArray byteArray,int pixX,int pixY)
{
    QPixmap p;
    p.loadFromData(byteArray);
    QGraphicsPixmapItem *itemPixmap = new QGraphicsPixmapItem(p,this);//,this->parent);

    itemPixmap->setPos(pixX,pixY);
    addItemToScene(itemPixmap);
}
void GeographySysCoord::addTile(QString prefix,int i,int j,double pixX,double pixY,QString ext)
{
    QString fileName = dirMaps+prefix+QString::number(currentZoom)+"/"+QString::number(i/1024)+"/x"+QString::number(i)+"/"+QString::number(j/1024)+"/y"+QString::number(j)+ext;

    if(QFile::exists(fileName) == false)
        fileName=":/png/no_tile";

    QPixmap pix(fileName);
    QGraphicsPixmapItem *itemPixmap = new QGraphicsPixmapItem(pix,this);

    itemPixmap->setPos(pixX,pixY);
    addItemToScene(itemPixmap);
}

