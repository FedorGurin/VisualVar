#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>

#include "DelegateTableManual.h"

//#define OLD_STEND
namespace VisualVariant
{

DelegateTableManual::DelegateTableManual(int columnIndex_,
                                         QObject *parent):QItemDelegate(parent)
{
    Q_UNUSED(parent);
    columnIndex  = columnIndex_;
    pageIndex    = 0;
    currentScene = nullptr;
}
QWidget* DelegateTableManual::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index)const
{
    if(index.column() == 0 && (pageIndex == 0|| pageIndex == 1))
    {
        QComboBox *comboBox=new QComboBox(parent);
        comboBox->addItem(tr("Да"));
        comboBox->addItem(tr("Нет"));
        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditorComboBox()));
        return comboBox;
    }
    if(index.column() == 2 && pageIndex == 2)
    {
        GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

        if(node->type() == GraphNode::AIRCRAFT)
        {
            QComboBox *comboBox = new QComboBox(parent);
            QStringList* list = nullptr;

            if(index.row() == 0)      list = AircraftObject::unitSpeed->getListAvailableUnits();
            else if(index.row() == 1) list = AircraftObject::unitLength->getListAvailableUnits();
            else if(index.row() == 2) list = AircraftObject::unitAngle->getListAvailableUnits();
            else if(index.row() == 3) list = AircraftObject::unitAngle->getListAvailableUnits();
            else if(index.row() == 4) list = AircraftObject::unitSpeed->getListAvailableUnits();
            else
                return QItemDelegate::createEditor(parent,option,index);


            for(auto i:(*list))
            {
                comboBox->addItem(i);
            }
            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditorComboBox()));
            return comboBox;
        }
        else if(node->type() == GraphNode::OBJ_V)
        {
            QComboBox *comboBox = new QComboBox(parent);
            QStringList* list = nullptr;

                if(index.row() == 0) list = AirObj::unitSpeed->getListAvailableUnits();
                else if(index.row() == 1 || index.row() == 5 || index.row() == 7) list = AirObj::unitLength->getListAvailableUnits();
                else if(index.row() == 2 || index.row() == 3 || index.row() == 4) list = AirObj::unitAngle->getListAvailableUnits();
                else
                    return QItemDelegate::createEditor(parent,option,index);

            for(auto i:(*list))
            {
                comboBox->addItem(i);
            }
            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditorComboBox()));
            return comboBox;
        }
        else if(node->type() == GraphNode::OBJ_G)
        {
            QComboBox *comboBox = new QComboBox(parent);
            QStringList* list = nullptr;
            if(index.row() == 0)
                list = GroundObj::unitSpeed->getListAvailableUnits();
            else if(index.row() == 1)
                list = GroundObj::unitAngle->getListAvailableUnits();
            else if(index.row() == 3 || index.row() == 4)
                list = GroundObj::unitLength->getListAvailableUnits();
            else
                return QItemDelegate::createEditor(parent,option,index);

            for(auto i:(*list))
            {
                comboBox->addItem(i);
            }
            connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditorComboBox()));
            return comboBox;
        }
    }
    if(index.column()>=4 && pageIndex==2)
    {
        QComboBox *comboBox = new QComboBox(parent);
        QStringList* list=AircraftObject::unitAngle->getListAvailableUnits();

        for(auto i:(*list))
        {
            comboBox->addItem(i);
        }
        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditorComboBox()));
        return comboBox;
    }
    if(index.column()>=4 && pageIndex==0)
    {
        QLineEdit *lineEdit=new QLineEdit(parent);
        cl_Scene* scene=static_cast<cl_Scene*>(index.internalPointer());
        lineEdit->setText(scene->comment());
        connect(lineEdit,SIGNAL(editingFinished()),
                this,SLOT(commitAndCloseEditorLine()));
        return lineEdit;
    }
    if(index.column() == columnIndex && pageIndex==2)
    {
        GraphNode* node=static_cast<GraphNode*>(index.internalPointer());
        if(node->type()==GraphNode::AIRCRAFT)
        {

            if(index.row()==4 )
            {
                QComboBox *comboBox=new QComboBox(parent);
                comboBox->addItem(tr("С земли"));
                comboBox->addItem(tr("В воздухе"));

                connect(comboBox,SIGNAL(currentIndexChanged(int)),
                        this,SLOT(commitAndCloseEditorComboBox()));
                return comboBox;
            }else
            {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditorDouble()));
                return spinBox;
            }
        }
        if(node->type() == GraphNode::OBJ_V)
        {
            if(index.row() == 6)
            {
                QSpinBox *spinBox = new QSpinBox(parent);
                spinBox->setMaximum(1000);
                spinBox->setMinimum(100);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditor()));
                return spinBox;
            }else if(index.row() == 8)
            {
                QComboBox *comboBox=new QComboBox(parent);
                comboBox->addItem(tr("Длина"));
                comboBox->addItem(tr("Код"));
                connect(comboBox,SIGNAL(currentIndexChanged(int)),
                        this,SLOT(commitAndCloseEditorComboBox()));
                return comboBox;
            }else
            {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditorDouble()));
                return spinBox;
            }
        }
        if(node->type() == GraphNode::OBJ_G)
        {
            if(index.row() == 2)
            {
                QSpinBox *spinBox = new QSpinBox(parent);
                spinBox->setMaximum(1000);
                spinBox->setMinimum(300);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditor()));
                return spinBox;
            }else if(index.row() == 5)
            {
                QSpinBox *spinBox = new QSpinBox(parent);
                spinBox->setMaximum(1);
                spinBox->setMinimum(0);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditor()));
                return spinBox;
            }
            else
            {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);

                connect(spinBox,SIGNAL(editingFinished()),
                        this,SLOT(commitAndCloseEditorDouble()));
                return spinBox;
            }
        }
    }
    return QItemDelegate::createEditor(parent,option,index);
}

