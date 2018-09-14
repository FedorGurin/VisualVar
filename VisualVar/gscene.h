#ifndef GSCENE_H
#define GSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>

class GScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GScene(QObject * parent = 0);
protected:
    virtual void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent    * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent   * mouseEvent);
signals:
    //! увеличить масштаб на один шаг
    void zoomUp();
    //! уменьшить масштаб на один шаг
    void zoomDown();
    //! перемещение курсора
    void dragMove();
    //! отпускание правой кнопки
    void sigRightMouse();
    //! текущее положение мышки в СК сцены
    void movePos(QPointF pos);
    //! нажатие левой кнопки мыши
    void clickLeftMouse();
};

#endif // GSCENE_H
