#include "formsettingbeaconrsbn.h"
#include "ui_formsettingbeaconrsbn.h"

FormSettingBeaconRSBN::FormSettingBeaconRSBN(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingBeaconRSBN)
{
    ui->setupUi(this);
}

FormSettingBeaconRSBN::~FormSettingBeaconRSBN()
{
    delete ui;
}
