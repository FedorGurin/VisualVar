#ifndef FORMSTATUSBAR_H
#define FORMSTATUSBAR_H

#include <QWidget>
#include "../globalFunc/gl_func.h"
namespace Ui {
    class FormStatusBar;
}

class FormStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit FormStatusBar(QWidget *parent = 0);

    enum TStatus
    {
        ST_BAD,
        ST_GOOD,
        ST_PREPARE
    };
    ~FormStatusBar();
public slots:
    void setX(double x);
    void setZ(double z);
    void setPos(QPointF);
    void setPos(QPointF pos,TGeoPoint geo);
    // широта
    void setLat(double lat);
    //! долгота
    void setLon(double lon);
    void setZoom(int zoom);
    void setStatusTransmit(QString,TStatus status);
    void setSource(QString str);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FormStatusBar *ui;
};

#endif // FORMSTATUSBAR_H
