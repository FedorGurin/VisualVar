#ifndef MESSUREUNITS_H
#define MESSUREUNITS_H

#include <QObject>
#include <QVector>

typedef struct
{
    //! имя единицы измерения
    QString name;
    //! коэффициент пересчета
    double k;

    double min;
    double max;

    double singleStep;

}TTableMessure;

class MessureUnits : public QObject
{
    Q_OBJECT
public:
    explicit MessureUnits(QObject *parent = nullptr);

    bool toStandartUnitSpeed(double value,
                             QString messure,
                             double &outValue);
    bool toStandartUnitAlt(double value,
                           QString messure,
                           double &outValue);
    bool toStandartUnitAngle(double value,
                             QString messure,
                             double &outValue);

    bool fromStandartUnitSpeed(double value,
                               QString messure,
                               double &outValue);
    bool fromStandartUnitAlt(double value,
                             QString messure,
                             double &outValue);
    bool fromStandartUnitAngle(double value,
                               QString messure,
                               double &outValue);

    bool convertSpeed(double value,QString messureCurrent,QString messureLast,double &outValue);
    bool convertAlt(double value,QString messureCurrent,QString messureLast,double &outValue);
    bool convertAngle(double value,QString messureCurrent,QString messureLast,double &outValue);

    bool minmaxSpeed(QString,double &min,double &max);
    bool minmaxAlt(QString,double &min,double &max);
    bool minmaxAngle(QString,double &min,double &max);
signals:

public slots:
private:
    QVector<TTableMessure> forSpeed;
    QVector<TTableMessure> forAlt;
    QVector<TTableMessure> forAngle;
};

#endif // MESSUREUNITS_H
