#include "formAirTarget.h"
#include "ui_formsettingforairtarget.h"
#include "../globalFunc/gl_func.h"
FormSettingForAirTarget::FormSettingForAirTarget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingForAirTarget)
{
    ui->setupUi(this);

    ui->comboBoxType->clear();
    prCodeLength=1;

    ui->lineEditLat->setInputMask(tr("000°00'00.00\""));
    ui->lineEditLon->setInputMask(tr("000°00'00.00\""));

    connect(ui->dialH,      SIGNAL(valueChanged(int)),this,SLOT(slotH(int)));
    connect(ui->dialV,      SIGNAL(valueChanged(int)),this,SLOT(slotV(int)));
    connect(ui->dialPsi,    SIGNAL(valueChanged(int)),this,SLOT(slotPsi(int)));
    connect(ui->dialFi,     SIGNAL(valueChanged(int)),this,SLOT(slotFi(int)));
    connect(ui->dialTeta,   SIGNAL(valueChanged(int)),this,SLOT(slotTeta(int)));

    connect(ui->doubleSpinBoxTeta,  SIGNAL(valueChanged(double)),this,SLOT(slotTetaD(double)));
    connect(ui->doubleSpinBoxFi,    SIGNAL(valueChanged(double)),this,SLOT(slotFiD(double)));
    connect(ui->doubleSpinBoxPsi,   SIGNAL(valueChanged(double)),this,SLOT(slotPsiD(double)));
    connect(ui->doubleSpinBoxH,     SIGNAL(valueChanged(double)),this,SLOT(slotHD(double)));
    connect(ui->doubleSpinBoxV,     SIGNAL(valueChanged(double)),this,SLOT(slotVD(double)));
    connect(ui->checkBoxEnable,     SIGNAL(clicked(bool)),this,SLOT(slotEnable(bool)));
    connect(ui->comboBoxType,       SIGNAL(currentIndexChanged(int)),this,SLOT(slotCode(int)));

    connect(ui->lineEditLat,SIGNAL(textEdited(QString)),this,SLOT(slotLat(QString)));
    connect(ui->lineEditLon,SIGNAL(textEdited(QString)),this,SLOT(slotLon(QString)));

    connect(ui->doubleSpinBoxLat,SIGNAL(valueChanged(double)),this,SLOT(slotLatDouble(double)));
    connect(ui->doubleSpinBoxLon,SIGNAL(valueChanged(double)),this,SLOT(slotLonDouble(double)));

}
void FormSettingForAirTarget::setPrCodeLength(bool value)
{
    prCodeLength=value;

    if(value==false)
    {
        ui->comboBoxType->setCurrentIndex(0);
    }
}
void FormSettingForAirTarget::setLonDouble(double value)
{
    ui->doubleSpinBoxLon->setValue(value);
}
void FormSettingForAirTarget::setLatDouble(double value)
{
    ui->doubleSpinBoxLat->setValue(value);
}
void FormSettingForAirTarget::setTeta(double value)
{
    ui->doubleSpinBoxTeta->setValue(value);
}
QString FormSettingForAirTarget::currentMessureSpeed()
{
    return ui->comboBoxSpeed->currentText();
}

QString FormSettingForAirTarget::currentMessureAlt()
{
    return ui->comboBoxAlt->currentText();
}
void FormSettingForAirTarget::slotTeta(int value)
{
    double tempValue=value;
    ui->doubleSpinBoxTeta->setValue(value);

    //messureUnits->toStandartUnitAngle(value,ui->comboBoxTeta->currentText(),tempValue);
    emit signalTeta(tempValue);
}
void FormSettingForAirTarget::slotTetaD(double value)
{
    double tempValue=value;
    ui->dialTeta->setValue(value);

    //messureUnits->toStandartUnitAngle(value,ui->comboBoxTeta->currentText(),tempValue);
    emit signalTeta(tempValue);
}
void FormSettingForAirTarget::slotLatDouble(double value)
{
    emit signalLat(value);
    setLat(value);
}
void FormSettingForAirTarget::slotLonDouble(double value)
{
    emit signalLon(value);
    setLon(value);
}
QString FormSettingForAirTarget::currentTypeObject()
{
    return ui->comboBoxType->currentText();
}

