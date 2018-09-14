#include "dialogsavecurvar.h"
#include "ui_dialogsavecurvar.h"

DialogSaveCurVar::DialogSaveCurVar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSaveCurVar)
{
    ui->setupUi(this);
    ok=false;
    connect(ui->pushButtonSave,SIGNAL(clicked()),this,SLOT(slotSave()));
    connect(ui->pushButtonClose,SIGNAL(clicked()),this,SLOT(slotClose()));
}
void DialogSaveCurVar::slotSave()
{
    emit signalOk(true);
    ok=true;
    hide();
}
void DialogSaveCurVar::slotClose()
{
    emit signalOk(false);
    ok=false;
    hide();
}

DialogSaveCurVar::~DialogSaveCurVar()
{
    delete ui;
}
