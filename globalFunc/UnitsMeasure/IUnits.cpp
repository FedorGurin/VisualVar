#include "IUnits.h"
#include <QFile>
#include <QMessageBox>
#include <math.h>

#define DATA_FILE "/xml/factors.xml"


QStringList* TObjectProperty::getListAvailableUnits(void)
{
    if(list.size()!=0) return &list;

    for(int i=0;i<units.size();i++)
    {
        if(units[i].unicode_rus.isEmpty()==false)
            list.push_back(units[i].unicode_rus);
        else
            list.push_back(units[i].unicode_id);
    }
    return &list;
}
TObjectUnit* TObjectProperty::find(QString value)
{
     for(int i=0;i<units.size();i++)
     {
         if(units[i].unicode_id==value || units[i].unicode_rus==value)
             return &units[i];
     }
     if(units.size()!=0)
         return &units[0];

     return 0;
}

bool TObjectProperty::openXMLFile(QString nameFile, QString prop)
{
    QFile file(qApp->applicationDirPath()+nameFile);
    bool openFile=false;
    bool ok=true;

    QDomDocument domMeasureData;

    openFile=file.open(QIODevice::ReadOnly | QIODevice::Text);

    if(openFile==true)
    {
        bool readXML=false;
        QString errMsg="";
        int errLine=0;
        int errColumn=0;
        readXML=domMeasureData.setContent(&file,true,&errMsg,&errLine,&errColumn);

        if(readXML==true)
        {
            QDomElement root=domMeasureData.documentElement();
            parseXMLFile(root.toElement(),prop);
        }
        else
        {
            ok=false;
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла = ")+QString(DATA_FILE)+"\n\nError msg="
                                 +errMsg+"\nLine="+QString::number(errLine)+"\nColumn="+QString::number(errColumn));
        }
    }
    else
    {
        ok=false;
        QMessageBox::warning(0,tr("Внимание"),
                             tr("Файл не найден = ")+QString(DATA_FILE));
    }
    return ok;
}
double TObjectProperty::convert(double valueFrom,TObjectUnit* from, TObjectUnit* to)
{
    return valueFrom*pow(10,from->e-to->e)*(from->factor/to->factor);
}
double TObjectProperty::convert(double valueFrom,TObjectUnit* from, QString to)
{
    TObjectUnit* to_=find(to);
    return convert(valueFrom,from,to_);
}
double TObjectProperty::convert(double valueFrom,QString from, TObjectUnit* to)
{
    TObjectUnit* from_=find(from);
    return convert(valueFrom,from_,to);
}
double TObjectProperty::convert(double valueFrom,QString from, QString to)
{
    TObjectUnit* from_=find(from);
    TObjectUnit* to_=find(to);
    return convert(valueFrom,from_,to_);
}

void TObjectProperty::parseXMLFile(const QDomElement &element, QString& prop)
{
    if(element.isNull()==true) return;

    QString tempName;
    TObjectUnit tempUnit;

    QDomElement ele1=element.firstChildElement(tr("property"));
    while(!ele1.isNull())
    {
        tempName=ele1.attribute("name","None");
        if(tempName==prop)
        {
            name=prop;
            QDomElement ele=ele1.firstChildElement(tr("unit"));
            while(!ele.isNull())
            {
                QDomElement temp_element=ele.firstChildElement(tr("enable"));
                if(temp_element.isNull()==false)
                {
                    int isEnable=temp_element.text().toInt();
                    if(isEnable==1)
                    {
                        ///////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("unitname"));
                        if(temp_element.isNull()==false)
                            tempUnit.unitname=      temp_element.text();
                        ////////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("unitname_rus"));
                        if(temp_element.isNull()==false)
                            tempUnit.unitname_rus=  temp_element.text();
                        else
                            tempUnit.unitname_rus=  tempUnit.unitname;
                        ///////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("unicode"));
                        if(temp_element.isNull()==false)
                            tempUnit.unicode_id=    temp_element.text();
                        ///////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("unicode_rus"));
                        if(temp_element.isNull()==false)
                            tempUnit.unicode_rus=   temp_element.text();
                        else
                            tempUnit.unicode_rus=   tempUnit.unicode_id;
                        ///////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("E"));
                        if(temp_element.isNull()==false)
                            tempUnit.e=             temp_element.text().toDouble();
                        ///////////////////////////////////////////////////////////////////
                        temp_element=ele.firstChildElement(tr("factor"));
                        if(temp_element.isNull()==false)
                            tempUnit.factor=        temp_element.text().toDouble();
                        ///////////////////////////////////////////////////////////////////
                        units.push_back(tempUnit);
                    }
                }
                ele=ele.nextSiblingElement();
            };
        }
        ele1=ele1.nextSiblingElement();
    }
}
