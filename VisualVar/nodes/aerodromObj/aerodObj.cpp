#include "aerodObj.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"


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
