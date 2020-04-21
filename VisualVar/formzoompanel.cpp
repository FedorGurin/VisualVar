#include "formzoompanel.h"
#include "ui_formzoompanel.h"
#include <math.h>
FormZoomPanel::FormZoomPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormZoomPanel)
{
    ui->setupUi(this);
    connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(slotEvents(int)));
    connect(ui->toolButtonUp,SIGNAL(clicked()),this,SLOT(slotButtonUp()));
    connect(ui->toolButtonDown,SIGNAL(clicked()),this,SLOT(slotButtonDown()));
}
void FormZoomPanel::slotEvents(int value)
{    
    emit signalZoom(value);
}
//! увеличить масштаб
void FormZoomPanel::slotButtonUp()
{
    int value = ui->verticalSlider->value()+1;
    ui->verticalSlider->setValue(value);
    emit signalZoom(value);
}
void FormZoomPanel::setBeginValue(int z)
{
    ui->verticalSlider->setValue(z);
}
//! уменьшить масштаб
void FormZoomPanel::slotButtonDown()
{
    int value = ui->verticalSlider->value()-1;
    ui->verticalSlider->setValue(value);
    emit signalZoom(value);
}
FormZoomPanel::~FormZoomPanel()
{
    delete ui;
}
