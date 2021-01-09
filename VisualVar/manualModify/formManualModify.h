#ifndef FORMMANUALMODIFY_H
#define FORMMANUALMODIFY_H

#include <QWidget>
#include <QString>
#include <QListWidget>

#include "ModelDataTable.h"
#include "DelegateTableManual.h"

namespace Ui{
    class FormManualModify;
}
namespace VisualVariant
{
class cl_MouseFilter:public QObject
{
    Q_OBJECT
protected:
    virtual bool eventFilter(QObject*,QEvent*);
public:
    cl_MouseFilter(QObject* pobj = nullptr,QWidget *slotForm_ = nullptr);
    QWidget *slotForm;
};
class FormManualModify : public QWidget
{
    Q_OBJECT
public:
    explicit FormManualModify( QVector<cl_Scene* > *scenes_,QWidget *parent = nullptr);
    ~FormManualModify();
    void resetModelData();
    void resetModelData(QModelIndex);

    QString comment();

    void setComment(QString name);
    void setLastNameFile(QString name);


public slots:
    void slotMouseDoubleClicked(QModelIndex);
    void slotMouseClicked(QModelIndex);
    void slotPushButtonBack();
    void slotPushButtonAdd();
    void slotPushButtonBegin();
    void slotPushButtonDelete();
    void slotPushButtonClone();
    void slotDoubleClickedListWidget(QListWidgetItem*);
    void slotKeyboardDel();
    void deleteScene(int index);
    void slotKeyboardInsert();
private:
    ModelDataTable *modelData;
    DelegateTableManual *delegateTable;
    QItemSelectionModel *selection;
protected:
    void changeEvent(QEvent *e);
signals:
    void createNewScene(bool);
    void cloneScene(cl_Scene*);
    void createNewAirObj();
    void createNewGroundObj();
    void currentActiveWindow(QString);
    void signalAttach();
private:
    QVector<cl_Scene* > *scenes;
    Ui::FormManualModify *ui;
    QString path;
    //! список возможных объектов для добавления
    QListWidget *listWidget;
};
}
#endif // FORMMANUALMODIFY_H
