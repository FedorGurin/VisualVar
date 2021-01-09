#include "messureUnits.h"

MessureUnits::MessureUnits(QObject *parent) :
    QObject(parent)
{
    forSpeed.clear();
    forAlt.clear();
    forAngle.clear();
    TTableMessure m;
    m.name=tr("м/с");
    m.k=1.0;
    m.min=50.0;
    m.max=500.0;
    m.singleStep=1.0;
    forSpeed.push_back(m);

    m.name=tr("км/ч");
    m.k=3.6;
    m.min=150.0;
    m.max=2500.0;
    m.singleStep=1.0;
    forSpeed.push_back(m);

    m.name=tr("миль/ч");
    m.k=1.0/0.44704;
    m.min=50.0;
    m.max=1500.0;
    m.singleStep=1.0;
    forSpeed.push_back(m);
    //////////////////////
    m.name=tr("м");
    m.k=1.0;
    m.min=0.0;
    m.max=20000.0;
    m.singleStep=100.0;
    forAlt.push_back(m);

    m.name=tr("км");
    m.k=1/1000.0;
    m.min=0.0;
    m.max=20.0;
    m.singleStep=1;
    forAlt.push_back(m);

    m.name=tr("фут");
    m.k=1.0/0.3048;
    m.min=0.0;
    m.max=60000.0;
    m.singleStep=100.0;
    forAlt.push_back(m);
    //////////////////////////////////////////
    m.name=tr("град");
    m.k=1.0;
    m.min=0.0;
    m.max=360.0;
    m.singleStep=1.0;
    forAngle.push_back(m);

    m.name=tr("рад");
    m.k=0.017453292519943295769236907684886;
    m.min=0.0;
    m.max=6.283185307179586476925286766559;
    m.singleStep=m.max/100.0;
    forAngle.push_back(m);
}

bool MessureUnits::minmaxSpeed(QString messure,double &min,double &max)
{
    bool find=false;
    for(int i=0;i<forSpeed.size();i++)
    {
        if(forSpeed[i].name==messure)
        {
            find=true;
            min=forSpeed[i].min;
            max=forSpeed[i].max;
        }
    }
    return find;
}
bool MessureUnits::minmaxAlt(QString messure,double &min,double &max)
{
    bool find=false;
    for(int i=0;i<forAlt.size();i++)
    {
        if(forAlt[i].name==messure)
        {
            find=true;
            min=forAlt[i].min;
            max=forAlt[i].max;
        }
    }
    return find;

}
bool MessureUnits::minmaxAngle(QString messure,double &min,double &max)
{
    bool find=false;
    for(int i=0;i<forAngle.size();i++)
    {
        if(forAngle[i].name==messure)
        {
            find=true;
            min=forAngle[i].min;
            max=forAngle[i].max;
        }
    }
    return find;

}
bool MessureUnits::toStandartUnitSpeed(double value,
                         QString messure,
                         double &outValue)
{
    bool find=false;
    for(int i=0;i<forSpeed.size();i++)
    {
        if(forSpeed[i].name==messure)
        {
            find=true;
            outValue=value/forSpeed[i].k;
        }
    }
    return find;
}
bool MessureUnits::toStandartUnitAlt(double value,
                       QString messure,
                       double &outValue)
{
    bool find=false;
    for(int i=0;i<forAlt.size();i++)
    {
        if(forAlt[i].name==messure)
        {
            find=true;
            outValue=value/forAlt[i].k;
        }
    }
    return find;
}

bool MessureUnits::toStandartUnitAngle(double value,
                         QString messure,
                         double &outValue)
{
    bool find=false;
    for(int i=0;i<forAngle.size();i++)
    {
        if(forAngle[i].name==messure)
        {
            find=true;
            outValue=value/forAngle[i].k;
        }
    }
    return find;

}
bool MessureUnits::convertSpeed(double value,QString messureCurrent,QString messureLast,double &outValue)
{
    //преобразуем в стандартные
    double stValue;
    bool flag=toStandartUnitSpeed(value,messureLast,stValue);
    if(flag==false) return false;
    flag=fromStandartUnitSpeed(stValue,messureCurrent,outValue);
    if(flag==false) return false;
    return true;

}

bool MessureUnits::convertAlt(double value,QString messureCurrent,QString messureLast,double &outValue)
{
    //преобразуем в стандартные
    double stValue;
    bool flag=toStandartUnitAlt(value,messureLast,stValue);
    if(flag==false) return false;
    flag=fromStandartUnitAlt(stValue,messureCurrent,outValue);
    if(flag==false) return false;
    return true;

}
bool MessureUnits::convertAngle(double value,QString messureCurrent,QString messureLast,double &outValue)
{
    //преобразуем в стандартные
    double stValue;
    bool flag=toStandartUnitAngle(value,messureLast,stValue);
    if(flag==false) return false;
    flag=fromStandartUnitAngle(stValue,messureCurrent,outValue);
    if(flag==false) return false;
    return true;

}

bool MessureUnits::fromStandartUnitSpeed(double value,
                           QString messure,
                           double &outValue)
{
    bool find=false;
    for(int i=0;i<forSpeed.size();i++)
    {
        if(forSpeed[i].name==messure)
        {
            find=true;
            outValue=value*forSpeed[i].k;
        }
    }
    return find;
}

bool MessureUnits::fromStandartUnitAlt(double value,
                         QString messure,double &outValue)
{
    bool find=false;
    for(int i=0;i<forAlt.size();i++)
    {
        if(forAlt[i].name==messure)
        {
            find=true;
            outValue=value*forAlt[i].k;
        }
    }
    return find;

}

bool MessureUnits::fromStandartUnitAngle(double value,
                           QString messure,double &outValue)
{
    bool find=false;
    for(int i=0;i<forAngle.size();i++)
    {
        if(forAngle[i].name==messure)
        {
            find=true;
            outValue=value*forAngle[i].k;
        }
    }
    return find;

}


