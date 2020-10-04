#include "nodeVisual.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"
//#define OLD_STEND
namespace VisualVariant
{

TObjectProperty* ObjectGraphNode::unitAngle = nullptr;
TObjectProperty* ObjectGraphNode::unitSpeed = nullptr;
TObjectProperty* ObjectGraphNode::unitLength= nullptr;
TObjectProperty* ObjectGraphNode::unitExp   = nullptr;

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
void GeographySysCoord::setZoomLevel(int z, QRectF rectView)
{
    currentZoom = z;
    QList<QGraphicsPixmapItem *> *ptrItemTemp = ptrItemMapOld;
    ptrItemMapOld = ptrItemMapNew;
    ptrItemMapNew = ptrItemTemp;
    ////////////////////////////////////////////////
    //! ограничим диапазон
    z = qBound(1,z,20);
    //! рассчитываем кол-во элементов в слое
    //int nLayer=2<<(2*(z-1));
    int tileX1,tileY1;
    int tileX0,tileY0;

    //! загружаем все элементы попадающие в rectView
    pixelXYToTileXY(rectView.x(),rectView.y(),tileX0,tileY0);
    tileX0--;tileY0--;
    tileX0 = qMax(tileX0,0);tileY0 = qMax(tileY0,0);

    pixelXYToTileXY(rectView.x() + rectView.width(),rectView.y() + rectView.height(),tileX1,tileY1);
    tileX1++;tileY1++;
    if(tileX1>(2<<(z-2))) tileX1 = 2<<(z-2);
    if(tileY1>(2<<(z-2))) tileY1 = 2<<(z-2);

    //! строки для идентификации карты
    QString pref_map="";
    QString ext_map=".jpg";
    switch(typeMap)
    {
        case YANDEX_SAT:        {pref_map=yandex_sat;               break;}
        case YANDEX_MAP:        {pref_map=yandex_map;ext_map=".png";break;}
        case GOOGLE_MAP:        {pref_map=google_map;ext_map=".png";break;}
        case GOOGLE_LAND:       {pref_map=google_land;              break;}
        case GOOGLE_SAT:        {pref_map=google_sat;              break;}
        case BING_SAT:          {pref_map=bing_sat;                 break;}
        case GOOGLE_SAT_EARTH:  {pref_map=google_sat_earth;         break;}
        case NOKIA_MAP:         {pref_map=nokia_map;ext_map=".png"; break;}
        case NOKIA_HYB:         {pref_map=nokia_hyb;ext_map=".png"; break;}
        default:                {pref_map="";}
    };

    //! для дополнительных слоев
    //! строки для идентификации карты
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

    //! запускаем поток для загрузки тайлов
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
    for(int i=tileX0;i<tileX1;i++)
    {
        for(int j=tileY0;j<tileY1;j++)
        {
            int pixX,pixY;
            tileXYToPixelXY(i,j,pixX,pixY);

            if(path_map.isEmpty()==false)
                addTile(path_map,i,j,pixX,pixY,ext_map);

            if(path_layer.isEmpty()==false)
                addTile(path_layer,i,j,pixX,pixY,ext_layer);
        }
    }
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
    bool openFile=file.open(QIODevice::ReadOnly);
    QByteArray byteArray;
    if(openFile == true)
        byteArray = file.readAll();
    else
        byteArray.clear();

    emit createNewPixmapItem(byteArray,pixX,pixY);
}
void GeographySysCoord::slotFinishedLoadAllTile()
{
    //! обнулим все элементы
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
    QGraphicsPixmapItem *itemPixmap=new QGraphicsPixmapItem(p,this);//,this->parent);

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

ObjectGraphNode::ObjectGraphNode(ObjectGraphNode* clone,QGraphicsItem *parent):GraphNode(parent)
{
    traj.clear();

    psi     = clone->psi;
    index   = 0;
    enable  = clone->enable;
    moving  = clone->moving;
    allInfo = clone->allInfo;
    trajectory=clone->trajectory;
    trajGeoPoints.clear();
    traj.clear();
    currentUnitTransPsi=unitAngle->find("deg");

    itemSvg = new QGraphicsSvgItem(clone->fileName,this);
    QRectF rect=itemSvg->boundingRect();
    itemSvg->setTransformOriginPoint(QPointF(rect.width()/2.0,rect.height()/2.0));

    rotate = new RotateObject(":/png/rotate", itemSvg);
//    rotate = new RotateObject(":/res/svg/rotate", itemSvg);
    rotate->setGraphNode(this);

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    colorItem   = new ColorItem(this);
    colorName   = new ColorItem(this);
    nameItem    = new QGraphicsSimpleTextItem(this);
}

ObjectGraphNode::ObjectGraphNode(QString fileName_,QGraphicsItem *parent):GraphNode(parent)
{
     psi            = 0.0;
     index          = -1;
     currentTime    = 0.0;
     lastTimeForTraj= 0.0;
     colorTraj      = Qt::cyan;
     enable         = true;
     moving         = false;
     allInfo        = false;
     trajectory     = false;
     trajGeoPoints.clear();
     fileName       = fileName_;
     traj.clear();

     currentUnitTransPsi=unitAngle->find("deg");
     itemSvg = new QGraphicsSvgItem(fileName,this);
     QRectF rect = itemSvg->boundingRect();
     itemSvg->setTransformOriginPoint(QPointF(rect.width()/2.0,rect.height()/2.0));

     rotate = new RotateObject(":/png/rotate", itemSvg);
//     rotate = new RotateObject(":/res/svg/rotate", itemSvg);
     rotate->setGraphNode(this);

     setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
     colorItem  = new ColorItem(this);
     colorName  = new ColorItem(this);
     nameItem   = new QGraphicsSimpleTextItem(this);
}
void ObjectGraphNode::refreshTrajectory(int zoom_)
{
    int curX1 = 0,curX2 = 0;
    int curY1 = 0,curY2 = 0;

    //! пересчитаем географические координаты в новые координаты линий
    for(int i = 1;i<trajGeoPoints.size();i++)
    {
        latLongToPixelXY(trajGeoPoints[i].lat,
                         trajGeoPoints[i].lon,
                         zoom_,
                         curX2,
                         curY2);

        latLongToPixelXY(trajGeoPoints[i-1].lat,
                         trajGeoPoints[i-1].lon,
                         zoom_,
                         curX1,
                         curY1);
        //! изменяем координаты линий на новые с учетом детализации
        traj[i]->setLine(curX1,curY1,curX2,curY2);
    }
}
void ObjectGraphNode::setVisibleTraj(bool value)
{
    for(auto i:traj)
    {
        i->setVisible(value);
    }
}
void ObjectGraphNode::setTime(double value)
{
    currentTime = value;
}
void ObjectGraphNode::clearTraj()
{
    trajGeoPoints.clear();
    for(auto i:traj)
    {
        scene()->removeItem(i);
    }
    traj.clear();
    currentTime     = 0;
    lastTimeForTraj = 0;
}
void ObjectGraphNode::setPosC(qreal dx,qreal dy)
{
    static bool addPoint = true;
    QPointF point(dx,dy);

    QPointF tempPoint = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    QPointF dP  =tempPoint - pos();

    //! получаем координаты левого верхнего угла элемента
    QPointF result = point - dP;

    //! перемещаем наш элемент
    setPos(result);

    //! добавить точку в список
    if(trajectory == true && addPoint == true)// && fabs(lastTimeForTraj-currentTime)>1.0)
    {
        if(traj.isEmpty() == false)
        {
            QGraphicsLineItem* tempLine=traj.back();
            QGraphicsLineItem* newLine=scene()->addLine(tempLine->line().x2(),
                                                        tempLine->line().y2(),
                                                        tempPoint.x(),
                                                        tempPoint.y());
            newLine->setVisible(this->isVisible());

            QColor color(Qt::cyan);
            color.setAlphaF(0.3);
            if(index>0)
                color.setRgbF(0.3+0.7/index,0,0,0.3);

            newLine->setPen(QPen(color,
                                 3,
                                 Qt::SolidLine,
                                 Qt::RoundCap,
                                 Qt::RoundJoin));

            traj.push_back(newLine);
            TGeoPoint geoPoint(lat,lon);
            trajGeoPoints.push_back(geoPoint);
            if(trajGeoPoints.size() >1000)
                trajGeoPoints.removeFirst();

            lastTimeForTraj=currentTime;
            addPoint=false;
        }else
        {
            if(this->scene()!=nullptr)
            {
                traj.push_back(this->scene()->addLine(tempPoint.x(),
                                                  tempPoint.y(),
                                                  tempPoint.x(),
                                                  tempPoint.y()));
                traj.back()->setVisible(this->isVisible());
                TGeoPoint geoPoint(lat,lon);
                trajGeoPoints.push_back(geoPoint);
                if(trajGeoPoints.size() >1000)
                    trajGeoPoints.removeFirst();
                lastTimeForTraj = currentTime;
                addPoint = false;
            }
        }
    }else
    {
        addPoint=true;
    }

}
ColorItem::ColorItem(QGraphicsItem* parent):QGraphicsRectItem(parent)
{

}
void ColorItem::setRowText(QString str,int row)
{
    if(lines.isEmpty()==true)
        return;
    if(row>=lines.size())
        row=lines.size()-1;

    lines[row]->setText(str);

    double height_=0;
    for(int i=0;i<lines.size();i++)
    {
        lines[i]->setPos(-3,height_);
        height_+=lines[i]->boundingRect().height();
    }
    calcResize();
}
void ColorItem::calcResize()
{
    double maxWidth=0;
    double maxHeight=0;
    for(int i=0;i<lines.size();i++)
    {
        QRectF rect=lines[i]->boundingRect();
        if(rect.width()>maxWidth) maxWidth=rect.width();
        maxHeight+=rect.height();
    }
    setRect(-20,0,maxWidth+20,maxHeight);
}
void ColorItem::setRows(unsigned rows)
{
    lines.clear();
    QGraphicsSimpleTextItem *tempItem = nullptr;
    for(unsigned i=0;i<rows;i++)
    {
        tempItem = new QGraphicsSimpleTextItem(this);
        lines.push_back(tempItem);
    }
}
void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget)
    QColor color_(Qt::gray);
    color_.setAlpha(180);
    painter->setBrush(QBrush(color_));
    painter->setPen(QPen(Qt::NoPen));
    painter->drawRect(this->boundingRect());
}

