#ifndef FORMSETTINGBEACONRSBN_H
#define FORMSETTINGBEACONRSBN_H

#include <QWidget>

namespace Ui {
    class FormSettingBeaconRSBN;
}

class FormSettingBeaconRSBN : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingBeaconRSBN(QWidget *parent = 0);
    ~FormSettingBeaconRSBN();

private:
    Ui::FormSettingBeaconRSBN *ui;
};

#endif // FORMSETTINGBEACONRSBN_H
