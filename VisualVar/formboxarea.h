#ifndef FORMBOXAREA_H
#define FORMBOXAREA_H

#include <QWidget>

namespace Ui {
    class FormBoxArea;
}

class FormBoxArea : public QWidget
{
    Q_OBJECT

public:
    explicit FormBoxArea(QWidget *parent = 0);
    ~FormBoxArea();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FormBoxArea *ui;
};

#endif // FORMBOXAREA_H