AircraftObject::AircraftObject(QString nameI,QString nameFile,QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
{
    setZValue(10);
    initMessureItem();
    name        = nameI;
    vc          = 250;
    teta        = 0;
    y           = 3500;
    vy          = 0.0;
    prVy        = false;

    startEarth  = true;

    setX_ust(0);
    setZ_ust(0);

    setAcceptHoverEvents(true);
    setScale(0.1);
    formSetting=new FormSettingForAircraft();
    formSetting->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width()+20,
                       rect.height(),
                       rect.width()+20,rect.height()*(1-vc/(formSetting->maxV()-formSetting->minV()+1)));

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed=new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(nameI);
    transItem.translate(0.0,rect.height()+100);
    transItem.scale(10.0,10.0);
    colorItem->setTransform(transItem);


    colorItem->setRows(3);
    colorItem->setRowText(nameI,0);
    colorItem->setRowText(tr("Высота=")+QString::number(y),1);
    colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);

    connect(formSetting,SIGNAL(signalH(double)),    this,SLOT(setY(double)));
    connect(formSetting,SIGNAL(signalPsi(double)),  this,SLOT(setPsi(double)));
    connect(formSetting,SIGNAL(signalTeta(double)), this,SLOT(setTeta(double)));
    connect(formSetting,SIGNAL(signalV(double)),    this,SLOT(setVc(double)));
    connect(formSetting,SIGNAL(signalLon(double,QString)),this,SLOT(slotLonToX(double,QString)));
    connect(formSetting,SIGNAL(signalLat(double,QString)),this,SLOT(slotLatToZ(double,QString)));
    connect(formSetting,SIGNAL(signalTanUnt(bool)),this,SLOT(slotTetaUnt(bool)));
    connect(formSetting,SIGNAL(signalPrVy(bool)),this,SLOT(slotTetaUnt(bool)));

    setFlags(QGraphicsItem::ItemIsSelectable| QGraphicsItem::ItemIsMovable);
}
AircraftObject::AircraftObject(AircraftObject *aircraft_,QString nameFile,QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
{
    setZValue(10);
    initMessureItem();
    name=aircraft_->name;
    startEarth = aircraft_->isStartEarth();

    setAcceptHoverEvents(true);
    setScale(0.1);
    formSetting=new FormSettingForAircraft();
    formSetting->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width()+20,
                       rect.height(),
                       rect.width()+20,rect.height()*(1-vc/(formSetting->maxV()-formSetting->minV()+1)));

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed=new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(name);
    transItem.translate(0.0,rect.height()+100);
    transItem.scale(10.0,10.0);
    colorItem->setTransform(transItem);


    colorItem->setRows(3);
    colorItem->setRowText(name,0);
    colorItem->setRowText(tr("Высота=")+QString::number(y),1);
    colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);

    setLat(aircraft_->lat);
    setLon(aircraft_->lon);

    setVc(aircraft_->currentV());
    setPsi(aircraft_->currentPsi());
    setTeta(aircraft_->currentTeta());
    setVy(aircraft_->currentVy());
    setPrVy(aircraft_->currentPrVy());

    setY(aircraft_->currentY());


    connect(formSetting,SIGNAL(signalPrVy(bool)),this,SLOT(slotTetaUnt(bool)));

    setFlags(QGraphicsItem::ItemIsSelectable| QGraphicsItem::ItemIsMovable);
    //map=map;
}
//! текущие ед. измерения
QString AircraftObject::curMessV()
{
    if(use_russian==true)
        return currentUnitTransV->unicode_rus;
    else
        return currentUnitTransV->unicode_id;
}
QString AircraftObject::curExpV()
{
    if(use_russian==true)
        return currentExpTransV->unicode_rus;
    else
        return currentExpTransV->unicode_id;
}
QString AircraftObject::curMessY()
{
    if(use_russian==true)
        return currentUnitTransY->unicode_rus;
    else
        return currentUnitTransY->unicode_id;
}
QString AircraftObject::curExpY()
{
    if(use_russian==true)
        return currentExpTransY->unicode_rus;
    else
        return currentExpTransY->unicode_id;
}
QString AircraftObject::curMessPsi()
{
    if(use_russian==true)
        return currentUnitTransPsi->unicode_rus;
    else
        return currentUnitTransPsi->unicode_id;
}
QString AircraftObject::curExpPsi()
{
    if(use_russian==true)
        return currentExpTransPsi->unicode_rus;
    else
        return currentExpTransPsi->unicode_id;
}
QString AircraftObject::curMessTeta()
{
    if(use_russian==true)
        return currentUnitTransTeta->unicode_rus;
    else
        return currentUnitTransTeta->unicode_id;
}
QString AircraftObject::curExpTeta()
{
    if(use_russian==true)
        return currentExpTransTeta->unicode_rus;
    else
        return currentExpTransTeta->unicode_id;
}
QString AircraftObject::curMessVy()
{
    if(use_russian==true)
        return currentUnitTransVy->unicode_rus;
    else
        return currentUnitTransVy->unicode_id;
}
QString AircraftObject::curExpVy()
{
    if(use_russian==true)
        return currentExpTransVy->unicode_rus;
    else
        return currentExpTransVy->unicode_id;
}

QString AircraftObject::curMessDelta_hc()
{
    if(use_russian==true)
        return currentUnitTransDelta_hc->unicode_rus;
    else
        return currentUnitTransDelta_hc->unicode_id;
}
QString AircraftObject::curExpDelta_hc()
{
    if(use_russian==true)
        return currentExpTransDelta_hc->unicode_rus;
    else
        return currentExpTransDelta_hc->unicode_id;
}
QString AircraftObject::curMessAlfa_c()
{
    if(use_russian==true)
        return currentUnitTransAlfa_c->unicode_rus;
    else
        return currentUnitTransAlfa_c->unicode_id;
}
QString AircraftObject::curExpAlfa_c()
{
    if(use_russian==true)
        return currentExpTransAlfa_c->unicode_rus;
    else
        return currentExpTransAlfa_c->unicode_id;
}
void AircraftObject::setCurMessV(QString value)
{
    TObjectUnit* newUnitTrans=unitSpeed->find(value);
    vc=unitSpeed->convert(vc,currentUnitTransV,newUnitTrans);
    currentUnitTransV=newUnitTrans;
}
//void AircraftObject::setCurExpV(QString value)
//{
//    TObjectUnit* newUnitTrans=unitExp->find(value);
//    vc=unitExp->convert(vc,currentUnitTransV,newUnitTrans);
//    currentUnitTransV=newUnitTrans;
//}
void AircraftObject::setCurMessY(QString value)
{
    TObjectUnit* newUnitTrans=unitLength->find(value);
    y=unitLength->convert(y,currentUnitTransY,newUnitTrans);
    currentUnitTransY=newUnitTrans;
}
void AircraftObject::setCurMessPsi(QString value)
{
    TObjectUnit* newUnitTrans=unitAngle->find(value);
    psi=unitAngle->convert(psi,currentUnitTransPsi,newUnitTrans);
    currentUnitTransPsi=newUnitTrans;
}
void AircraftObject::setCurMessTeta(QString value)
{
    TObjectUnit* newUnitTrans=unitAngle->find(value);
    teta=unitAngle->convert(teta,currentUnitTransTeta,newUnitTrans);
    currentUnitTransTeta=newUnitTrans;
}
void AircraftObject::setCurMessVy(QString value)
{
    TObjectUnit* newUnitTrans=unitSpeed->find(value);
    vy=unitSpeed->convert(vy,currentUnitTransVy,newUnitTrans);
    currentUnitTransVy=newUnitTrans;
}

