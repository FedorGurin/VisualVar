#include "formsettingtacan.h"
#include "ui_formsettingtacan.h"

FormSettingBeaconTACAN::FormSettingBeaconTACAN(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingBeaconTACAN)
{
    ui->setupUi(this);
}

FormSettingBeaconTACAN::~FormSettingBeaconTACAN()
{
    delete ui;
}
