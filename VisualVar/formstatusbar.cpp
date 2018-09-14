#include <QPalette>

#include "formstatusbar.h"
#include "ui_formstatusbar.h"
#include "../globalFunc/gl_func.h"
FormStatusBar::FormStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormStatusBar)
{
    ui->setupUi(this);
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

    if(status==ST_BAD)
    {
        QColor color(Qt::red);
        QPalette palette=ui->labelStatus->palette();

        palette.setColor(QPalette::Foreground,color);
        ui->labelStatus->setPalette(palette);
    }
    else if(status==ST_GOOD)
    {
        QColor color(Qt::blue);
        QPalette palette=ui->labelStatus->palette();

        palette.setColor(QPalette::Foreground,color);
        ui->labelStatus->setPalette(palette);
    }
    else
    {
        QColor color(Qt::black);
        QPalette palette=ui->labelStatus->palette();

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