void AircraftObject::slotTetaUnt(bool flag)
{
    vy=flag;
    setTeta(teta);
    formSetting->setUntOtVy(flag);
}
void AircraftObject::loadXML(QDomElement tempNode)
{
    setLat(tempNode.attribute("lat","0").toDouble());
    setLon(tempNode.attribute("lon","0").toDouble());

    setPosC(tempNode.attribute("xMap","0").toDouble(),tempNode.attribute("zMap","0").toDouble());

    setVc(tempNode.attribute("vc","0").toDouble());
    setCurMessV(tempNode.attribute("messVc","m/s"));

    setPsi(tempNode.attribute("psi","0").toDouble());
    setCurMessPsi(tempNode.attribute("messPsi","deg"));

    setPrVy(tempNode.attribute("prVy_Teta","0").toInt());
    setTeta(tempNode.attribute("teta","0").toDouble());
    setCurMessTeta(tempNode.attribute("messTeta","deg"));

    setVy(tempNode.attribute("vy","0").toDouble());
    setCurMessVy(tempNode.attribute("messVy","m/s"));

    setY(tempNode.attribute("y","0").toDouble());
    setCurMessY(tempNode.attribute("messY","m"));
    setName(tempNode.attribute("name",""));


    setStartEarth(tempNode.attribute("typeStart","0").toUInt());
}
void AircraftObject::saveXML(QDomDocument &domDocument,QDomElement &node)
{
    QDomElement tempNode=domDocument.createElement("Aircraft");
    tempNode.setAttribute("vc",unitSpeed->convert(vc,currentUnitTransV,"m/s"));
    tempNode.setAttribute("messVc",curMessV());
    tempNode.setAttribute("defualtMesVc","m/s");

    tempNode.setAttribute("teta",unitAngle->convert(teta,currentUnitTransTeta,"deg"));
    tempNode.setAttribute("messTeta",curMessTeta());
    tempNode.setAttribute("defualtMesTeta","deg");

    tempNode.setAttribute("psi",unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    tempNode.setAttribute("messPsi",curMessPsi());
    tempNode.setAttribute("defualtMesPsi","deg");

    tempNode.setAttribute("y",unitLength->convert(y,currentUnitTransY,"m"));
    tempNode.setAttribute("messY",curMessY());
    tempNode.setAttribute("defualtMesY","m");

    tempNode.setAttribute("xMap",posC().x());
    tempNode.setAttribute("zMap",posC().y());

    tempNode.setAttribute("lat",lat);
    tempNode.setAttribute("lon",lon);

    tempNode.setAttribute("vy",unitSpeed->convert(vy,currentUnitTransVy,"m/s"));
    tempNode.setAttribute("messVy",curMessVy());
    tempNode.setAttribute("defualtMesVy","m/s");

    tempNode.setAttribute("prVy_Teta",prVy);
    tempNode.setAttribute("typeStart",startEarth);


    //tempNode.setAttribute("messVy",curMessVy());


    tempNode.setAttribute("name",name);
    node.appendChild(tempNode);
}
void AircraftObject::getRequest(QString prefix, TCommonRequest *request)
{


        QString prefixName = prefix;// + "INITDesArcraft.";
        request->append(prefixName+"Vist",      QString::number(unitSpeed->convert(vc,currentUnitTransV,"km/h")));
        request->append(prefixName+"Psi",       QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg")));
        request->append(prefixName+"Tan0",      QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg")));
        request->append(prefixName+"OnGround",  QString::number(startEarth));
        request->append(prefixName+"Lat",       QString::number(unitAngle->convert(lat,currentUnitTransPsi,"deg")));
        request->append(prefixName+"Lon",       QString::number(unitAngle->convert(lon,currentUnitTransPsi,"deg")));
        request->append(prefixName+"Y",         QString::number(unitLength->convert(y,currentUnitTransY,"m")));


     

}

void AircraftObject::saveXMLForModel(QDomDocument &domDocument,QDomElement &ele)
{

        //узел=вертолет
        QDomElement tempNode=domDocument.createElement("Aircraft");
        ele.appendChild(tempNode);
        //узел=скорость вертолета
        QDomElement tempNodeAircraft=domDocument.createElement("vc_aircraft");
        tempNodeAircraft.setAttribute("name",tr("skorost samoleta"));
        QDomText tempTextNode=domDocument.createTextNode(QString::number(unitSpeed->convert(vc,currentUnitTransV,"m/s")));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);
        //узел=курс вертолета
        tempNodeAircraft=domDocument.createElement("psic_aircraft");
        tempNodeAircraft.setAttribute("name",tr("Kurs samoleta"));
        tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg")));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);
        //узел=высота вертолета
        tempNodeAircraft=domDocument.createElement("yc_aircraft");
        tempNodeAircraft.setAttribute("name",tr("Vysota samoleta"));
        tempTextNode=domDocument.createTextNode(QString::number(unitLength->convert(y,currentUnitTransY,"m")));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);
        //узел=угол тангажа/наклона траектории
        tempNodeAircraft=domDocument.createElement("tetac_aircraft");
        tempNodeAircraft.setAttribute("name",tr("Ugol tangazha/naklona traektorii"));

        //признак установки угла тангажа/вертикальной скорости
        if(this->prVy==true)
        {
            double valueVy=unitSpeed->convert(vy,currentUnitTransVy,"m/s");
            if(valueVy>0.0)
                valueVy+=1000.;
            else
                valueVy+=-1000.;
            tempTextNode=domDocument.createTextNode(QString::number(valueVy));
        }
        else
            tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg")));

        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);
        /////////////////////////////////////////////////////////////
        //! координаты fi и lam
        tempNodeAircraft=domDocument.createElement("lat");
        tempNodeAircraft.setAttribute("name",tr("Shirota"));
        tempTextNode=domDocument.createTextNode(QString::number(lat,'g',14));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

        tempNodeAircraft=domDocument.createElement("lon");
        tempNodeAircraft.setAttribute("name",tr("Dolgota"));
        tempTextNode=domDocument.createTextNode(QString::number(lon,'g',14));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

}

void AircraftObject::refresh()
{
    itemSvg->setSelected(this->isSelected());
}
void AircraftObject::slotLonToX(double value,QString strValue)
{
    lon=value;
    int pX,pZ;
    sLon=strValue;

    latLongToPixelXY(lat,lon,zoom-1,pX,pZ);
    //setXMap(pX);
    //setZMap(pZ);
}
void AircraftObject::slotLatToZ(double value, QString strValue)
{
    lat=value;
    int pX,pZ;
    sLat=strValue;

    latLongToPixelXY(lat,lon,zoom - 1,pX,pZ);
    //setXMap(pX);
    //setZMap(pZ);
}
void AircraftObject::slotLonToX(double value)
{
    lon = value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom - 1,pX,pZ);

    setPosC(pX,pZ);
}

void AircraftObject::slotLatToZ(double value)
{
    lat=value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom-1,pX,pZ);


    setPosC(pX,pZ);
}
void AircraftObject::slotLonLatToXZ(double lat_, double lon_)
{
    lat = lat_;
    lon = lon_;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom - 1,pX,pZ);
    setPosC(pX,pZ);
}

void AircraftObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button()==Qt::RightButton)
    {
        formSetting->setV(vc);
        formSetting->setH(y);
        formSetting->setPsi(psi);
        formSetting->setTeta(teta);

        formSetting->show();
    }
    ObjectGraphNode::mousePressEvent(event);
}
void AircraftObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit signalReleaseAircraft();
    }
    ObjectGraphNode::mouseReleaseEvent(event);
}
void AircraftObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit sigDoubleClick();
    ObjectGraphNode::mouseDoubleClickEvent(event);
}

void AircraftObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToScene(itemSvg->transformOriginPoint());

    pixelXYToLatLong(p,zoom-1,lat,lon);

    formSetting->setLat(lat);
    formSetting->setLon(lon);

    TGeoPoint pt(lat,lon);
    emit isModifyPosition(pos(),pt);
    ObjectGraphNode::mouseMoveEvent(event);
}
void AircraftObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit sigHoverEnterEvent(true);
    refresh();
    ObjectGraphNode::hoverEnterEvent(event);
}
void AircraftObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit sigHoverEnterEvent(false);
    refresh();
    ObjectGraphNode::hoverLeaveEvent(event);
}
//AirObj *AirObj::clone()
//{
//    return new AirObj(this,map);
//}
AirObj::AirObj(AirObj *airTarget,
                                 AircraftObject *aircraft,
                                 QGraphicsItem *parent):ObjectGraphNode(airTarget,parent)
{
    setZValue(9);
    initMessureItem();
    code_prev = 0;

    d = 2000;
    formSetting=new FormSettingForAirTarget();
    formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width() + 20,
                       rect.height(),
                       rect.width() + 20,rect.height() * (1 - v/(formSetting->maxV() - formSetting->minV())) + 1);

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed = new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(name);

    lineNotEnable    = new QGraphicsLineItem(QLineF(0.0,0.0,rect.width(),rect.height()),itemSvg);
    ellipseNotEnable = new QGraphicsEllipseItem(rect,itemSvg);

    lineToAircraft  = new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
    colorItemD      = new ColorItem(lineToAircraft);
    colorItemD->setScale(10);
    colorItemD->setRows(1);
    colorItemD->setRowText("d="+QString::number(d) + tr("м"),0);
    //lineToAircraftTextD=new QGraphicsSimpleTextItem("d="+QString::number(d),lineToAircraft);

    QPen penA(Qt::SolidLine);
    penA.setColor(Qt::gray);
    penA.setWidth(20);
    lineToAircraft->setPen(penA);
    lineNotEnable->setPen(penA);
    ellipseNotEnable->setPen(penA);
    lineNotEnable->setVisible(!isEnable());
    ellipseNotEnable->setVisible(!isEnable());

    transName.translate(0.0,rect.height()+10);
    transName.scale(10.0,10.0);
    colorName->setTransform(transName);

    colorName->setRows(1);

    transItem.translate(0.0,rect.height()+rect.height()/3);
    transItem.scale(10.0,10.0);
    colorItem->setTransform(transItem);

    colorItem->setRows(4);

    colorItem->setRowText(tr("Высота=")+QString::number(y),1);
    colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);
    colorItem->setRowText(tr("Тип объекта=")+QString::number(code),3);
    colorItem->setVisible(false);

    setAircraft(aircraft);

    setFi(airTarget->currentFi());
    setD(airTarget->currentD());
    setY(airTarget->currentY());
    setV(airTarget->currentV());
    setTeta(airTarget->currentTeta());

    setCode(airTarget->currentCode(),airTarget->currentCodeStr());

    setScale(0.1);
    setAcceptHoverEvents(true);

    connect(formSetting,SIGNAL(signalH(double)),    this,SLOT(setY(double)));
    connect(formSetting,SIGNAL(signalPsi(double)),  this,SLOT(setPsi(double)));
    //connect(formSetting,SIGNAL(signalFi(double)),   this,SLOT(setFi(double)));
    connect(formSetting,SIGNAL(signalV(double)),    this,SLOT(setV(double)));
    //connect(formSetting,SIGNAL(signalD(double)),    this,SLOT(setD(double)));
    connect(formSetting,SIGNAL(signalEnable(bool)), this,SLOT(setEnable(bool)));
    connect(formSetting,SIGNAL(signalCode(int,QString)),this,SLOT(setCode(int,QString)));
    connect(formSetting,SIGNAL(signalLat(double,QString)),this,SLOT(slotLatToZ(double,QString)));
    connect(formSetting,SIGNAL(signalLon(double,QString)),this,SLOT(slotLonToX(double,QString)));
    connect(formSetting,SIGNAL(signalTeta(double)), this,SLOT(setTeta(double)));

    setCode(airTarget->currentCode());
}

AirObj::AirObj(QString name_,
                                 QString nameFile,
                                 QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
{
    setZValue(9);
    initMessureItem();
    name        = name_;
    aircraft    = nullptr;
    code        = 100;


    fi          = 0.0;
    d           = 2000.0;
    y           = 3500;
    v           = 210;
    teta        = 0.0;

    code_prev   = 0;

    setScale(0.1);
    setAcceptHoverEvents(true);

    formSetting=new FormSettingForAirTarget();
    formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width()+20,
                       rect.height(),
                       rect.width()+20,rect.height()*(1-v/(formSetting->maxV()-formSetting->minV()))+1);

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed=new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(name);


    lineNotEnable=new QGraphicsLineItem(QLineF(0.0,0.0,rect.width(),rect.height()),itemSvg);
    ellipseNotEnable=new QGraphicsEllipseItem(rect,itemSvg);

    lineToAircraft=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
    colorItemD=new ColorItem(lineToAircraft);
    colorItemD->setScale(10);
    colorItemD->setRows(1);
    colorItemD->setRowText("d="+QString::number(d)+tr("м"),0);
    //lineToAircraftTextD=new QGraphicsSimpleTextItem("d="+QString::number(d),lineToAircraft);

    QPen penA(Qt::SolidLine);
    penA.setColor(Qt::gray);
    penA.setWidth(20);
    lineToAircraft->setPen(penA);
    lineNotEnable->setPen(penA);
    ellipseNotEnable->setPen(penA);
    lineNotEnable->setVisible(!isEnable());
    ellipseNotEnable->setVisible(!isEnable());


    transName.translate(0.0,rect.height()+10);
    transName.scale(10.,10.);
    colorName->setTransform(transName);

    colorName->setRows(1);
    colorName->setRowText(name,0);

    transItem.translate(0.0,rect.height()+rect.height()/3);
    transItem.scale(10.,10.);
    colorItem->setTransform(transItem);


    colorItem->setRows(4);
    //colorItem->setRowText(name,0);
    colorItem->setRowText(tr("Высота=")+QString::number(y),1);
    colorItem->setRowText(tr("УНТ=")+QString::number(teta),2);
    colorItem->setRowText(tr("Тип объекта=")+QString::number(code),3);
    colorItem->setVisible(false);

    connect(formSetting,SIGNAL(signalH(double)),        this,SLOT(setY(double)));
    connect(formSetting,SIGNAL(signalPsi(double)),      this,SLOT(setPsi(double)));
    //connect(formSetting,SIGNAL(signalFi(double)),     this,SLOT(setFi(double)));
    connect(formSetting,SIGNAL(signalV(double)),        this,SLOT(setV(double)));
    //connect(formSetting,SIGNAL(signalD(double)),      this,SLOT(setD(double)));
    connect(formSetting,SIGNAL(signalEnable(bool)),     this,SLOT(setEnable(bool)));
    connect(formSetting,SIGNAL(signalCode(int,QString)),this,SLOT(setCode(int,QString)));
    //connect(formSetting,SIGNAL(signalLat(double,QString)),this,SLOT(slotLatToZ(double,QString)));
    //connect(formSetting,SIGNAL(signalLon(double,QString)),this,SLOT(slotLonToX(double,QString)));
    connect(formSetting,SIGNAL(signalTeta(double)), this,SLOT(setTeta(double)));

    setCode(code);
}
void AirObj::loadXML(QDomElement tempNode)
{
    setPosC(tempNode.attribute("xMap","0").toDouble(),
           tempNode.attribute("zMap","0").toDouble());

    setLat(tempNode.attribute("lat","0").toDouble());
    setLon(tempNode.attribute("lon","0").toDouble());

    setV(tempNode.attribute("v","0").toDouble());
    setCurMessV(tempNode.attribute("messV","m/s"));

    setPsi(tempNode.attribute("psi","0").toDouble());
    setCurMessPsi(tempNode.attribute("messPsi","deg"));

    setFi(tempNode.attribute("fi","0").toDouble());
    setCurMessFi(tempNode.attribute("messFi","deg"));

    setY(tempNode.attribute("y","0").toDouble());
    setCurMessY(tempNode.attribute("messY","m"));

    setD(tempNode.attribute("d","0").toDouble());
    setCurMessD(tempNode.attribute("messD","m"));

    setName(tempNode.attribute("name",""));
    setEnable((tempNode.attribute("enable","1")).toInt());

    setTeta(tempNode.attribute("teta","0").toDouble());
    setCurMessTeta(tempNode.attribute("messTeta","m"));



}
QString AirObj::curMessTime()
{
    return QString("sec");
}
//! текущие ед. измерения
QString AirObj::curMessV()
{
    if(use_russian==true)
        return currentUnitTransV->unicode_rus;
    else
        return currentUnitTransV->unicode_id;
}
QString AirObj::curMessY()
{
    if(use_russian==true)
        return currentUnitTransY->unicode_rus;
    else
        return currentUnitTransY->unicode_id;
}
QString AirObj::curMessPsi()
{
    if(use_russian==true)
        return currentUnitTransPsi->unicode_rus;
    else
        return currentUnitTransPsi->unicode_id;
}
QString AirObj::curMessTeta()
{
    if(use_russian==true)
        return currentUnitTransTeta->unicode_rus;
    else
        return currentUnitTransTeta->unicode_id;
}
QString AirObj::curMessFi()
{
    if(use_russian==true)
        return currentUnitTransFi->unicode_rus;
    else
        return currentUnitTransFi->unicode_id;
}
QString AirObj::curMessD()
{
    if(use_russian==true)
        return currentUnitTransD->unicode_rus;
    else
        return currentUnitTransD->unicode_id;
}
QString AirObj::curMessLength()
{
    if(use_russian==true)
        return currentUnitTransLength->unicode_rus;
    else
        return currentUnitTransLength->unicode_id;
}
void AirObj::setCurMessV(QString value)
{
    TObjectUnit* newUnitTrans = unitSpeed->find(value);
    v = unitSpeed->convert(v,currentUnitTransV,newUnitTrans);
    currentUnitTransV = newUnitTrans;
}
void AirObj::setCurMessY(QString value)
{
    TObjectUnit* newUnitTrans = unitLength->find(value);
    y = unitLength->convert(y,currentUnitTransY,newUnitTrans);
    currentUnitTransY = newUnitTrans;
}
void AirObj::setCurMessPsi(QString value)
{
    TObjectUnit* newUnitTrans=unitAngle->find(value);
    psi=unitAngle->convert(psi,currentUnitTransPsi,newUnitTrans);
    currentUnitTransPsi=newUnitTrans;
}
void AirObj::setCurMessTeta(QString value)
{
    TObjectUnit* newUnitTrans=unitAngle->find(value);
    teta=unitAngle->convert(teta,currentUnitTransTeta,newUnitTrans);
    currentUnitTransTeta=newUnitTrans;
}
void AirObj::setCurMessFi(QString value)
{
    TObjectUnit* newUnitTrans=unitAngle->find(value);
    fi=unitAngle->convert(fi,currentUnitTransFi,newUnitTrans);
    currentUnitTransFi=newUnitTrans;
}
void AirObj::setCurMessD(QString value)
{
    TObjectUnit* newUnitTrans=unitLength->find(value);
    d=unitLength->convert(d,currentUnitTransD,newUnitTrans);
    currentUnitTransD=newUnitTrans;
}
void AirObj::setCurMessLength(QString value)
{
    TObjectUnit* newUnitTrans=unitLength->find(value);
    //length=unitLength->convert(length,currentUnitTransLength,newUnitTrans);
    currentUnitTransLength=newUnitTrans;
}