void DelegateTableManual::setEditorData(QWidget *editor,
                                        const QModelIndex &index)const
{
    if(index.column() == 0 && (pageIndex == 0 || pageIndex == 1))
    {
        QString value = index.model()->data(index,Qt::DisplayRole).toString();
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if(comboBox != nullptr)
            comboBox->setCurrentIndex(comboBox->findText(value));
    }else if(index.column()==2 && pageIndex==2)
    {
        GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

        if(node->type()==GraphNode::AIRCRAFT && index.row()!=5)
        {
            QString value=index.model()->data(index,Qt::DisplayRole).toString();
            QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
            int indexInCB=comboBox->findText(value);
            if(indexInCB!=-1)
                comboBox->setCurrentIndex(indexInCB);
        }else if((node->type()==GraphNode::OBJ_V && index.row()!=6 && index.row()!=8 )||
                 (node->type()==GraphNode::OBJ_G && index.row()!=2))
        {
            QString value=index.model()->data(index,Qt::DisplayRole).toString();
            QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
            int indexInCB=comboBox->findText(value);
            if(indexInCB!=-1)
                comboBox->setCurrentIndex(indexInCB);
        }
    }else if(index.column()==4 && pageIndex==0)
    {
        QString value=index.model()->data(index,Qt::DisplayRole).toString();
        QLineEdit *lineEdit=qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(value);
    }else if(index.column()==columnIndex && pageIndex==2)
    {
        GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

        if(node->type()==GraphNode::AIRCRAFT)
        {

            if(index.row()==4  )

            {
                QString value=index.model()->data(index,Qt::DisplayRole).toString();
                QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
                comboBox->setCurrentIndex(comboBox->findText(value));
            }else
            {
                double value=index.model()->data(index,Qt::DisplayRole).toDouble();
                QDoubleSpinBox *spinBox=qobject_cast<QDoubleSpinBox*>(editor);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);
                spinBox->setValue(value);
            }
        }
        if(node->type()==GraphNode::OBJ_V)
        {
            if(index.row()==6)
            {
                int value=index.model()->data(index,Qt::DisplayRole).toInt();
                QSpinBox *spinBox=qobject_cast<QSpinBox*>(editor);
                spinBox->setMaximum(1000);
                spinBox->setMinimum(100);
                spinBox->setValue(value);
            }else if(index.row()==8)
            {
                QString value=index.model()->data(index,Qt::DisplayRole).toString();
                QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
                comboBox->setCurrentIndex(comboBox->findText(value));
            }else
            {
                double value=index.model()->data(index,Qt::DisplayRole).toDouble();
                QDoubleSpinBox *spinBox=qobject_cast<QDoubleSpinBox*>(editor);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);
                spinBox->setValue(value);
            }
        }
        if(node->type()==GraphNode::OBJ_G)
        {
            if(index.row()==2)
            {
                int value=index.model()->data(index,Qt::DisplayRole).toInt();
                QSpinBox *spinBox=qobject_cast<QSpinBox*>(editor);
                spinBox->setMaximum(1000);
                spinBox->setMinimum(300);
                spinBox->setValue(value);
            }else if(index.row()==5)
            {
                int value=index.model()->data(index,Qt::DisplayRole).toInt();
                QSpinBox *spinBox=qobject_cast<QSpinBox*>(editor);
                spinBox->setMaximum(1);
                spinBox->setMinimum(0);
                spinBox->setValue(value);
            }else
            {
                double value=index.model()->data(index,Qt::DisplayRole).toDouble();
                QDoubleSpinBox *spinBox=qobject_cast<QDoubleSpinBox*>(editor);
                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);
                spinBox->setValue(value);
            }
        }
    }else
        QItemDelegate::setEditorData(editor,index);
}

