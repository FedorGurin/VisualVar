#ifndef DELEGATETABLEMANUAL_H
#define DELEGATETABLEMANUAL_H

#include <QItemDelegate>
#include "../cl_scene.h"

namespace VisualVariant
{
class DelegateTableManual : public QItemDelegate
{
    Q_OBJECT
public:
    DelegateTableManual(int indexColumn,
                        QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    //! указать индекс страницы
    void setPageIndex(int index_,
                      cl_Scene* scene);
private:
    int pageIndex;
    int columnIndex;
    //! текущая сцена
    cl_Scene *currentScene;
private slots:
    void commitAndCloseEditor();
    void commitAndCloseEditorDouble();
    void commitAndCloseEditorComboBox();
    void commitAndCloseEditorLine();
   /* void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;*/
};
}
#endif // DELEGATETABLEMANUAL_H
