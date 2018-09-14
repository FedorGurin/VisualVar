#include "ConvertUnitsMeasure.h"
#include <QFile>

#define DATA_FILE "factors.xml"
ConvertUnitsMeasure::ConvertUnitsMeasure(QObject *parent) :
    QObject(parent)
{
    bool isOpen=openXMLFile(qApp->applicationDirPath()+"/"+QString(DATA_FILE));
}
bool ConvertUnitsMeasure::openXMLFile(QString nameFile)
{
    QFile file(nameFile);
    bool openFile=false;
    bool ok=true;

    openFile=file.open(QIODevice::ReadOnly | QIODevice::Text);

    if(openFile==true)
    {
        qDebug("open\n");
        bool readXML=false;
        QString errMsg="";
        int errLine=0;
        int errColumn=0;
        readXML=domMeasureData.setContent(&file,true,&errMsg,&errLine,&errColumn);

        if(readXML==true)
        {
            qDebug("parseXMLFile\n");

            QDomElement root=domMeasureData.documentElement();
            parseXMLFile(root.toElement());
        }
        else
        {
            ok=false;
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла = ")+QString(DATA_FILE)+"\n\nError msg="
                                 +errMsg+"\nLine="+QString::number(errLine)+"\nColumn="+QString::number(errColumn));

            //qApp->quit();
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
void ConvertUnitsMeasure::parseXMLFile(const QDomElement &element)
{
    if(element.isNull()==true) return;

    TObjectProperty obj;

    QDomElement ele1=element.firstChildElement(tr("property"));
    while(!ele1.isNull())
    {

        obj.name=ele1.attribute("name","None");
        qDebug("property=%s\n",qPrintable(obj.name));

        QDomElement ele=ele1.firstChildElement(tr("unit"));
        while(!ele.isNull())
        {
            TObjectUnit tempUnit;

            tempUnit.unitname=(ele.firstChildElement(tr("unitname"))).text();
            tempUnit.unitname_rus=ele.firstChildElement(tr("unitname_rus")).text();
            tempUnit.unicode_id=ele.firstChildElement(tr("unicode")).text();
            tempUnit.e=ele.firstChildElement(tr("E")).text().toDouble();
            tempUnit.factor=ele.firstChildElement(tr("factor")).text().toDouble();

            ele=ele.nextSiblingElement();
        };
        units.push_back(obj);
        ele1=ele1.nextSiblingElement();
    }
}
TObjectProperty* ConvertUnitsMeasure::getObjectUnitsByUnitname(QString baseUnit)
{

    for(int i=0;i<units.size();i++)
    {
        for(int j=0;j<units[i].units.size();j++)
        {
            if(units[i].units[j].unicode_id==baseUnit) return &units[i];
        }
    }
    return 0;
}
