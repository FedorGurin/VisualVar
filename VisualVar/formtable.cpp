#include "formtable.h"
#include "ui_formtable.h"

FormTable::FormTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTable)
{
    ui->setupUi(this);
}

FormTable::~FormTable()
{
    delete ui;
}

void FormTable::changeEvent(QEvent *e)
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
