#ifndef FORMSETTINGAERODROM_H
#define FORMSETTINGAERODROM_H

#include <QWidget>

namespace Ui {
    class FormSettingAerodrom;
}

class FormSettingAerodrom : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingAerodrom(QWidget *parent = nullptr);
    ~FormSettingAerodrom();

private:
    Ui::FormSettingAerodrom *ui;
};

#endif // FORMSETTINGAERODROM_H
