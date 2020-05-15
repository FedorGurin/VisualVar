#include <QPalette>

#include "formstatusbar.h"
#include "ui_formstatusbar.h"
#include "../globalFunc/gl_func.h"
FormStatusBar::FormStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormStatusBar)
{
    ui->setupUi(this);

    isConnect = -1;

    ui->labelSource->setVisible(false);
    ui->label_6->setVisible(false);
    ui->label->setVisible(false);
    ui->labelX->setVisible(false);
    ui->label_2->setVisible(false);
    ui->labelZ->setVisible(false);

    p_no_info = p_connect = p_disconnect = ui->labelStatus->palette();
    p_no_info.setColor(QPalette::Foreground,Qt::gray);
    p_connect.setColor(QPalette::Foreground,Qt::green);
    p_disconnect.setColor(QPalette::Foreground,Qt::red);

    updateConnectInfo(TRequestEvent::PROCESSED);
}
void FormStatusBar::setX(double x)
{
    ui->labelX->setText(QString::number((long)x));
}
void FormStatusBar::setZ(double z)
{
    ui->labelZ->setText(QString::number((long)z));
}
//широта
void FormStatusBar::setLat(double lat)
{
    ui->labelLat->setText(QString::number(lat));
}
//долгота
void FormStatusBar::setLon(double lon)
{
    ui->labelLon->setText(QString::number(lon));
}
void FormStatusBar::setPos(QPointF pos)
{
    ui->labelX->setText(QString::number((long)pos.x()));
    ui->labelZ->setText(QString::number((long)pos.y()));
}
void FormStatusBar::setPos(QPointF pos,TGeoPoint geo)
{
    ui->labelX->setText(QString::number((long)pos.x()));
    ui->labelZ->setText(QString::number((long)pos.y()));

    QString str1=geo_double_to_string(geo.lat);
    QString str2=geo_double_to_string(geo.lon);

    ui->labelLat->setText(tr("С ")+str1);
    ui->labelLon->setText(tr("В ")+str2);
}
void FormStatusBar::setZoom(int zoom)
{
    ui->labelZoom->setText(QString::number(zoom));
}
void FormStatusBar::setSource(QString str)
{
    ui->labelSource->setText(str);
}
void FormStatusBar::setStatusTransmit(QString name,TStatus status)
{

    if(status == ST_BAD)
    {
        QColor color(Qt::red);
        QPalette palette = ui->labelStatus->palette();

        palette.setColor(QPalette::Foreground,color);
        ui->labelStatus->setPalette(palette);
    }
    else if(status == ST_GOOD)
    {
        QColor color(Qt::blue);
        QPalette palette = ui->labelStatus->palette();

        palette.setColor(QPalette::Foreground,color);
        ui->labelStatus->setPalette(palette);
    }
    else
    {
        QColor color(Qt::black);
        QPalette palette = ui->labelStatus->palette();

        palette.setColor(QPalette::Foreground,color);
        ui->labelStatus->setPalette(palette);
    }
    ui->labelStatus->setText(name);
}

FormStatusBar::~FormStatusBar()
{
    delete ui;
}

void FormStatusBar::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FormStatusBar::updateConnectInfo(TRequestEvent::TYPE_STATUS status)
{
    if(status == TRequestEvent::PROCESSED_FAULT)
    {
        if(isConnect != 0){
            ui->connectState->setText("Отключено");
            ui->connectState->setPalette(p_disconnect);
        }
        isConnect = 0;
    }
    else if(status == TRequestEvent::PROCESSED)
    {
        if(isConnect != 1){
            ui->connectState->setText("Включено");
            ui->connectState->setPalette(p_connect);
        }
        isConnect = 1;
    }
    else
    {
        if(isConnect != -1){
            ui->connectState->setText("Нет информации");
            ui->connectState->setPalette(p_no_info);
        }
        isConnect = -1;
    }
}
