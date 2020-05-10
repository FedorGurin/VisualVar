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
    if(mouseEvent->buttons() == Qt::RightButton)
    {
        emit sigRightMouse();
    }
    if(mouseEvent->buttons() == Qt::LeftButton)
    {
        emit clickLeftMouse();
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    signalDoubleClickMouse();
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
   // qDebug("release Event\n");
    if(mouseEvent->buttons() == Qt::RightButton)
    {
        //qDebug("rightButton\n");
        emit sigRightMouse();
    }
    else
        emit dragMove();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
        emit signalCtrlPress();
    QGraphicsScene::keyPressEvent(event);
}

void GScene::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
        emit signalCtrlRelease();
    QGraphicsScene::keyReleaseEvent(event);
}
