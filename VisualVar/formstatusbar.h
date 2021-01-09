#ifndef FORMSTATUSBAR_H
#define FORMSTATUSBAR_H

#include <QWidget>
#include "../globalFunc/gl_func.h"
#include "../mppm/CommonEngineData.h"
namespace Ui {
    class FormStatusBar;
}

class FormStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit FormStatusBar(QWidget *parent = nullptr);

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
    void updateConnectInfo(TRequestEvent::TYPE_STATUS status);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FormStatusBar *ui;
    //! информация о наличии соединения с УЦВС
    uint8_t isConnect;

    QPalette p_no_info;
    QPalette p_connect;
    QPalette p_disconnect;
};

#endif // FORMSTATUSBAR_H
