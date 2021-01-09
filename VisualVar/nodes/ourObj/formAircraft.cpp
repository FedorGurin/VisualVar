#include "formAircraft.h"
#include "ui_formsettingforaircraft.h"
#include "../globalFunc/gl_func.h"
FormSettingForAircraft::FormSettingForAircraft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingForAircraft)
{
    ui->setupUi(this);

    //setWindowFlags(Qt::ToolTip);
    messureUnits=new MessureUnits(this);
    ui->lineEditLat->setInputMask(tr("000°00'00.00\""));
    ui->lineEditLon->setInputMask(tr("000°00'00.00\""));

    messureCurrentSpeed=ui->comboBoxSpeed->currentText();
    messureCurrentAlt=ui->comboBoxAlt->currentText();
    messureCurrentPsi=ui->comboBoxPsi->currentText();
    messureCurrentTeta=ui->comboBoxTeta->currentText();

    connect(ui->dialH,SIGNAL(valueChanged(int)),this,SLOT(slotH(int)));
    connect(ui->dialV,SIGNAL(valueChanged(int)),this,SLOT(slotV(int)));
    connect(ui->dialPsi,SIGNAL(valueChanged(int)),this,SLOT(slotPsi(int)));
    connect(ui->dialTeta,SIGNAL(valueChanged(int)),this,SLOT(slotTeta(int)));

//    connect(ui->doubleSpinBoxH,SIGNAL(valueChanged(double)),this,SLOT(slotHD(double)));
//    connect(ui->doubleSpinBoxV,SIGNAL(valueChanged(double)),this,SLOT(slotVD(double)));
//    connect(ui->doubleSpinBoxPsi,SIGNAL(valueChanged(double)),this,SLOT(slotPsiD(double)));
//    connect(ui->doubleSpinBoxTeta,SIGNAL(valueChanged(double)),this,SLOT(slotTetaD(double)));

   // connect(ui->lineEditLat,SIGNAL(textEdited(QString)),this,SLOT(slotLat(QString)));
   // connect(ui->lineEditLon,SIGNAL(textEdited(QString)),this,SLOT(slotLon(QString)));
    connect(ui->lineEditLat,SIGNAL(editingFinished()),this,SLOT(slotLat()));
    connect(ui->lineEditLon,SIGNAL(editingFinished()),this,SLOT(slotLon()));


    connect(ui->doubleSpinBoxLat,SIGNAL(valueChanged(double)),this,SLOT(slotLatDouble(double)));
    connect(ui->doubleSpinBoxLon,SIGNAL(valueChanged(double)),this,SLOT(slotLonDouble(double)));
    connect(ui->comboBoxSpeed,SIGNAL(activated(int)),this,SLOT(slotComboBoxSpeed(int)));
    connect(ui->comboBoxAlt,SIGNAL(activated(int)),this,SLOT(slotComboBoxAlt(int)));
    connect(ui->comboBoxPsi,SIGNAL(activated(int)),this,SLOT(slotComboBoxPsi(int)));
    connect(ui->comboBoxTeta,SIGNAL(activated(int)),this,SLOT(slotComboBoxTeta(int)));

    connect(ui->radioButtonUnt,SIGNAL(clicked(bool)),this,SLOT(slotRadioButtonTan(bool)));
    connect(ui->radioButtonVy,SIGNAL(clicked(bool)),this,SLOT(slotRadioButtonUnt(bool)));

}
FormSettingForAircraft::~FormSettingForAircraft()
{
    delete ui;
}
double FormSettingForAircraft::minV()
{
    double value     = ui->dialV->minimum();
    double tempValue = value;
    messureUnits->toStandartUnitSpeed(value,ui->comboBoxSpeed->currentText(),tempValue);
    return tempValue;
}
double FormSettingForAircraft::maxV()
{
    double value     = ui->dialV->maximum();
    double tempValue = value;
    messureUnits->toStandartUnitSpeed(value,ui->comboBoxSpeed->currentText(),tempValue);
    return tempValue;
}
void FormSettingForAircraft::setV(double value)
{
    ui->dialV->setValue(value);
}
void FormSettingForAircraft::setVy(double)
{

}
void FormSettingForAircraft::setTeta(double value)
{
    ui->doubleSpinBoxTeta->setValue(value);
}
void FormSettingForAircraft::setPsi(double value)
{
    ui->dialPsi->setValue(value);
}
void FormSettingForAircraft::setH(double value)
{
    //ui->doubleSpinBoxH->setValue(value);
    ui->dialH->setValue(value);
}
void FormSettingForAircraft::setUntOtVy(bool b)
{
    if(b==true)
    {
        ui->radioButtonUnt->setChecked(!b);
        ui->radioButtonVy->setChecked(b);
    }
    else
    {
        ui->radioButtonUnt->setChecked(!b);
        ui->radioButtonVy->setChecked(b);
    }
}