//void AirObj::getRequest(QString prefix,TCommonRequest *request,bool circleVariant,int numIndex)
//{
//    if(numIndex== -1)
//        numIndex = index;

//    if(circleVariant==false)
//    {
//        QString prefixName = prefix + "INITDesTarget["+QString::number(numIndex)+"].";

//        request->append(prefixName+"Global_numTarget",  QString::number(numIndex));
//        request->append(prefixName+"TipTarget",         QString::number(code));
//        request->append(prefixName+"Speed0Target",      QString::number(unitSpeed->convert(v,currentUnitTransV,"m/s")));
//        request->append(prefixName+"Psi0Target",        QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg"),'g',14));
//        request->append(prefixName+"Hight0Target",      QString::number(unitLength->convert(y,currentUnitTransY,"m")));
//        request->append(prefixName+"Tetta0Target",      QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg"),'g',9));
//        request->append(prefixName+"Fi0Target",         QString::number(unitAngle->convert(fi,currentUnitTransFi,"deg"),'g',14));
//        request->append(prefixName+"Dr0Target",         QString::number(unitLength->convert(d,currentUnitTransD,"m"),'g',14));
//        //request->append(prefixName+"Beg_num_target",QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg")));
//    }else
//    {
//        QString prefixName = prefix + "INITDes_Target.";

//        request->append(prefixName+"L",          QString::number(code));
//        request->append(prefixName+"Ng_z",       QString::number(ng_z));
//        request->append(prefixName+"V",          QString::number(unitSpeed->convert(v,currentUnitTransV,"m/s")));
//        request->append(prefixName+"Psi",        QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg"),'g',14));
//        request->append(prefixName+"H",          QString::number(unitLength->convert(y,currentUnitTransY,"m")));
//        request->append(prefixName+"Fi",         QString::number(unitAngle->convert(fi,currentUnitTransFi,"deg"),'g',14));
//        request->append(prefixName+"Dr_aircraft",QString::number(unitLength->convert(d,currentUnitTransD,"m"),'g',14));
//    }
//}

