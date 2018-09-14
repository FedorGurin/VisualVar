#ifndef FORMSETTINGFORAIRCRAFT_H
#define FORMSETTINGFORAIRCRAFT_H

#include <QWidget>
#include "MessureUnits.h"


namespace Ui {
    class FormSettingForAircraft;
}

class FormSettingForAircraft : public QWidget
{
    Q_OBJECT
public:
    explicit FormSettingForAircraft(QWidget *parent = 0);

    ~FormSettingForAircraft();

    double minV();
    double maxV();

    void setV(double);
    void setVy(double);
    void setTeta(double);
    void setPsi(double);
    void setH(double);
    void setLat(double);
    void setLon(double);
    void setLatDouble(double);
    void setLonDouble(double);
    void setUntOtVy(bool);


    QString currentMessureSpeed();
    QString currentMessureAlt();
    QString currentMessurePsi();
    QString currentMessureTeta();
    QString currentMessureVy();

    double currentV();
    double currentVy();
    double currentTeta();
    double currentPsi();
    double currentH();

public slots:
    void slotV(int);
    void slotTeta(int);
    void slotPsi(int);
    void slotH(int);

    void slotVD(double);
    void slotTetaD(double);
    void slotPsiD(double);
    void slotHD(double);

    void slotLat();
    void slotLon();

    //! представление в числом с плавующей точкой
    void slotLonDouble(double);
    void slotLatDouble(double);
    //
    void slotComboBoxSpeed(int);
    void slotComboBoxAlt(int);
    void slotComboBoxPsi(int);
    void slotComboBoxTeta(int);
    //
    void slotRadioButtonTan(bool);
    void slotRadioButtonUnt(bool);

signals:
    void signalV(double);
    void signalTeta(double);
    void signalVy(double);
    void signalPsi(double);
    void signalH(double);
    void signalTanUnt(bool); //! если false- то угол тангажа, если true - то угол наклона траектории

    void signalLat(double,QString);
    void signalLon(double,QString);
    void signalPrVy(bool);
protected:
    void changeEvent(QEvent *e);
private:    
    Ui::FormSettingForAircraft *ui;
    MessureUnits *messureUnits;

    QString messureCurrentSpeed;
    QString messureCurrentAlt;
    QString messureCurrentPsi;
    QString messureCurrentTeta;

};

#endif // FORMSETTINGFORAIRCRAFT_H
