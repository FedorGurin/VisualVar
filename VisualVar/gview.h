#ifndef GVIEW_H
#define GVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class GView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GView(QGraphicsScene * scene, QWidget * parent=0);
protected:
    virtual void mouseMoveEvent(QMouseEvent * mouseEvent);
    virtual void resizeEvent(QResizeEvent * event);
public:
    GView();
signals:
    void movePos(QPoint pos);
    void sigResize();
};

#endif // GVIEW_H
