#include "formsettingcloud.h"
#include "ui_formsettingcloud.h"

FormSettingCloud::FormSettingCloud(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingCloud)
{
    ui->setupUi(this);
}

FormSettingCloud::~FormSettingCloud()
{
    delete ui;
}