void AirObj::saveXMLForModel(QDomDocument &domDocument,QDomElement &ele)
{

        //узел=вертолет
        QDomElement tempNode=domDocument.createElement("Air_Target");
        QDomText tempTextNode=domDocument.createTextNode(QString::number(index));
        tempNode.appendChild(tempTextNode);
        ele.appendChild(tempNode);

        //узел=скорость вертолета
        QDomElement tempNodeAircraft=domDocument.createElement("Tip_target_A");
        tempNodeAircraft.setAttribute("name",tr("Tip target"));

        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

        //узел=курс вертолета
        tempNodeAircraft=domDocument.createElement("psi_target_A");
        tempNodeAircraft.setAttribute("name",tr("Kurs target"));
        tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg"),'g',14));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

        //узел=курс вертолета
        tempNodeAircraft=domDocument.createElement("v_target_A");
        tempNodeAircraft.setAttribute("name",tr("speed target"));
        tempTextNode=domDocument.createTextNode(QString::number(unitSpeed->convert(v,currentUnitTransV,"m/s")));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

        //узел=высота вертолета
        tempNodeAircraft=domDocument.createElement("y_target");
        tempNodeAircraft.setAttribute("name",tr("H target"));
        tempTextNode=domDocument.createTextNode(QString::number(unitLength->convert(y,currentUnitTransY,"m")));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);
        //узел=угол тангажа/наклона траектории
        tempNodeAircraft=domDocument.createElement("teta_target");
        tempNodeAircraft.setAttribute("name",tr("Ugol tangazha/naklona traektorii"));
        tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg"),'g',9));
        tempNodeAircraft.appendChild(tempTextNode);

        //узел= направление на цель
        tempNode.appendChild(tempNodeAircraft);
        tempNodeAircraft=domDocument.createElement("fi_target");
        tempNodeAircraft.setAttribute("name",tr("Napravlenie na tsel (otnositelno samoleta)"));
        tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(fi,currentUnitTransFi,"deg"),'g',14));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

        //узел= расстояние до цели
        tempNodeAircraft=domDocument.createElement("D_target");
        tempNodeAircraft.setAttribute("name",tr("Rasstoyanie do tseli"));
        tempTextNode=domDocument.createTextNode(QString::number(unitLength->convert(d,currentUnitTransD,"m"),'g',14));
        tempNodeAircraft.appendChild(tempTextNode);
        tempNode.appendChild(tempNodeAircraft);

}
void AirObj::saveXML(QDomDocument &domDocument,QDomElement &node)
{
    QDomElement tempNode=domDocument.createElement("AirTarget");
    tempNode.setAttribute("name",name);
    tempNode.setAttribute("v",unitSpeed->convert(v,currentUnitTransV,"m/s"));
    tempNode.setAttribute("messV",curMessV());
    tempNode.setAttribute("defualtMesV","m/s");

    tempNode.setAttribute("d",unitLength->convert(d,currentUnitTransD,"m"));
    tempNode.setAttribute("messD",curMessD());
    tempNode.setAttribute("defualtMesPsi","m");

    tempNode.setAttribute("psi",unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    tempNode.setAttribute("messPsi",curMessPsi());
    tempNode.setAttribute("defualtMesPsi","deg");

    tempNode.setAttribute("y",unitLength->convert(y,currentUnitTransY,"m"));
    tempNode.setAttribute("messY",curMessY());
    tempNode.setAttribute("defualtMesY","m");

    tempNode.setAttribute("xMap",posC().x());
    tempNode.setAttribute("zMap",posC().y());

    tempNode.setAttribute("lat",lat);
    tempNode.setAttribute("lon",lon);
    tempNode.setAttribute("fi",unitAngle->convert(fi,currentUnitTransFi,"deg"));
    tempNode.setAttribute("messFi",curMessFi());
    tempNode.setAttribute("defualtMesFi","deg");

    tempNode.setAttribute("codeObjectVis",code);
   // tempNode.setAttribute("length",unitLength->convert(length,currentUnitTransLength,"m"));
    tempNode.setAttribute("messLength",curMessLength());
    tempNode.setAttribute("defualtMesLength","deg");


    tempNode.setAttribute("enable",isEnable());
    tempNode.setAttribute("teta",unitAngle->convert(teta,currentUnitTransTeta,"deg"));
    tempNode.setAttribute("messTeta",curMessTeta());
    tempNode.setAttribute("defualtMesTeta","deg");

    node.appendChild(tempNode);
}

void AirObj::mousePressEvent (QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        formSetting->setV(v);
        formSetting->setH(y);
        //formSetting->setPsi(psi);
        //formSetting->setFi(fi);
        formSetting->setD(d);

        formSetting->show();
    }
    ObjectGraphNode::mousePressEvent(event);
}
void AirObj::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    pixelXYToLatLong(p.x(),p.y(),zoom - 1,lat,lon);

    TGeoPoint pt(lat,lon);
    emit isModifyPosition(pos(),pt);

    slotIsModifyPsi();
    slotEnterLeaveCur(true);
    ObjectGraphNode::mouseMoveEvent(event);
}
void AirObj::slotMovePos()
{
    slotEnterLeaveCur(true);
    slotFi();

}
void AirObj::slotIsModifyPsi()
{
    //! смещения относительно центра
    double dx=posC().x()-aircraft->posC().x();
    double dy=posC().y()-aircraft->posC().y();
    //! дальность
    d=sqrt(dx*dx+dy*dy)*groundResolution(lat,zoom-1);
    d=unitLength->convert(d,"m",currentUnitTransD);
    //! угол относительно вертолета
    fi=atan2(dx,-dy)*180.0/M_PI;
    fi=check180(fi-KursToPsiGrad(aircraft->psi));

    //formSetting->setFi(fi);
    //formSetting->setD(d);
    setFi(fi);
}
void AirObj::setD(double value)
{
    d=value;
    double tempFi=checkPI(GradToRadian(fi)+GradToRadian(KursToPsiGrad(aircraft->psi))+M_PI/2.0);

    double d_=unitLength->convert(d,currentUnitTransD,"m");
    double x=(-d_*cos(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().x();
    double z=(-d_*sin(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().y();

    pixelXYToLatLong(x,z,zoom-1,lat,lon);
    setPosC(x,z);
}
void AirObj::slotFi()
{
    double tempFi=checkPI(GradToRadian(fi)+GradToRadian(KursToPsiGrad(aircraft->psi))+M_PI/2.0);

    double d_=unitLength->convert(d,currentUnitTransD,"m");
    double x=(-d_*cos(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().x();
    double z=(-d_*sin(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().y();

    pixelXYToLatLong(x,z,zoom-1,lat,lon);
    setPosC(x,z);
    colorItem->setRowText(tr("fi=")+QString::number(fi),4);
    updateDToAircraft();
}

void AirObj::setFi(double value)
{
    //! сохранить значение угла на цель
    fi=value;
    double tempFi=checkPI(GradToRadian(fi)+GradToRadian(KursToPsiGrad(aircraft->psi))+M_PI/2.0);

    double d_=unitLength->convert(d,currentUnitTransD,"m");
    double x=(-d_*cos(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().x();
    double z=(-d_*sin(tempFi)/groundResolution(lat,zoom-1))+aircraft->posC().y();

    pixelXYToLatLong(x,z,zoom-1,lat,lon);
    setPosC(x,z);
    colorItem->setRowText(tr("fi=")+QString::number(fi),4);
}
void AirObj::slotIsModifyPosition()
{
    //! при модификации курсового угла
    slotIsModifyPsi();
    //! отрисовка прямой соединяющей вертолет и цель
    slotEnterLeaveCur(true);
}
void AirObj::slotEnterLeaveCur(bool flag)
{
    QPointF pointEnd=mapFromItem(itemSvg,itemSvg->transformOriginPoint());
    QPointF pointStart=mapFromItem(aircraft->itemSvg,aircraft->itemSvg->transformOriginPoint());
    QLineF line(pointStart,pointEnd);
    lineToAircraft->setLine(line);
    lineToAircraft->setVisible(flag);

    colorItemD->setPos(line.x1()+(line.x2()-line.x1())/2.0,
                       5+line.y1()+(line.y2()-line.y1())/2.0);
    colorItemD->setRowText("d="+QString::number(d)+curMessD(),0);
    colorItemD->setVisible(flag);
    //lineToAircraftTextD->setVisible(flag);
}
void AirObj::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    slotEnterLeaveCur(true);
    //colorItem->setRows(5);
    if(isAllInfo()==false)
        colorItem->setVisible(true);
}
void AirObj::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    slotEnterLeaveCur(false);
    //colorItem->setRows(1);

    if(isAllInfo()==false)
        colorItem->setVisible(false);
}
void AirObj::slotLonToX(double value)
{
    lon=value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom-1,pX,pZ);
    setPosC(pX,pZ);
}

void AirObj::slotLatToZ(double value)
{
    lat=value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom-1,pX,pZ);

    setPosC(pX,pZ);
}
GroundObj::GroundObj(QString name_,QString nameFile,QGraphicsItem *parent):ObjectGraphNode(nameFile,parent)
{
    //! имя объекта
    name=name_;
    //! положение объекта при перекрывании
    setZValue(8);
    //! инициализация элементов перевода ед. измерения
    initMessureItem();
    //! масштаб объекта
    setScale(0.1);
    //! признак задания кодов
    prCodeLen=true;
    //! длинна
    length=0.0;

    //enable=true;
    setAcceptHoverEvents(true);
    formSetting=new FormSettingForGroundTargets();
    formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);
    //! код объекта
    code = 311;
    //! тип системы коордиант
    sks = 1;
    //! координаты по умолчанию
    x = 0.0;
    z = 0.0;
    v = 0.0;


    //colorItemD=new ColorItem(this);
    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width()+20,
                       rect.height(),
                       rect.width()+20,rect.height()*(1-v/(formSetting->maxV()-formSetting->minV()))+1);

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed=new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(name);

    transName.translate(0.0,rect.height()+10);
    transName.scale(10.,10.);
    colorName->setTransform(transName);

    colorName->setRows(1);
    colorName->setRowText(name,0);

    transItem.translate(0.0,rect.height()+rect.height()/3);
    transItem.scale(10.,10.);
    colorName->setTransform(transItem);


    colorItem->setRows(2);

    colorItem->setVisible(false);

    lineNotEnable=new QGraphicsLineItem(QLineF(0.0,0.0,rect.width(),rect.height()),itemSvg);
    lineToAircraft=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
    colorItemD=new ColorItem(lineToAircraft);
    colorItemD->setScale(10);
    colorItemD->setRows(1);
    colorItemD->setRowText("d="+QString::number(d),0);
   // lineToAircraftTextD=new QGraphicsSimpleTextItem("d="+QString::number(d),lineToAircraft);

    QPen penA(Qt::SolidLine);
    penA.setColor(Qt::gray);
    lineToAircraft->setPen(penA);
    lineNotEnable->setPen(penA);
    lineNotEnable->setVisible(!isEnable());

    colorItem->setRowText(tr("Тип объекта="),1);

    connect(formSetting,SIGNAL(signalPsi(double)),  this,SLOT(setPsi(double)));
    connect(formSetting,SIGNAL(signalV(double)),    this,SLOT(setV(double)));
    connect(formSetting,SIGNAL(signalEnable(bool)), this,SLOT(setEnable(bool)));
    connect(formSetting,SIGNAL(signalCode(int,QString)),this,SLOT(setCode(int,QString)));
    connect(formSetting,SIGNAL(signalLat(double)),this,SLOT(slotLatToZ(double)));
    connect(formSetting,SIGNAL(signalLon(double)),this,SLOT(slotLonToX(double)));

    setCode(code);

}
//! для операции клонирования
GroundObj::GroundObj(GroundObj   *groundTarget, /*наземная цель*/
                   AircraftObject       *aircraft,     /*носителя*/
                   QGraphicsItem        *parent):ObjectGraphNode(groundTarget,parent)
{
    //! положение объекта при перекрывании
    setZValue(8);
    //! инициализация элементов перевода ед. измерения
    initMessureItem();
    //! масштаб объекта
    setScale(0.1);

    setAircraft(aircraft);
    setLength(groundTarget->currentLength());
    setPrCodeLength(groundTarget->currentPrCodeLength());

    setXt(groundTarget->currentX());
    setZt(groundTarget->currentZ());
    setV(groundTarget->currentV());

    setAcceptHoverEvents(true);
    formSetting=new FormSettingForGroundTargets();
    formSetting->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint |Qt::WindowCloseButtonHint);

    //! тип системы коордиант
    sks=1;



    QRectF rect=itemSvg->boundingRect();
    lineFSpeed.setLine(rect.width()+20,
                       rect.height(),
                       rect.width()+20,rect.height()*(1-v/(formSetting->maxV()-formSetting->minV()))+1);

    QRectF rect1=boundingRect();
    setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));
    itemSvg->setTransformOriginPoint(QPointF(rect1.width()/2.0,rect1.height()/2.0));

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::blue);

    lineSpeed=new QGraphicsLineItem(lineFSpeed,itemSvg);
    lineSpeed->setPen(pen);

    formSetting->setWindowTitle(name);

    transName.translate(0.0,rect.height()+10);
    transName.scale(10.0,10);
    colorName->setTransform(transName);
    colorName->setRows(1);
    colorName->setRowText(name,0);

    transItem.translate(0.0,rect.height()+rect.height()/3);
    transItem.scale(10.0,10);
    colorItem->setTransform(transItem);

    colorItem->setRows(2);
    colorItem->setVisible(false);

    lineNotEnable=new QGraphicsLineItem(QLineF(0.0,0.0,rect.width(),rect.height()),itemSvg);
    lineToAircraft=new QGraphicsLineItem(QLineF(0.0,0.0,10.0,10.0),this);
    colorItemD=new ColorItem(lineToAircraft);
    colorItemD->setScale(10);
    colorItemD->setRows(1);
    colorItemD->setRowText("d="+QString::number(d),0);
   // lineToAircraftTextD=new QGraphicsSimpleTextItem("d="+QString::number(d),lineToAircraft);

    QPen penA(Qt::SolidLine);
    penA.setColor(Qt::gray);
    lineToAircraft->setPen(penA);
    lineNotEnable->setPen(penA);
    lineNotEnable->setVisible(!isEnable());

    colorItem->setRowText(tr("Тип объекта="),1);

    connect(formSetting,SIGNAL(signalPsi(double)),  this,SLOT(setPsi(double)));
    connect(formSetting,SIGNAL(signalV(double)),    this,SLOT(setV(double)));
    connect(formSetting,SIGNAL(signalEnable(bool)), this,SLOT(setEnable(bool)));
    connect(formSetting,SIGNAL(signalCode(int,QString)),this,SLOT(setCode(int,QString)));
    connect(formSetting,SIGNAL(signalLat(double)),this,SLOT(slotLatToZ(double)));
    connect(formSetting,SIGNAL(signalLon(double)),this,SLOT(slotLonToX(double)));

    setCode(groundTarget->currentCode());
}

