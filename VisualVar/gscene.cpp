#include "gscene.h"

GScene::GScene(QObject * parent):QGraphicsScene(parent)
{
}
void GScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{
//    QGraphicsScene::wheelEvent(wheelEvent);
    if(wheelEvent->delta()>0)
    {
        emit zoomUp();
    }
    else if(wheelEvent->delta()<0)
    {
        emit zoomDown();
    }
    wheelEvent->accept();

}
void GScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    emit movePos(mouseEvent->scenePos());
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
void GScene::mousePressEvent(QGraphicsSceneMouseEvent   * mouseEvent)
{
    if(mouseEvent->buttons()==Qt::RightButton)
    {
        //qDebug("rightButton\n");
        emit sigRightMouse();
    }
    if(mouseEvent->buttons()==Qt::LeftButton)
    {
        //qDebug("rightButton\n");
        emit clickLeftMouse();
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
void GScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
   // qDebug("release Event\n");
    if(mouseEvent->buttons()==Qt::RightButton)
    {
        //qDebug("rightButton\n");
        emit sigRightMouse();
    }
    else
        emit dragMove();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