FormSettingForAirTarget::~FormSettingForAirTarget()
{
    delete ui;
}
double FormSettingForAirTarget::minV()
{
    return ui->dialV->minimum();
}
double FormSettingForAirTarget::maxV()
{
    return ui->dialV->maximum();
}
void FormSettingForAirTarget::setV(double value)
{
    ui->dialV->setValue(value);
}
void FormSettingForAirTarget::setFi(double value)
{
    ui->dialFi->setValue(value);
    ui->doubleSpinBoxFi->setValue(value);
}
void FormSettingForAirTarget::setPsi(double value)
{
    ui->dialPsi->setValue(value);
}
void FormSettingForAirTarget::setH(double value)
{
    ui->dialH->setValue(value);
}
void FormSettingForAirTarget::setD(double value)
{
    ui->doubleSpinBoxD->setValue(value);
}
///////////////////////////////////////////////////////////////////////
void FormSettingForAirTarget::slotV(int value)
{
    ui->doubleSpinBoxV->setValue(value);
    emit signalV(value);
}
void FormSettingForAirTarget::slotFi(int value)
{
    ui->doubleSpinBoxFi->setValue(value);
    emit signalFi(value);
}
void FormSettingForAirTarget::slotPsi(int value)
{
    ui->doubleSpinBoxPsi->setValue(value);
    emit signalPsi(value);
}
void FormSettingForAirTarget::slotH(int value)
{
    ui->doubleSpinBoxH->setValue(value);
    emit signalH(value);
}
void FormSettingForAirTarget::slotFiD(double value)
{
    ui->dialFi->setValue(value);
    emit signalFi(value);
}

void FormSettingForAirTarget::slotVD(double value)
{
    ui->doubleSpinBoxV->setValue(value);
    emit signalV(value);
}

void FormSettingForAirTarget::slotPsiD(double value)
{
    ui->doubleSpinBoxPsi->setValue(value);
    emit signalPsi(value);
}

void FormSettingForAirTarget::slotHD(double value)
{
    ui->doubleSpinBoxH->setValue(value);
    emit signalH(value);
}
void FormSettingForAirTarget::slotEnable(bool value)
{
    emit signalEnable(value);
}

void FormSettingForAirTarget::slotD(double value)
{
    emit signalD(value);
}
void FormSettingForAirTarget::slotCode(int c)
{
    if(prCodeLength==1 && c>0 && c<=listObjectVisAir.size())
    {
        emit signalCode(listObjectVisAir[c-1]->code,listObjectVisAir[c-1]->name);
    }
    if(c>listObjectVisAir.size())
    {
        emit signalCode(currentCode,currentCodeStr);
    }
}


void FormSettingForAirTarget::setEnableTarget(bool value)
{
    if(value==true)
        ui->checkBoxEnable->setCheckState(Qt::Checked);
    else
        ui->checkBoxEnable->setCheckState(Qt::Unchecked);
}

void FormSettingForAirTarget::setListObjectVis(QList<TObjectVis*> &list)
{
    listObjectVisAir=list;
    ui->comboBoxType->clear();
    ui->comboBoxType->addItem(tr("Вручную(в метрах)"));

    for(int i=0;i<listObjectVisAir.size();i++)
    {
        ui->comboBoxType->addItem(listObjectVisAir[i]->name);
    }
    ui->comboBoxType->addItem(tr("Неизвестный код"));
}
void FormSettingForAirTarget::setCode(int code)
{
    currentCode=code;

    for(int i=0;i<listObjectVisAir.size();i++)
    {
        if(listObjectVisAir[i]->code==code)
        {
            ui->comboBoxType->setCurrentIndex(i+1);
            currentCodeStr=ui->comboBoxType->itemText(i+1);
            return;
        }
    }

    //! если код не обнаружен, то выставляем "неизвестный код" и не выдаем сигнал
    if(ui->comboBoxType->count()>0)
        ui->comboBoxType->setCurrentIndex(ui->comboBoxType->count()-1);

    currentCodeStr=ui->comboBoxType->itemText(ui->comboBoxType->count()-1);
}
QString FormSettingForAirTarget::typeObject(int code)
{
    for(int i=0;i<listObjectVisAir.size();i++)
    {
        if(listObjectVisAir[i]->code==code)
        {

            return listObjectVisAir[i]->name;
        }
    }
    QString unknownCode=tr("неизвестный код");
    return unknownCode;
}
void FormSettingForAirTarget::slotLat(QString value)
{
    double v=geo_string_to_double(value);
    emit signalLat(v);
    setLatDouble(v);
}
void FormSettingForAirTarget::slotLon(QString value)
{
    double v=geo_string_to_double(value);
    emit signalLon(v);
    setLonDouble(v);
}
void FormSettingForAirTarget::setLat(double value)
{
    ui->lineEditLat->setText(geo_double_to_string(value));
    setLatDouble(value);
}
void FormSettingForAirTarget::setLon(double value)
{
    ui->lineEditLon->setText(geo_double_to_string(value));
    setLonDouble(value);
}
void FormSettingForAirTarget::slotComboMessureV(int)
{

}
void FormSettingForAirTarget::changeEvent(QEvent *e)
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
