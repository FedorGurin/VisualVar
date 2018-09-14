#ifndef CONVERTUNITSMEASURE_H
#define CONVERTUNITSMEASURE_H

#include <QObject>
#include <QVector>
#include <QStringList>

#include <QApplication>
#include <QDomElement>
#include <QDomDocument>
#include <QMessageBox>

#include "IUnits.h"

class ConvertUnitsMeasure : public QObject
{
    Q_OBJECT
public:
    explicit ConvertUnitsMeasure(QObject *parent = 0);
    //! получить список возможных преобразований для указаной ед. измерения
    QStringList getListAvailableUnits(QString& currentUnit);

    //! проверка того, что указанная пара может быть выведена друг и здруга
    bool checkDerivationUnits(QString& newUnit,QString& currentUnit);

    //! вернуть объект преобразования связанный с указанной единицей
    //IUnits* getObjectUnits(QString& unit);

    bool openXMLFile(QString nameFile);
    void parseXMLFile(const QDomElement &element);

    //! DOM - дерево c  описанием данных c ед. измерения
    QDomDocument domMeasureData;
    TObjectProperty* getObjectUnitsByUnitname(QString baseUnit);

signals:

public slots:

private:
    QVector<TObjectProperty> units;

};

#endif // CONVERTUNITSMEASURE_H