void FormSettingForAircraft::slotV(int value)
{
    double tempValue=value;
    ui->doubleSpinBoxV->setValue(value);

    messureUnits->toStandartUnitSpeed(value,ui->comboBoxSpeed->currentText(),tempValue);
    emit signalV(tempValue);
}
void FormSettingForAircraft::slotTeta(int value)
{
     double tempValue=value;
    ui->doubleSpinBoxTeta->setValue(value);

    messureUnits->toStandartUnitAngle(value,ui->comboBoxTeta->currentText(),tempValue);
    emit signalTeta(tempValue);
}
void FormSettingForAircraft::slotPsi(int value)
{
    double tempValue=value;
    ui->doubleSpinBoxPsi->setValue(value);

    messureUnits->toStandartUnitAngle(value,ui->comboBoxPsi->currentText(),tempValue);
    emit signalPsi(tempValue);
}
void FormSettingForAircraft::slotH(int value)
{
    double tempValue=value;
    ui->doubleSpinBoxH->setValue(value);

    messureUnits->toStandartUnitAlt(value,ui->comboBoxAlt->currentText(),tempValue);
    emit signalH(tempValue);
}
void FormSettingForAircraft::slotVD(double value)
{
    double tempValue=value;
    ui->dialV->setValue(value);
    messureUnits->toStandartUnitAlt(value,ui->comboBoxAlt->currentText(),tempValue);
    emit signalV(tempValue);
}
void FormSettingForAircraft::slotTetaD(double value)
{
    double tempValue=value;
    ui->dialTeta->setValue(value);

    messureUnits->toStandartUnitAngle(value,ui->comboBoxTeta->currentText(),tempValue);
    emit signalTeta(tempValue);
}
void FormSettingForAircraft::slotPsiD(double value)
{
    double tempValue=value;
    ui->dialPsi->setValue(value);
    messureUnits->toStandartUnitAngle(value,ui->comboBoxPsi->currentText(),tempValue);
    emit signalPsi(tempValue);
}
void FormSettingForAircraft::slotHD(double value)
{
    double tempValue=value;
    ui->dialH->setValue(value);
    messureUnits->toStandartUnitAlt(value,ui->comboBoxAlt->currentText(),tempValue);
    emit signalH(tempValue);
}
void FormSettingForAircraft::setLat(double value)
{
    ui->lineEditLat->setText(geo_double_to_string(value));
    setLatDouble(value);
}
void FormSettingForAircraft::setLon(double value)
{
    ui->lineEditLon->setText(geo_double_to_string(value));
    setLonDouble(value);
}
void FormSettingForAircraft::slotLat()
{
    QString value=ui->lineEditLat->text();
    double v=geo_string_to_double(value);
    emit signalLat(v,value);
    setLatDouble(v);
}
void FormSettingForAircraft::slotLon()
{
    QString value=ui->lineEditLon->text();
    double v=geo_string_to_double(value);
    emit signalLon(v,value);
    setLonDouble(v);
}
void FormSettingForAircraft::setLonDouble(double value)
{
    ui->doubleSpinBoxLon->setValue(value);
}
void FormSettingForAircraft::setLatDouble(double value)
{
    ui->doubleSpinBoxLat->setValue(value);
}
void FormSettingForAircraft::slotLatDouble(double value)
{
    setLat(value);
    emit signalLat(value,ui->lineEditLat->text());
//    qDebug("value=%s\n",ui->lineEditLat->text().toAscii().data());
}
void FormSettingForAircraft::slotLonDouble(double value)
{
    emit signalLon(value,ui->lineEditLon->text());
    setLon(value);
}
void FormSettingForAircraft::slotRadioButtonTan(bool flag)
{
    if(flag==true) emit signalVy(false);
}
void FormSettingForAircraft::slotRadioButtonUnt(bool flag)
{
    if(flag==true) emit signalVy(true);
}
QString FormSettingForAircraft::currentMessureSpeed()
{
    return ui->comboBoxSpeed->currentText();
}

