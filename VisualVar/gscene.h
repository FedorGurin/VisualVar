#ifndef GSCENE_H
#define GSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>

class GScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GScene(QObject * parent = nullptr);
protected:
    virtual void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent    * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent   * mouseEvent);    
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
signals:
    //! увеличить масштаб на один шаг
    void zoomUp();
    //! уменьшить масштаб на один шаг
    void zoomDown();
    //! перемещение курсора
    void dragMove();
    //! отпускание правой кнопки
    void sigRightMouse();
    //!
    void signalDoubleClickMouse();
    //! текущее положение мышки в СК сцены
    void movePos(QPointF pos);
    //! нажатие левой кнопки мыши
    void clickLeftMouse();
    //!
    void signalCtrlPress();
    //!
    void signalCtrlRelease();
};

#endif // GSCENE_H
