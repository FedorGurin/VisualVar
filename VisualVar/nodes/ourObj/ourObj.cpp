#include "ourObj.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"



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
