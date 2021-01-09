#include "groundObj.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"

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
