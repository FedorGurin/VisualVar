#include "formfileeditor.h"
#include "ui_formfileeditor.h"

FormFileEditor::FormFileEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFileEditor)
{
    ui->setupUi(this);
}

FormFileEditor::~FormFileEditor()
{
    delete ui;
}
