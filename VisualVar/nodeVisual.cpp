#include "nodeVisual.h"
#include <QFile>
#include <math.h>

#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./GeographicLib/geoFunctions.h"


TObjectProperty* ObjectGraphNode::unitAngle = nullptr;
TObjectProperty* ObjectGraphNode::unitSpeed = nullptr;
TObjectProperty* ObjectGraphNode::unitLength= nullptr;
TObjectProperty* ObjectGraphNode::unitExp   = nullptr;

extern GeographicLib::Geocentric* earth;






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

    //пересчитаем географические координаты в новые координаты линий
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
        //изменяем координаты линий на новые с учетом детализации
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

    //получаем координаты левого верхнего угла элемента
    QPointF result = point - dP;

    //перемещаем наш элемент
    setPos(result);

    //добавить точку в список
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
    //дальность
    d = QPointF::dotProduct(posC(),routeRight->posC()) * groundResolution(lat,zoom - 1);
}

