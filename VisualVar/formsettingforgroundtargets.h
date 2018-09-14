#ifndef FORMSETTINGFORGROUNDTARGETS_H
#define FORMSETTINGFORGROUNDTARGETS_H

#include <QWidget>
#include "globalTypeData.h"
namespace Ui {
    class FormSettingForGroundTargets;
}

class FormSettingForGroundTargets : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingForGroundTargets(QWidget *parent = 0);
    ~FormSettingForGroundTargets();
    void setV(double);
    void setPsi(double); 
    void setListObjectVis(QList<TObjectVis*> &list);
    void setEnableTarget(bool);
    void setDefaultObjectVis();
    void setCode(int);
     void setPrCodeLength(bool value);
    void setLat(double);
    void setLon(double);
    void setLonDouble(double value);
    void setLatDouble(double value);
    void setX(double);
    void setZ(double);

    double minV();
    double maxV();

    QString currentTypeObject();


public slots:
    void slotV(int);
    void slotPsi(int);
    void slotEnable(bool);
    void slotCode(int);

    void slotLat(QString);
    void slotLon(QString);

    void slotVD(double);
    void slotPsiD(double);
    void slotLatDouble(double);
    void slotLonDouble(double);
signals:
    void signalPsi(double);
    void signalV(double);
    void signalCode(int,QString);

    void signalX(double);
    void signalZ(double);

    void signalEnable(bool);

    void signalLat(double);
    void signalLon(double);
private:
    int prCodeLength;
    int currentCode;
    QString currentCodeStr;
    QList<TObjectVis*> listObjectVisGround;
    Ui::FormSettingForGroundTargets *ui;
};

#endif // FORMSETTINGFORGROUNDTARGETS_H
