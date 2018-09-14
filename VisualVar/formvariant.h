#ifndef FORMVARIANT_H
#define FORMVARIANT_H

#include <QWidget>

namespace Ui {
    class FormVariant;
}

class FormVariant : public QWidget {
    Q_OBJECT
public:
    FormVariant(QWidget *parent = 0);
    ~FormVariant();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FormVariant *ui;
};

#endif // FORMVARIANT_H
