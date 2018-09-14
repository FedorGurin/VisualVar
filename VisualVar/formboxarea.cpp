#include "formboxarea.h"
#include "ui_formboxarea.h"

FormBoxArea::FormBoxArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBoxArea)
{
    ui->setupUi(this);
}

FormBoxArea::~FormBoxArea()
{
    delete ui;
}

void FormBoxArea::changeEvent(QEvent *e)
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