QString FormSettingForAircraft::currentMessureAlt()
{
    return ui->comboBoxAlt->currentText();
}

QString FormSettingForAircraft::currentMessurePsi()
{
    return ui->comboBoxPsi->currentText();
}

QString FormSettingForAircraft::currentMessureTeta()
{
    return ui->comboBoxTeta->currentText();
}
double FormSettingForAircraft::currentVy()
{
    return ui->doubleSpinBoxTeta->value();
}

QString FormSettingForAircraft::currentMessureVy()
{
    return ui->comboBoxTeta->currentText();
}
double FormSettingForAircraft::currentV()
{
    return ui->doubleSpinBoxV->value();
}
double FormSettingForAircraft::currentTeta()
{
    return ui->doubleSpinBoxTeta->value();
}
double FormSettingForAircraft::currentPsi()
{
    return ui->doubleSpinBoxPsi->value();
}
double FormSettingForAircraft::currentH()
{
    return ui->doubleSpinBoxH->value();
}

void FormSettingForAircraft::slotComboBoxSpeed(int)
{
    double min,max;
    QString value=ui->comboBoxSpeed->currentText();
    messureUnits->minmaxSpeed(value,min,max);

    double newValue=ui->doubleSpinBoxV->value();
    messureUnits->convertSpeed(newValue,
                               ui->comboBoxSpeed->currentText(),
                               messureCurrentSpeed,
                               newValue);
    ui->dialV->setMinimum(min);
    ui->dialV->setMaximum(max);
    ui->doubleSpinBoxV->setMaximum(max);
    ui->doubleSpinBoxV->setMinimum(min);

    ui->dialV->setValue(newValue);
    ui->doubleSpinBoxV->setValue(newValue);
    messureCurrentSpeed=ui->comboBoxSpeed->currentText();

}
void FormSettingForAircraft::slotComboBoxAlt(int)
{
    double min,max;
    QString value=ui->comboBoxAlt->currentText();
    messureUnits->minmaxAlt(value,min,max);

    double newValue=ui->doubleSpinBoxH->value();
    messureUnits->convertAlt(newValue,
                               ui->comboBoxAlt->currentText(),
                               messureCurrentAlt,
                               newValue);
    ui->dialH->setMinimum(min);
    ui->dialH->setMaximum(max);
    ui->dialH->setPageStep(5);
    ui->dialH->setSingleStep(1);

    ui->doubleSpinBoxH->setMaximum(max);
    ui->doubleSpinBoxH->setMinimum(min);

    ui->dialH->setValue(newValue);
    ui->doubleSpinBoxH->setValue(newValue);
    messureCurrentAlt=ui->comboBoxAlt->currentText();

}

void FormSettingForAircraft::slotComboBoxPsi(int)
{
    double min,max;
    QString value=ui->comboBoxPsi->currentText();
    messureUnits->minmaxAngle(value,min,max);

    double newValue=ui->doubleSpinBoxPsi->value();
    messureUnits->convertAngle(newValue,
                               ui->comboBoxPsi->currentText(),
                               messureCurrentPsi,
                               newValue);

    ui->dialPsi->setMinimum(min);
    ui->dialPsi->setMaximum(max);

    ui->doubleSpinBoxPsi->setMaximum(max);
    ui->doubleSpinBoxPsi->setMinimum(min);

    ui->dialPsi->setValue(newValue);
    ui->doubleSpinBoxPsi->setValue(newValue);
    messureCurrentPsi=ui->comboBoxPsi->currentText();

}

void FormSettingForAircraft::slotComboBoxTeta(int)
{
    double min,max;
    QString value=ui->comboBoxTeta->currentText();
    messureUnits->minmaxAngle(value,min,max);

    double newValue=ui->doubleSpinBoxTeta->value();
    messureUnits->convertAngle(newValue,
                               ui->comboBoxTeta->currentText(),
                               messureCurrentSpeed,
                               newValue);
    ui->dialTeta->setMinimum(min);
    ui->dialTeta->setMaximum(max);
    ui->doubleSpinBoxTeta->setMaximum(max);
    ui->doubleSpinBoxTeta->setMinimum(min);

    ui->dialTeta->setValue(newValue);
    ui->doubleSpinBoxTeta->setValue(newValue);
    messureCurrentTeta=ui->comboBoxTeta->currentText();

}

void FormSettingForAircraft::changeEvent(QEvent *e)
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
