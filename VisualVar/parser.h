/*
 * Parser.h
 *
 *  Created on: 17.06.2009
 *      Author: Fedor
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <QFile>
#include <QDomElement>
#include <QDomDocument>
#include <QMessageBox>
#include <QTextStream>

#include "cl_scene.h"
#include "TypeObjectsVis.h"


//! класс для сохранения и чтения вариантов
class Parser :public QObject
{
public:
    Parser(FormStatusBar* form,QVector<cl_Scene* > *scenes_,TypeObjectsVis *typeObectsVis_);
    QDomDocument domVariants;
    QDomDocument saveDomVariants;
    QDomElement  rootNode;

    QDomDocument domVariantsForModel;
    QDomDocument saveDomVariantsForModel;
    QDomElement  rootNodeForModel;

    //! открыть файл с вариантами
    bool openFileVariants(const QString &fileName, QString *,bool &useMap,uint &id);
    //! сохранить  варианты в файл
    void saveVariants(const QString &fileName, const QString &comment, bool useMap,uint id, QString nameFileSend="");
    //! создать файл с вариантами для последующей передачи в целевую машину
    void createXMLForModel(QByteArray *array,QString comment,bool useMap,uint id);
    //! создать файл с вариантами в формате целевой машины
    void createXMLForModel(QString comment,bool useMap,uint id);
    //! функция чтения(разбора) данных
    void parseVariant(const QDomElement& element);//рекурсивная

    //! отсортировать общий список объектов
    void sortScenesObjects();
    //! прочитать последний загруженный файл
    QString readLastNameFile(const QString &fileName);
private:
    //! общий список объектов
    QVector<cl_Scene* > *scenes;
    //! список объектов только с начальными условиями
    QVector<cl_Scene* > scenesNU;

    FormStatusBar* statusForm;
    TypeObjectsVis *typeObjectsVis;

};


#endif /* PARSER_H_ */