void DelegateTableManual::setModelData(QWidget *editor,
                                       QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    if(index.column() == 0 && (pageIndex == 0 || pageIndex == 1))
    {

        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);

        QString value = comboBox->currentText();
        model->setData(index,value);
    }else if(index.column() == 4 && pageIndex == 0)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);

        QString value = lineEdit->text();
        model->setData(index,value);
    }else if(index.column() == 2 && pageIndex == 2)
    {
        GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

        if((node->type() == GraphNode::AIRCRAFT && index.row()!=5) ||
           //(node->type()==GraphNode::AIRCRAFT && index.row()!=2 && currentScene->circleVariant==true) ||
           (node->type() == GraphNode::OBJ_V && index.row()!=6 && index.row()!=8 )||
           (node->type() == GraphNode::OBJ_G && index.row()!=2))
        {
            QComboBox *comboBox = qobject_cast<QComboBox*>(editor);

            QString value = comboBox->currentText();
            model->setData(index,value);
        }else QItemDelegate::setModelData(editor,model,index);
    }else if(index.column() == 1 && pageIndex == 2)
    {
        GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

        if(node->type() == GraphNode::AIRCRAFT)
        {

            if(index.row() == 4 )

            {
                QComboBox *comboBox = qobject_cast<QComboBox*>(editor);

                QString value = comboBox->currentText();
                model->setData(index,value);
            }
            else
            {
                QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);

                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);
                spinBox->setDecimals(12);

                double value = spinBox->value();
                model->setData(index,value);
            }
        }
        if(node->type() == GraphNode::OBJ_V)
        {
            if(index.row() == 6)
            {
                QSpinBox *spinBox=qobject_cast<QSpinBox*>(editor);

                spinBox->setMaximum(1000);
                spinBox->setMinimum(100);

                int value = spinBox->value();
                model->setData(index,value);
            }else if(index.row() == 8)
            {
                QComboBox *comboBox = qobject_cast<QComboBox*>(editor);

                QString value = comboBox->currentText();
                model->setData(index,value);
            }else
            {
                QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);

                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);

                double value = spinBox->value();
                model->setData(index,value);
            }
        }
        if(node->type() == GraphNode::OBJ_G)
        {
            if(index.row() == 2)
            {
                QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);

                spinBox->setMaximum(1000);
                spinBox->setMinimum(300);

                int value=spinBox->value();
                model->setData(index,value);
            }else if(index.row() == 5)
            {
                QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);

                spinBox->setMaximum(1);
                spinBox->setMinimum(0);

                int value = spinBox->value();
                model->setData(index,value);
            }else
            {
                QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);

                spinBox->setMaximum(9999999999.9);
                spinBox->setMinimum(-9999999999.9);

                double value = spinBox->value();
                model->setData(index,value);
            }
        }
    }else
        QItemDelegate::setModelData(editor,model,index);
}
void DelegateTableManual::commitAndCloseEditor()
{
    QSpinBox *editor = qobject_cast<QSpinBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
void DelegateTableManual::commitAndCloseEditorDouble()
{
    QDoubleSpinBox *editor = qobject_cast<QDoubleSpinBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
void DelegateTableManual::commitAndCloseEditorComboBox()
{
    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);;
}
void DelegateTableManual::commitAndCloseEditorLine()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
void DelegateTableManual::setPageIndex(int index_,cl_Scene* scene)
{
    currentScene = scene;
    if(index_ < 0)
        index_ = 0;
    pageIndex = index_;
}
}
