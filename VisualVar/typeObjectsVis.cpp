#include "typeObjectsVis.h"
#include <QFile>
#include <QMessageBox>
#include <QDomElement>
#include <QApplication>
#define NAME_OBJ_VIS "/xml/typeObjectVis.xml"
TypeObjectsVis::TypeObjectsVis():QObject()
{
    bool openFile = false;
    QFile file(qApp->applicationDirPath()+QString(NAME_OBJ_VIS));

    openFile = file.open(QIODevice::ReadOnly);
    if(openFile == true)
    {
        bool readXML = false;

        readXML  = domDocument.setContent(&file,true);
        if(readXML == true)
        {
            QDomElement root = domDocument.documentElement();
            parser(root.toElement());
        }
        else
        {
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла = ") + QString(NAME_OBJ_VIS));
        }
    }
    else
    {
        QMessageBox::warning(0,tr("Внимание"),
                             tr("Файл не найден = ")+QString(NAME_OBJ_VIS));
    }
}
QString TypeObjectsVis::codeAir(int code)
{
    QString str = "0";
    for(auto i:listObjectVisAir)
    {
        if(i->code == code)
            str= i->name;
    }
    return str;
}

QString TypeObjectsVis::codeGround(int code)
{
    QString str = "0";
    for(auto i:listObjectVisGround)
    {
        if(i->code == code)
            str = i->name;
    }
    return str;
}

//загрузка объектов воздушных целей
void TypeObjectsVis::parserObject(QDomElement &tempElement,QList<TObjectVis*> &list)
{
    QDomElement ele = tempElement.firstChildElement("object");
    while(!ele.isNull())
    {
        TObjectVis *tempObjVis  = new TObjectVis;
        tempObjVis->name        = ele.attribute("name","");
        tempObjVis->className   = ele.attribute("class","");
        tempObjVis->code        = (ele.attribute("code","0")).toInt();
        tempObjVis->pathToImage = ele.attribute("pixmap","");
        tempObjVis->comment     = ele.attribute("comment","");

        list.push_back(tempObjVis);
        ele = ele.nextSiblingElement("object");
    }
}

void TypeObjectsVis::parser(QDomElement tempElement)
{
    QDomElement ele = tempElement.firstChildElement("air");
    parserObject(ele,listObjectVisAir);

    ele = tempElement.firstChildElement("ground");
    parserObject(ele,listObjectVisGround);

    ele = tempElement.firstChildElement("info");
    parserObject(ele,listObjectVisInfo);
}
QList<TObjectVis*>& TypeObjectsVis::listAirObjects()
{
    return listObjectVisAir;
}

QList<TObjectVis*>& TypeObjectsVis::listGroundObjects()
{
    return listObjectVisGround;
}

QList<TObjectVis*>& TypeObjectsVis::listInfoObjects()
{
    return listObjectVisInfo;
}

