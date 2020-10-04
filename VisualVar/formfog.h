#ifndef FORMFOG_H
#define FORMFOG_H

#include <QWidget>

namespace Ui {
class FormFog;
}

class FormFog : public QWidget
{
    Q_OBJECT

public:
    explicit FormFog(QWidget *parent = nullptr);
    ~FormFog();

private:
    Ui::FormFog *ui;
};

#endif // FORMFOG_H
