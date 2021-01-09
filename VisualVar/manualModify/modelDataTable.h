#ifndef MODELDATATABLE_H
#define MODELDATATABLE_H
#include <QAbstractTableModel>
#include "../cl_scene.h"
#include <QVector>


class ModelDataTable: public QAbstractTableModel
{
public:
    ModelDataTable(QVector<cl_Scene* > *scenes_,QObject *parent = nullptr);
    QModelIndex index(int row,int column, const QModelIndex &parent )const;
    int rowCount(const QModelIndex &parent)const;
    int columnCount(const QModelIndex &parent)const;
    //QModelIndex parent(const QModelIndex &child)const;
    
    QVariant data(const QModelIndex &index,int role)const;
    QVariant headerData(int section,Qt::Orientation orientation, int role)const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void changePageIndex(QModelIndex index);
    void setPageIndex(int index_);
    int currentPageIndex(){return pageIndex;}
    void refreshModelData();
    void refreshModelData(QModelIndex);
    
    //! страницы
    QVariant page0(const QModelIndex &index,int role)const;
    QVariant page1(const QModelIndex &index,int role)const;
    QVariant page2(const QModelIndex &index,int role)const;

    //! отрисовка пути
    void checkPath(); 
    //! путь
    QString path;
    //! текущая сцена
    cl_Scene *currentScene;
private:
    
    //! индекс страницы
    int pageIndex;
    //! список всех сцен
    QVector<cl_Scene* > *scenes;

    GraphNode *currentGraphNode;
};

#endif // MODELDATATABLE_H
