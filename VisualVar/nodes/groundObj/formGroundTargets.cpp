#include "formGroundTargets.h"
#include "ui_formsettingforgroundtargets.h"
#include "../globalFunc/gl_func.h"
FormSettingForGroundTargets::FormSettingForGroundTargets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingForGroundTargets)
{
    ui->setupUi(this);

    ui->comboBox->clear();
    prCodeLength=1;

    ui->lineEditLat->setInputMask(tr("000°00'00.00\""));
    ui->lineEditLon->setInputMask(tr("000°00'00.00\""));

    connect(ui->dialV,          SIGNAL(valueChanged(int)),this,         SLOT(slotV(int)));
    connect(ui->dialPsi,        SIGNAL(valueChanged(int)),this,         SLOT(slotPsi(int)));
    connect(ui->checkBoxEnable, SIGNAL(clicked(bool)),this,             SLOT(slotEnable(bool)));
    connect(ui->comboBox,       SIGNAL(currentIndexChanged(int)),this,  SLOT(slotCode(int)));

    connect(ui->doubleSpinBoxV,SIGNAL(valueChanged(double)),this,SLOT(slotVD(double)));
    connect(ui->doubleSpinBoxPsi,SIGNAL(valueChanged(double)),this,SLOT(slotPsiD(double)));

    connect(ui->lineEditLat,SIGNAL(textEdited(QString)),this,SLOT(slotLat(QString)));
    connect(ui->lineEditLon,SIGNAL(textEdited(QString)),this,SLOT(slotLon(QString)));

    connect(ui->doubleSpinBoxLat,SIGNAL(valueChanged(double)),this,SLOT(slotLatDouble(double)));
    connect(ui->doubleSpinBoxLon,SIGNAL(valueChanged(double)),this,SLOT(slotLonDouble(double)));
}

void FormSettingForGroundTargets::setLonDouble(double value)
{
    ui->doubleSpinBoxLon->setValue(value);
}
void FormSettingForGroundTargets::setLatDouble(double value)
{
    ui->doubleSpinBoxLat->setValue(value);
}
void FormSettingForGroundTargets::slotLatDouble(double value)
{
    emit signalLat(value);
    setLat(value);
}
void FormSettingForGroundTargets::slotLonDouble(double value)
{
    emit signalLon(value);
    setLon(value);
}
double FormSettingForGroundTargets::minV()
{
    return ui->dialV->minimum();
}
double FormSettingForGroundTargets::maxV()
{
    return ui->dialV->maximum();
}
void FormSettingForGroundTargets::slotV(int value)
{
    ui->doubleSpinBoxV->setValue(value);
    emit signalV(value);
}
void FormSettingForGroundTargets::slotEnable(bool value)
{
    emit signalEnable(value);
}
void FormSettingForGroundTargets::slotPsi(int value)
{
    ui->doubleSpinBoxPsi->setValue(value);
    emit signalPsi(value);
}
void FormSettingForGroundTargets::slotCode(int c)
{
    if(prCodeLength==1 && c>0 && c<=listObjectVisGround.size())
    {
        emit signalCode(listObjectVisGround[c-1]->code,listObjectVisGround[c-1]->name);
    }
    if(c>listObjectVisGround.size())
    {
        emit signalCode(currentCode,currentCodeStr);
    }

}
void FormSettingForGroundTargets::slotVD(double value)
{
    ui->dialV->setValue(value);
    emit signalV(value);
}

void FormSettingForGroundTargets::slotPsiD(double value)
{
    ui->dialPsi->setValue(value);
    emit signalPsi(value);
}
QString FormSettingForGroundTargets::currentTypeObject()
{
    return ui->comboBox->currentText();
}
void FormSettingForGroundTargets::setListObjectVis(QList<TObjectVis*> &list)
{
    listObjectVisGround=list;
    ui->comboBox->clear();
    ui->comboBox->addItem(tr("Вручную(в метрах)"));
    for(int i=0;i<listObjectVisGround.size();i++)
    {
        ui->comboBox->addItem(listObjectVisGround[i]->name);
    }    
    ui->comboBox->addItem(tr("Неизвестный код"));

    //slotCode(1);
}
void FormSettingForGroundTargets::setV(double value)
{
    ui->doubleSpinBoxV->setValue(value);
    slotV(value);
}

void FormSettingForGroundTargets::setPsi(double value)
{
    ui->doubleSpinBoxPsi->setValue(value);
    slotPsi(value);
}
void FormSettingForGroundTargets::setPrCodeLength(bool value)
{
    prCodeLength=value;
    if(value==false)
    {
        ui->comboBox->setCurrentIndex(0);
    }
}
void FormSettingForGroundTargets::setCode(int code)
{
    int j = 0;
    currentCode = code;

    for(auto i:listObjectVisGround)
    {
        if(i->code == code)
        {
            ui->comboBox->setCurrentIndex(j + 1);
            currentCodeStr = ui->comboBox->itemText(j + 1);
            return;
        }
        j++;
    }

    //! если код не обнаружен, то выставляем "неизвестный код" и не выдаем сигнал
    if(ui->comboBox->count()>0)
        ui->comboBox->setCurrentIndex(ui->comboBox->count()-1);

    currentCodeStr=ui->comboBox->itemText(ui->comboBox->count()-1);
}
void FormSettingForGroundTargets::slotLat(QString value)
{
    double v=geo_string_to_double(value);
    emit signalLat(v);
    setLatDouble(v);
}
void FormSettingForGroundTargets::slotLon(QString value)
{
    double v=geo_string_to_double(value);
    emit signalLon(v);
    setLonDouble(v);
}

void FormSettingForGroundTargets::setLat(double value)
{
    ui->lineEditLat->setText(geo_double_to_string(value));
    setLatDouble(value);
}
void FormSettingForGroundTargets::setLon(double value)
{
    ui->lineEditLon->setText(geo_double_to_string(value));
    setLonDouble(value);
}

void FormSettingForGroundTargets::setX(double value)
{
    ui->doubleSpinBoxX->setValue(value);
}

void FormSettingForGroundTargets::setZ(double value)
{
    ui->doubleSpinBoxZ->setValue(value);
}
void FormSettingForGroundTargets::setEnableTarget(bool value)
{
    if(value==true)ui->checkBoxEnable->setCheckState(Qt::Checked);
    else ui->checkBoxEnable->setCheckState(Qt::Unchecked);
}
void FormSettingForGroundTargets::setDefaultObjectVis()
{
    ui->comboBox->setCurrentIndex(0);
}

FormSettingForGroundTargets::~FormSettingForGroundTargets()
{
    delete ui;
}
