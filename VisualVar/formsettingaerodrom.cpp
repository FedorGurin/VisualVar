#include "formsettingaerodrom.h"
#include "ui_formsettingaerodrom.h"

FormSettingAerodrom::FormSettingAerodrom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingAerodrom)
{
    ui->setupUi(this);
}

FormSettingAerodrom::~FormSettingAerodrom()
{
    delete ui;
}
