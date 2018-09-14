#ifndef FORMTABLE_H
#define FORMTABLE_H

#include <QWidget>

namespace Ui {
    class FormTable;
}

class FormTable : public QWidget {
    Q_OBJECT
public:
    FormTable(QWidget *parent = 0);
    ~FormTable();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FormTable *ui;
};

#endif // FORMTABLE_H
