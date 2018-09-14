#include "VariantEngineData.h"

VariantEngineData::VariantEngineData(QObject *parent):IEngineData(parent)
{
    setSenderName(tr("VisV"));
    pathFolder.clear();
}
uint VariantEngineData::setValue(QString idName,QString value, TYPE_REQUEST type)
{
   /* Q_UNUSED(type);
    QStringList tempList = idName.split(tr("."), QString::SkipEmptyParts);
    //! проверка формата команды
    if(tempList.size()<2) return 0;
    if(tempList[0]!=tr("VisV")) return 0;

    TRequestEvent rEvent;
    rEvent.module=tr("VisV");
    uint uid=qrand();

    rEvent.result=true;
    if(tempList[1]==tr("setLoadPath"))
    {
        pathFolder=value;
    }else if(tempList[1]==tr("setLoadVariantByName"))
    {
        emit signalLoadVariant(pathFolder+value);
    }else if(tempList[1]=="Aircraft")
    {
        if(tempList[2]=="Psi")
            emit sigAircraftPsi(value.toDouble());
        else if(tempList[2]=="Lat")
            emit sigAircraftLat(value.toDouble());
        else if(tempList[2]=="Lon")
            emit sigAircraftLon(value.toDouble());
    }else if(tempList[1]=="Time")
    {
        emit sigTime(value.toDouble());
    }else
    {
        QString     tempStr="";
        int index=-1;
        tempStr=tempList[1];
        if(tempStr.contains("[") && tempStr.contains("]"))
        {
            int posL=tempStr.indexOf("[");
            int posR=tempStr.indexOf("]");
            if(tempStr.size()>posL)
            {
                index=(tempStr.mid(posL+1,posR-(posL+1))).toInt();
                QStringList temp_=tempStr.split(tr("["), QString::SkipEmptyParts);;
                tempStr=temp_[0];
            }
        }
        if(tempStr=="AirTarget")
        {
            if(tempList[2]=="Psi")
            {
                emit sigTargetPsi(index,value.toDouble());
            }else if(tempList[2]=="Lat")
            {
                emit sigTargetLat(index,value.toDouble());
            }else if(tempList[2]=="Lon")
            {
                emit sigTargetLon(index,value.toDouble());
            }
        }
    }
    rEvent.uid=uid;
    if(uid!=0)
    {

        emit signalEventsRequest(rEvent);
    }
    return uid;*/

    return 0;
}

//! выполнение специальной команды
uint VariantEngineData::specialCommand(QStringList str1,QStringList str2,TYPE_REQUEST type)
{
    /*Q_UNUSED(str2);Q_UNUSED(type);
    QStringList tempList = str1[0].split(".", QString::SkipEmptyParts);
    if(tempList.size()>2) return 0;
    if(tempList[0]!=tr("VisV")) return 0;

    TRequestEvent rEvent;
    rEvent.result=true;
    rEvent.module=tr("VisV");
    uint uid=qrand();
    //qDebug("uid=%d\n",uid);

    //qDebug("call specialCommand=%s\n",str2[0].toAscii().data());

    if(tempList[1]==tr("Set"))
    {
        QString nameFile="";
        for(int i=2;i<tempList.size();i++)
        {
            nameFile+=tempList[i];
        }
        emit signalLoadVariant(nameFile);
    }else
    {
        uid=0;
        rEvent.result=false;
    }
    rEvent.uid=uid;
    if(uid!=0)
    {
        qDebug("uid!=0\n");
        emit signalEventsRequest(rEvent);
    }
    return uid;*/
    return 0;

}

//! получить данные
uint VariantEngineData::getValue(QString idName,QString& value,TYPE_REQUEST type)
{
    Q_UNUSED(idName);Q_UNUSED(value);Q_UNUSED(type);
    return 0;
}
