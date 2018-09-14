#ifndef FORMSETTINGFORAIRTARGET_H
#define FORMSETTINGFORAIRTARGET_H

#include <QWidget>
#include "globalTypeData.h"
#include "MessureUnits.h"
namespace Ui {
    class FormSettingForAirTarget;
}

class FormSettingForAirTarget : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingForAirTarget(QWidget *parent = 0);
    ~FormSettingForAirTarget();

    double minV();
    double maxV();

    void setV(double);
    void setFi(double);
    void setPsi(double);
    void setTeta(double);
    void setH(double);
    void setD(double);
    void setListObjectVis(QList<TObjectVis*> &list);
    void setEnableTarget(bool);
    void setCode(int);
    QString typeObject(int code);
    void setPrCodeLength(bool value);
    void setLat(double);
    void setLon(double);
    void setLonDouble(double value);
    void setLatDouble(double value);
    QString currentTypeObject();
    //double currentTeta();

    QString currentMessureSpeed();
    QString currentMessureAlt();
public slots:
    void slotV(int);
    void slotFi(int);
    void slotPsi(int);
    void slotH(int);
    void slotD(double);
    void slotTeta(int);

    void slotTetaD(double);
    void slotFiD(double);
    void slotVD(double);
    void slotPsiD(double);
    void slotHD(double);
    void slotEnable(bool);
    void slotCode(int);

    void slotLat(QString);
    void slotLon(QString);
    //! реакция на смену ед. измерения скорости
    void slotComboMessureV(int);
    void slotLatDouble(double);
    void slotLonDouble(double);
signals:
    void signalV(double);
    void signalFi(double);
    void signalPsi(double);
    void signalH(double);
    void signalD(double);
    void signalEnable(bool);
    void signalCode(int,QString);
    void signalTeta(double);

    void signalLat(double);
    void signalLon(double);
protected:
    void changeEvent(QEvent *e);

private:
    int messureV;
    int prCodeLength;
    QList<TObjectVis*> listObjectVisAir;
    Ui::FormSettingForAirTarget *ui;

    MessureUnits *messureUnits;

    QString messureCurrentSpeed;
    QString messureCurrentAlt;
    int currentCode;
    QString currentCodeStr;
};

#endif // FORMSETTINGFORAIRTARGET_H