void GroundObj::loadXML(QDomElement tempNode)
{
    setPosC(tempNode.attribute("xMap","0").toDouble(),
           tempNode.attribute("zMap","0").toDouble());
//    setXZMap(tempNode.attribute("xMap","0").toDouble(),
//             tempNode.attribute("zMap","0").toDouble());


    setLat(tempNode.attribute("lat","0").toDouble());
    setLon(tempNode.attribute("lon","0").toDouble());

    setV(tempNode.attribute("v","0").toDouble());
    setCurMessV(tempNode.attribute("messV","m/s"));

    setPsi(tempNode.attribute("psi","0").toDouble());
    setCurMessPsi(tempNode.attribute("messPsi","deg"));
    //target->setD(tempNode.attribute("d","0").toDouble());

    setName(tempNode.attribute("name",""));
    setEnable((tempNode.attribute("enable","1")).toInt());

    setXt(tempNode.attribute("x","0").toDouble());
    setCurMessX(tempNode.attribute("messX","m"));
    setZt(tempNode.attribute("z","0").toDouble());
    setCurMessZ(tempNode.attribute("messZ","m"));
#ifdef OLD_STEND
    setSKS(tempNode.attribute("sks","0").toInt());
#endif
}

void GroundObj::saveXML(QDomDocument &domDocument,QDomElement &node)
{
    QDomElement tempNode=domDocument.createElement("GroundTarget");
    tempNode.setAttribute("name",name);
    tempNode.setAttribute("v",unitSpeed->convert(v,currentUnitTransV,"m/s"));
    tempNode.setAttribute("messV",curMessV());
    tempNode.setAttribute("defualtMesV","m/s");

    tempNode.setAttribute("psi",unitAngle->convert(psi,currentUnitTransPsi,"deg"));
    tempNode.setAttribute("messPsi",curMessPsi());
    tempNode.setAttribute("defualtMesPsi","deg");

    tempNode.setAttribute("sks",sks);
    tempNode.setAttribute("x",unitLength->convert(x,currentUnitTransX,"m"));
    tempNode.setAttribute("messX",curMessX());
    tempNode.setAttribute("defualtMesX","m");

    tempNode.setAttribute("z",unitLength->convert(z,currentUnitTransZ,"m"));
    tempNode.setAttribute("messZ",curMessZ());
    tempNode.setAttribute("defualtMesZ","m");

    tempNode.setAttribute("xMap",posC().x());
    tempNode.setAttribute("zMap",posC().y());

    tempNode.setAttribute("lat",lat);
    tempNode.setAttribute("lon",lon);

    tempNode.setAttribute("codeObjectVis",code);
    tempNode.setAttribute("enable",isEnable());
    node.appendChild(tempNode);
}
void GroundObj::saveXMLForModel(QDomDocument &domDocument,QDomElement &ele)
{

    //узел=вертолет
    QDomElement tempNode=domDocument.createElement("Ground_Target");
    QDomText tempTextNode=domDocument.createTextNode(QString::number(index));
    tempNode.appendChild(tempTextNode);
    ele.appendChild(tempNode);

    //узел=скорость вертолета
    QDomElement tempNodeAircraft=domDocument.createElement("v_target_G");
    tempNodeAircraft.setAttribute("name",tr("Speed target"));
    tempTextNode=domDocument.createTextNode(QString::number(unitSpeed->convert(v,currentUnitTransV,"m/s")));
    tempNodeAircraft.appendChild(tempTextNode);
    tempNode.appendChild(tempNodeAircraft);

    tempNodeAircraft=domDocument.createElement("Tip_target_G");
    tempNodeAircraft.setAttribute("name",tr("Tip target"));
    tempTextNode=domDocument.createTextNode(QString::number(code));
    tempNodeAircraft.appendChild(tempTextNode);
    tempNode.appendChild(tempNodeAircraft);

    //узел=курс вертолета
    tempNodeAircraft=domDocument.createElement("psi_target_G");
    tempNodeAircraft.setAttribute("name",tr("Kurs target"));
    tempTextNode=domDocument.createTextNode(QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg")));
    tempNodeAircraft.appendChild(tempTextNode);
    tempNode.appendChild(tempNodeAircraft);

    //узел=высота вертолета
    tempNodeAircraft=domDocument.createElement("SKS_target");
    tempNodeAircraft.setAttribute("name",tr("Sistema koordinat"));
    tempTextNode=domDocument.createTextNode(QString::number(sks));
    tempNodeAircraft.appendChild(tempTextNode);
    tempNode.appendChild(tempNodeAircraft);

    //узел=угол тангажа/наклона траектории
    tempNodeAircraft=domDocument.createElement("x_target");
    tempNodeAircraft.setAttribute("name",tr("Koordinata X"));
    tempTextNode=domDocument.createTextNode(QString::number(unitLength->convert(x,currentUnitTransX,"m")));
    tempNodeAircraft.appendChild(tempTextNode);

    tempNode.appendChild(tempNodeAircraft);
    tempNodeAircraft=domDocument.createElement("z_target");
    tempNodeAircraft.setAttribute("name",tr("Koordinata Z"));
    tempTextNode=domDocument.createTextNode(QString::number(unitLength->convert(z,currentUnitTransZ,"m")));
    tempNodeAircraft.appendChild(tempTextNode);
    tempNode.appendChild(tempNodeAircraft);
}
void GroundObj::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
//    qDebug("Right Button");

    if(event->button()==Qt::RightButton)
    {
        formSetting->setV(v);
        formSetting->setPsi(psi);
        formSetting->setX(z);
        formSetting->setZ(x);

        formSetting->show();
    }
    ObjectGraphNode::mousePressEvent(event);
}
void GroundObj::slotIsModifyPsi()
{
    //! дальность  смещения относительно центра
    QPointF v = posC() - aircraft->posC();
    float r = QPointF::dotProduct(posC(),aircraft->posC());

    d =  r* groundResolution(lat,zoom - 1);
    d = unitLength->convert(d,"m",currentUnitTransD);

    //! расчет координат НЦ относительно модельной СК
    x = -v.y() * groundResolution(lat,zoom -1);
    z =  v.x() * groundResolution(lat,zoom -1);
    if(sks == 1)
    {
        //! положение НЦ относительно модельной
        glm::vec3 vec=glm::vec3(x,0.0,z);
        //! расчет матрицы курса
        glm::mat3 mPsi;
        //! курс вертолета
        double psi_c=GradToRadian(aircraft->psi);

        mPsi[0][0]=cos(psi_c);
        mPsi[0][1]=0.0;
        mPsi[0][2]=-sin(psi_c);

        mPsi[1][0]=0.0;
        mPsi[1][1]=1.0;
        mPsi[1][2]=0.0;

        mPsi[2][0]=sin(psi_c);
        mPsi[2][1]=0.0;
        mPsi[2][2]=cos(psi_c);

        glm::vec3 vec0=mPsi*vec;
        x=vec0.x; z=vec0.z;
    }
}
void GroundObj::getRequest(QString prefix,TCommonRequest *request,int numIndex)
{
//    if(numIndex == -1)
//        numIndex = index;
//    QString prefixName = prefix + "INITDesGRTarget["+QString::number(numIndex)+"].";

//    request->append(prefixName+"Global_numGRTarget",QString::number(numIndex));
//    request->append(prefixName+"Tip_GRTarget",      QString::number(code));
//    request->append(prefixName+"Speed0_RGTarget",   QString::number(unitSpeed->convert(v,currentUnitTransV,"m/s")));
//    request->append(prefixName+"Psi0_GRTarget",     QString::number(unitAngle->convert(psi,currentUnitTransPsi,"deg"),'g',14));
//    request->append(prefixName+"X0_GRTarget",       QString::number(unitLength->convert(x,currentUnitTransX,"m"),'g',9));
//    request->append(prefixName+"Z0_GRTarget",       QString::number(unitLength->convert(z,currentUnitTransX,"m"),'g',9));
//    request->append(prefixName+"SKS_GRTarget",      QString::number(sks));
//    //request->append(prefixName+"Beg_num_target",QString::number(unitAngle->convert(teta,currentUnitTransTeta,"deg")));
}

