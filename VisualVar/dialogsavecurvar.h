#ifndef DIALOGSAVECURVAR_H
#define DIALOGSAVECURVAR_H

#include <QDialog>

namespace Ui {
    class DialogSaveCurVar;
}

class DialogSaveCurVar : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSaveCurVar(QWidget *parent = 0);
    ~DialogSaveCurVar();
    bool ok;
signals:
    void signalOk(bool);
public slots:
    void slotSave();
    void slotClose();
private:
    Ui::DialogSaveCurVar *ui;

};

#endif // DIALOGSAVECURVAR_H
