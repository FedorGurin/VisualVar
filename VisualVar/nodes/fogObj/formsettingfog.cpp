#include "formsettingfog.h"
#include "ui_formfog.h"

FormFog::FormFog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFog)
{
    ui->setupUi(this);
}

FormFog::~FormFog()
{
    delete ui;
}
