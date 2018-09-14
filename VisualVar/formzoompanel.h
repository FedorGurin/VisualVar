#ifndef FORMZOOMPANEL_H
#define FORMZOOMPANEL_H

#include <QWidget>

namespace Ui {
    class FormZoomPanel;
}

class FormZoomPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FormZoomPanel(QWidget *parent = 0);
    ~FormZoomPanel();

public slots:
    void slotEvents(int);
    //! увеличить масштаб
    void slotButtonUp();
    //! уменьшить масштаб
    void slotButtonDown();
    //! установка начального значения из дипазона[0.0-1.0][1.0-10.0]
    void setBeginValue(int);
signals:
    void signalZoom(int);

private:
    Ui::FormZoomPanel *ui;
};

#endif // FORMZOOMPANEL_H
