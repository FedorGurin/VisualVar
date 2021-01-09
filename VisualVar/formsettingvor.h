#ifndef FORMSETTINGBEACONVOR_H
#define FORMSETTINGBEACONVOR_H

#include <QWidget>

namespace Ui {
    class FormSettingBeaconVOR;
}

class FormSettingBeaconVOR : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingBeaconVOR(QWidget *parent = nullptr);
    ~FormSettingBeaconVOR();

private:
    Ui::FormSettingBeaconVOR *ui;
};

#endif // FORMSETTINGBEACONVOR_H
