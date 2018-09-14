#ifndef VARIANTENGINEDATA_H
#define VARIANTENGINEDATA_H

#include "../CommonEngineData/CommonEngineData.h"

class VariantEngineData:public IEngineData
{
    Q_OBJECT
public:
    VariantEngineData(QObject* parent=0);

    //! задать данные
    virtual uint setValue(QString idName,QString value, TYPE_REQUEST type=ASYNCH_ENGINE);
    //! выполнение специальной команды
    virtual uint specialCommand(QStringList str1,QStringList str2,TYPE_REQUEST type=ASYNCH_ENGINE);
    //! получить данные
    virtual uint getValue(QString idName,QString& value,TYPE_REQUEST type=ASYNCH_ENGINE);
signals:
    void signalLoadVariant(QString);

    void sigAircraftPsi(double);
    void sigAircraftLat(double);
    void sigAircraftLon(double);

    void sigTargetPsi(int,double);
    void sigTargetLat(int,double);
    void sigTargetLon(int,double);

    void sigTime(double);
private:
    QString pathFolder;


};

#endif // VARIANTENGINEDATA_H
