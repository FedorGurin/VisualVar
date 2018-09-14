#ifndef FORMFILEEDITOR_H
#define FORMFILEEDITOR_H

#include <QWidget>

namespace Ui {
class FormFileEditor;
}

class FormFileEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormFileEditor(QWidget *parent = 0);
    ~FormFileEditor();
    
private:
    Ui::FormFileEditor *ui;
};

#endif // FORMFILEEDITOR_H
