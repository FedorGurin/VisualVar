#include "formsettingbeaconvor.h"
#include "ui_formsettingbeaconvor.h"

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
