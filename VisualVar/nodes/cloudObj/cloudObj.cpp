#include "cloudObj.h"

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


