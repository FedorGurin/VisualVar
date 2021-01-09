#include "formsettingvor.h"
#include "ui_formsettingvor.h"

FormSettingBeaconVOR::FormSettingBeaconVOR(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingBeaconVOR)
{
    ui->setupUi(this);
}

FormSettingBeaconVOR::~FormSettingBeaconVOR()
{
    delete ui;
}
