#ifndef FORMADDLABELMAP_H
#define FORMADDLABELMAP_H

#include <QWidget>
#include <QFile>
#include <QDomElement>
#include <QDomDocument>
#include <QMessageBox>
#include <QTextStream>
#include <QList>

#include "NodeVisual.h"

namespace Ui {
class FormAddLabelMap;
}
using namespace VisualVariant;
class FormAddLabelMap : public QWidget
{
    Q_OBJECT
    
public:
    explicit  FormAddLabelMap(QWidget *parent = nullptr);

    void addNewLabel(double lat,double lon);
    void addNewLabel(QString name,double lat,double lon);

    //! разбор XML с данными
    void parseXML(const QDomElement& element);//рекурсивная
    bool openXML(const QString &fileName);
    void saveXML(const QString &fileName);
    QDomDocument domLabels;
    QDomDocument saveDomLabels;
    QDomElement  rootNode;
    QList<LabelObject* >    labelObjects;
protected:
    virtual bool event(QEvent *event);

    ~FormAddLabelMap();
public slots:
    void slotCancel();
    void slotAdd();
    void slotGoto();
    void slotDelete();
    void slotCellChanged(int row,int column);
signals:
    void gotoLatLon(double lat,double lon);
    void deleteLabel(LabelObject*);
private:
    Ui::FormAddLabelMap *ui;
};

#endif // FORMADDLABELMAP_H
