/*
 * Parser.cpp
 *
 *  Created on: 17.06.2009
 *      Author: Fedor
 */

#include "parser.h"
#include <QApplication>
namespace VisualVariant
{
#define DATA_FILE "None"
#define VARIANT_FILE "data.xml"

Parser::Parser(FormStatusBar* form,QVector<cl_Scene* > *scenes_,TypeObjectsVis *typeObjectsVis_)
{
    domVariants.clear();
    domVariantsForModel.clear();

    scenes          = scenes_;
    statusForm      = form;
    typeObjectsVis  = typeObjectsVis_;
}
void Parser::sortScenesObjects()
{
    scenesNU.clear();

    for(auto i: *scenes)
    {
        scenesNU.push_back(i);
    }
}
void Parser::saveVariants(const QString &fileName,
                          const QString &comment,
                          bool useMap,
                          uint id,
                          QString nameFileSend)
{
    bool fileOpen = false;
    QFile file(fileName);
    fileOpen = file.open(QIODevice::WriteOnly|QIODevice::Text);

    if(fileOpen == true)
    {
        saveDomVariants.clear();
        QTextStream out(&file);

        rootNode = saveDomVariants.createElement("data");
        rootNode.setAttribute("comment",    comment);
        rootNode.setAttribute("pathFile",   nameFileSend);
        rootNode.setAttribute("id_var",     id);
        rootNode.setAttribute("useMap",     useMap);
        rootNode.setAttribute("type","variantNU");
        saveDomVariants.appendChild(rootNode);

        for(auto i: *scenes)
        {
            i->saveToXML(saveDomVariants,rootNode);
        }
        QDomNode xmlNode=saveDomVariants.createProcessingInstruction("xml","version=\"1.0\" encoding=\"windows-1251\"");
        saveDomVariants.insertBefore(xmlNode,saveDomVariants.firstChild());
        saveDomVariants.save(out,4);
        out.flush();
    }

}
void Parser::createXMLForModel(QString comment,bool useMap,uint id)
{
    bool fileOpen=false;
    QFile file("./xml/export_var_vxworks.xml");
    fileOpen = file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);

    saveDomVariantsForModel.clear();
    rootNodeForModel = saveDomVariantsForModel.createElement("DOCUMENT");
    rootNodeForModel.setAttribute("comment",comment);
    rootNodeForModel.setAttribute("useMap",useMap);
    rootNodeForModel.setAttribute("id_var",id);
    saveDomVariantsForModel.appendChild(rootNodeForModel);

    QDomElement sizeVariantsNode = saveDomVariantsForModel.createElement("NumberOfVariations");
    QDomText domTextVar = saveDomVariantsForModel.createTextNode(QString::number(scenes->size()));
    sizeVariantsNode.appendChild(domTextVar);
    rootNodeForModel.appendChild(sizeVariantsNode);

    for(auto i : *scenes)
    {
        if(i->use == true)
            i->saveToXMLForModel(saveDomVariantsForModel,rootNodeForModel);
    }
    QDomNode xmlNode=saveDomVariantsForModel.createProcessingInstruction("xml","version=\"1.0\" encoding=\"windows-1251\"");
    saveDomVariantsForModel.insertBefore(xmlNode,saveDomVariantsForModel.firstChild());
    saveDomVariantsForModel.save(out,4);
    out.flush();
}

void Parser::createXMLForModel(QByteArray *array,QString comment,bool useMap,uint id)
{
    QTextStream out(array);

    /*bool fileOpen=false;
    QFile file("testmydata.xml");
    fileOpen=file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);*/

    saveDomVariantsForModel.clear();
    rootNodeForModel=saveDomVariantsForModel.createElement("DOCUMENT");
    rootNodeForModel.setAttribute("comment",comment);
    rootNodeForModel.setAttribute("useMap",useMap);
    rootNodeForModel.setAttribute("id_var",id);
    saveDomVariantsForModel.appendChild(rootNodeForModel);

    QDomElement sizeVariantsNode=saveDomVariantsForModel.createElement("NumberOfVariations");
    QDomText domTextVar=saveDomVariantsForModel.createTextNode(QString::number(scenes->size()));
    sizeVariantsNode.appendChild(domTextVar);
    rootNodeForModel.appendChild(sizeVariantsNode);

    for(auto i:*scenes)
    {
        if(i->use == true)
            i->saveToXMLForModel(saveDomVariantsForModel,rootNodeForModel);
    }
    QDomNode xmlNode = saveDomVariantsForModel.createProcessingInstruction("xml","version=\"1.0\" encoding=\"windows-1251\"");
    saveDomVariantsForModel.insertBefore(xmlNode,saveDomVariantsForModel.firstChild());
    saveDomVariantsForModel.save(out,4);
}
QString Parser::readLastNameFile(const QString &nameFile)
{
    bool openFile =  false;
    QFile file(nameFile);

    openFile = file.open(QIODevice::ReadOnly);
    if(openFile == true)
    {
        bool readXML=false;

        readXML = domVariants.setContent(&file,true);
        if(readXML == true)
        {
            QDomElement root = domVariants.documentElement();
            return root.attribute("pathFile",tr("Неизвестно"));
        }
    }
    return QString(tr("Неизвестно"));
}

void Parser::parseVariant(const QDomElement& element)
{
    int i = 0;
    QDomElement ele = element.firstChildElement("variant");
    while(!ele.isNull())
    {
        i++;
        cl_Scene *tempScene = new cl_Scene(ele,
                                         statusForm,
                                         typeObjectsVis);
        tempScene->index = i;
        scenes->push_back(tempScene);

        ele = ele.nextSiblingElement("variant");
    }
}

bool Parser::openFileVariants(const QString &nameFile, QString *name, bool &useMap,uint &id)
{
    bool openFile = false;
    QFile file(nameFile);

    openFile = file.open(QIODevice::ReadOnly);
    if(openFile == true)
    {
        bool readXML = false;

        readXML = domVariants.setContent(&file,true);
        if(readXML == true)
        {
            QDomElement root = domVariants.documentElement();

            QString typeVar = root.attribute("type",tr("unknown"));
            if(typeVar != tr("variantNU"))
            {

                QMessageBox::warning(0,tr("Внимание"),
                                     tr("Этот файл не содержит варианты(или файл имеет устаревший формат)"));
                return false;
            }

            *name  = root.attribute("comment","ff");
            useMap = root.attribute("useMap","1").toInt();
            id     = root.attribute("id_var","0").toUInt();
            parseVariant(root.toElement());
        }
        else
        {
            QMessageBox::warning(0,tr("Внимание"),
                                 tr("Ошибка в структуре XML файла \n = "))+QString(DATA_FILE);
            return false;
        }
    }
    /*else
    {
        QMessageBox::warning(0,tr("Внимание"),
                             tr("Файл не найден = "))+QString(VARIANT_FILE);
    }*/
    return openFile;
}
}
