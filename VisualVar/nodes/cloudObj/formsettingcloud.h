#ifndef FORMSETTINGCLOUD_H
#define FORMSETTINGCLOUD_H

#include <QWidget>

namespace Ui {
class FormSettingCloud;
}

class FormSettingCloud : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingCloud(QWidget *parent = nullptr);
    ~FormSettingCloud();

private:
    Ui::FormSettingCloud *ui;
};

#endif // FORMSETTINGCLOUD_H
