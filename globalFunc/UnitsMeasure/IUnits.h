#ifndef IUNITS_H
#define IUNITS_H

#include <QString>
#include <QVector>
#include <QStringList>
#include <QObject>

#include <QApplication>
#include <QDomElement>
#include <QDomDocument>

//! Класс интерфейс для всех классов единиц измерения
typedef struct
{
    QString unitname;
    QString unitname_rus;

    double factor;
    double e;

    QString unicode_id;
    QString unicode_rus;
}TObjectUnit;

class TObjectProperty: public QObject
{
    Q_OBJECT
public:

    TObjectProperty(QObject *parent=0):QObject(parent)
    {
        list.size();
    }
    TObjectProperty(QString nameFile, QString prop,QObject *parent=0):QObject(parent)
    {

        list.size();
        openXMLFile(nameFile,prop);
    }

    bool openXMLFile(QString nameFile, QString prop);
    void parseXMLFile(const QDomElement &element,QString& prop);
    double convert(double valueFrom,TObjectUnit* from, TObjectUnit* to);
    double convert(double valueFrom,TObjectUnit* from, QString to);
    double convert(double valueFrom,QString from, QString to);
    double convert(double valueFrom,QString from, TObjectUnit* to);

    TObjectUnit* find(QString value);
    QString name;
    QString baseUnitName;
    QVector<TObjectUnit> units;
    QStringList* getListAvailableUnits(void);
    QStringList list;
};

#endif // IUNITS_H
