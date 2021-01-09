#pragma once

#include <QString>
#include <QDomElement>
#include <QDomDocument>
#include <QList>
#include <QObject>

#include "globalTypeData.h"

//! Класс для загрузки кодов и изображений для объектов
class TypeObjectsVis:public QObject
{
public:
    TypeObjectsVis();
    //! загрузка объектов воздушных целей
    void parserObject(QDomElement &tempElement,QList<TObjectVis*> &list);

    QString codeAir(int code);
    QString codeGround(int code);

    QList<TObjectVis*>& listAirObjects();
    QList<TObjectVis*>& listGroundObjects();
    QList<TObjectVis*>& listInfoObjects();
private:
    void parser(QDomElement tempElement);
    //! список типов объектов для системы визуализации
    QList<TObjectVis*> listObjectVisAir;
    QList<TObjectVis*> listObjectVisGround;
    QList<TObjectVis*> listObjectVisInfo;
    //!
    QDomDocument domDocument;
    QDomElement rootNode;
};
