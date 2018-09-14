#include "gview.h"

GView::GView(QGraphicsScene * scene, QWidget * parent):QGraphicsView(scene,parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
//void GView::wheelEvent ( QWheelEvent * wheelEvent)
//{
//    if(wheelEvent->)

//}
void GView::resizeEvent(QResizeEvent * event)
{
    emit sigResize();
    QGraphicsView::resizeEvent(event);
}

void GView::mouseMoveEvent(QMouseEvent * mouseEvent)
{
    emit movePos(mouseEvent->pos());
    QGraphicsView::mouseMoveEvent(mouseEvent);
}