void GroundObj::slotIsModifyPosition()
{
    slotIsModifyPsi();
    slotEnterLeaveCur(true);
}
void GroundObj::slotLonToX(double value)
{
    lon=value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom-1,pX,pZ);
    setPosC(pX,pZ);
}

void GroundObj::slotLatToZ(double value)
{
    lat=value;
    int pX,pZ;
    latLongToPixelXY(lat,lon,zoom - 1,pX,pZ);
    setPosC(pX,pZ);
}
void GroundObj::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    pixelXYToLatLong(p.x(),p.y(),zoom-1,lat,lon);

    formSetting->setLat(lat);
    formSetting->setLon(lon);

    TGeoPoint pt(lat,lon);
    emit isModifyPosition(p,pt);

    slotIsModifyPsi();
    slotEnterLeaveCur(true);
    ObjectGraphNode::mouseMoveEvent(event);
}
void GroundObj::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    slotEnterLeaveCur(true);
}
void GroundObj::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    slotEnterLeaveCur(false);
}
void GroundObj::slotEnterLeaveCur(bool flag)
{
    QLineF line(aircraft->transformOriginPoint(),
                this->mapFromItem(aircraft,aircraft->transformOriginPoint()));
    lineToAircraft->setLine(line);
    lineToAircraft->setVisible(flag);
    colorItemD->setPos(line.x1()+(line.x2()-line.x1())/2.0,
                                5+line.y1()+(line.y2()-line.y1())/2.0);
    colorItemD->setRowText("d="+QString::number(d)+tr(" м"),0);
    colorItemD->setVisible(flag);

}
void AerodromObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button()==Qt::RightButton)
    {
        /*formSetting->setV(vc);
        formSetting->setH(y);
        formSetting->setPsi(psi);
        formSetting->setTeta(teta);*/

        formSetting->show();
    }
    ObjectGraphNode::mousePressEvent(event);
}
void AerodromObject::slotIsModifyPsi()
{
    QPointF centerRotate=itemSvg->transformOriginPoint();
    QPointF pTar=aircraft->mapFromItem(this,centerRotate);

    pTar=pTar-itemSvg->transformOriginPoint();

    d=distanceSphere(aircraft->lon,aircraft->lat,lon,lat);
    fi=atan2(pTar.x(),-pTar.y())*180.0/M_PI;

    fi=check180(fi-KursToPsiGrad(aircraft->psi));

    //formSetting->setFi(fi);
    //formSetting->setD(d);
}
void AerodromObject::slotIsModifyPosition()
{
    slotIsModifyPsi();
    slotEnterLeaveCur(true);
    //! отрисовка прямой соединяющей вертолет и объект
}
void AerodromObject::slotEnterLeaveCur(bool flag)
{
    QLineF line(aircraft->transformOriginPoint(),
                this->mapFromItem(aircraft,aircraft->transformOriginPoint()));
    lineToAircraft->setLine(line);
    lineToAircraft->setVisible(flag);

    colorItemD->setPos(line.x1()+(line.x2()-line.x1())/2.0,
                                5+line.y1()+(line.y2()-line.y1())/2.0);
    colorItemD->setRowText("d="+QString::number(d)+tr(" м"),0);
    colorItemD->setVisible(flag);
    //lineToAircraftTextD->setVisible(flag);
}
void AerodromObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    slotEnterLeaveCur(true);
    ObjectGraphNode::hoverEnterEvent(event);
}
void AerodromObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    slotEnterLeaveCur(false);
    ObjectGraphNode::hoverLeaveEvent(event);
}
void AerodromObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToItem(map,this->transformOriginPoint());

//    xMap=p.x();
//    zMap=p.y();

    pixelXYToLatLong(posC().x(),posC().y(),zoom-1,lat,lon);
    //baseItem->mapping->mappingToLatLon(xMap,zMap,lon,lat);

    //formSetting->setLat(lat);
    //formSetting->setLon(lon);


    TGeoPoint pt(lat,lon);
    emit isModifyPosition(p,pt);

    slotIsModifyPsi();
    slotEnterLeaveCur(true);
    ObjectGraphNode::mouseMoveEvent(event);
}
void RouteObject::refreshLine()
{
    QPointF pointEnd=mapFromItem(itemSvg,itemSvg->transformOriginPoint());
    QPointF pointStart=mapFromItem(routeRight->itemSvg,routeRight->itemSvg->transformOriginPoint());
    QLineF line(pointStart,pointEnd);
    lineToRight->setLine(line);
    //lineToRight->setVisible(flag);

    colorItemD->setPos(line.x1()+(line.x2()-line.x1())/2.0,
                       5+line.y1()+(line.y2()-line.y1())/2.0);
    colorItemD->setRowText("d="+QString::number(d)+tr(" метров"),0);
    //colorItemD->setVisible(flag);

}
void RouteObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p=mapToScene(itemSvg->transformOriginPoint());
    pixelXYToLatLong(p,zoom - 1,lat,lon);

    refresh();
    ObjectGraphNode::mouseMoveEvent(event);
}
void RouteObject::refresh()
{
    if(routeRight != nullptr)
    {
        calcD();
        refreshLine();
    }
    if(routeLeft != nullptr)
    {
        routeLeft->calcD();
        routeLeft->refreshLine();
    }
    if(routeRight == nullptr)
    {
        colorItemSumD->setRowText(tr("Общее расстояние: ")+QString::number(calcAllRoute())+tr(" метров"),0);
        colorItemSumD->setVisible(true);
    }
}
double RouteObject::calcAllRoute()
{
    double di=0.0;
    RouteObject *tempRoute=routeLeft;
    while(tempRoute!=nullptr)
    {
        di+=tempRoute->dist();
        tempRoute=tempRoute->routeLeft;
    }

    return di;
}
void RouteObject::calcD()
{
    //! дальность
    d = QPointF::dotProduct(posC(),routeRight->posC()) * groundResolution(lat,zoom - 1);
}

void CloudObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    pixelXYToLatLong(p.x(),p.y(),zoom - 1,lat,lon);

    TGeoPoint pt(lat,lon);
    emit isModifyPosition(pos(),pt);

    //slotIsModifyPsi();
    //slotEnterLeaveCur(true);
    ObjectGraphNode::mouseMoveEvent(event);
}
void FogObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF p = mapToScene(mapFromItem(itemSvg,itemSvg->transformOriginPoint()));
    pixelXYToLatLong(p.x(),p.y(),zoom - 1,lat,lon);

    TGeoPoint pt(lat,lon);
    emit isModifyPosition(pos(),pt);

    //slotIsModifyPsi();
    //slotEnterLeaveCur(true);
    ObjectGraphNode::mouseMoveEvent(event);
}
}
