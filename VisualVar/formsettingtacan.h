#ifndef FORMSETTINGTACAN_H
#define FORMSETTINGTACAN_H

#include <QWidget>

namespace Ui {
    class FormSettingBeaconTACAN;
}

class FormSettingBeaconTACAN : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingBeaconTACAN(QWidget *parent = 0);
    ~FormSettingBeaconTACAN();

private:
    Ui::FormSettingBeaconTACAN *ui;
};

#endif // FORMSETTINGBEACONRSBN_H
