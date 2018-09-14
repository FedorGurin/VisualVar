#ifndef FORMLISTAERODROMS_H
#define FORMLISTAERODROMS_H

#include <QWidget>

#include <QDomElement>
#include <QDomDocument>

#include "NodeVisual.h"
namespace Ui {
class FormListAerodroms;
}

using namespace VisualVariant;
//! класс для чтения аэродромов и отображение данных
class FormListAerodroms : public QWidget
{
    Q_OBJECT    
public:
    explicit FormListAerodroms(QWidget *parent = 0);
    ~FormListAerodroms();
    // добавить новый аэродром
    void addNewAerodrom(const QDomElement& element);
    //! разбор XML с данными
    void parseXML(const QDomElement& element);//рекурсивная
    bool openXML(const QString &fileName);
    //void saveXML(const QString &fileName);
    QDomDocument domAerodroms;
    QDomDocument saveDomAerodroms;
    QDomElement  rootNode;

    QList<AerodromObject* >    aerodromObjects;

protected:
    virtual bool event(QEvent *event);
private:
    Ui::FormListAerodroms *ui;
};

#endif // FORMLISTAERODROMS_H
