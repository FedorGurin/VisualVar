#include "airObj.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"

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

    connectToObj(aircraft);

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
