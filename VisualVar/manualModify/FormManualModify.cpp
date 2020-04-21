#include "FormManualModify.h"
#include "ui_FormManualModify.h"
#include <QRegExpValidator>
#include <QKeyEvent>

namespace VisualVariant
{
cl_MouseFilter::cl_MouseFilter(QObject* pobj,QWidget *slotForm_):QObject(pobj)
{
    slotForm = slotForm_;
}
bool cl_MouseFilter::eventFilter(QObject* pobj,QEvent *event)
{
    Q_UNUSED(pobj);

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent*>(event);
        if(key->key() == Qt::Key_Delete)
        {
            FormManualModify *p=qobject_cast<FormManualModify* >(slotForm);
            if(p != nullptr)
            {
                p->slotKeyboardDel();
                return true;
            }
        }else if(key->key() == Qt::Key_Insert)
        {
            FormManualModify *p = qobject_cast<FormManualModify* >(slotForm);
            if(p != nullptr)
            {
                p->slotKeyboardInsert();
                return true;
            }else
                return false;
        }
    }
    return false;
}
FormManualModify::FormManualModify( QVector<cl_Scene* > *scenes_,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormManualModify)
{
    ui->setupUi(this);
    scenes          = scenes_;
    modelData       = new ModelDataTable(scenes_,this);
    delegateTable   = new DelegateTableManual(1,modelData);
    listWidget      = new QListWidget;

    QHeaderView *qhv;
    qhv = ui->tableView->horizontalHeader();
    qhv->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView->setModel(modelData);
    selection = new QItemSelectionModel(modelData);
    ui->tableView->setSelectionModel(selection);
    ui->tableView->setItemDelegate(delegateTable);
    ui->tableView->installEventFilter(new cl_MouseFilter(ui->tableView,this));
    ui->lineEditComment->setValidator(new  QRegExpValidator(QRegExp("[A-Za-z0-9 ]*"),this));

    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotMouseDoubleClicked(QModelIndex)));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(slotMouseClicked(QModelIndex)));

    connect(ui->pushButtonBack,     SIGNAL(clicked()),this,SLOT(slotPushButtonBack()));
    connect(ui->pushButtonAdd,      SIGNAL(clicked()),this,SLOT(slotPushButtonAdd()));
    connect(ui->pushButtonBegin,    SIGNAL(clicked()),this,SLOT(slotPushButtonBegin()));
    connect(ui->pushButtonDelete,   SIGNAL(clicked()),this,SLOT(slotPushButtonDelete()));
    connect(ui->pushButtonClone,    SIGNAL(clicked()),this,SLOT(slotPushButtonClone()));
    connect(ui->pushButtonAttach,   SIGNAL(clicked()),this,SIGNAL(signalAttach()));

    connect(listWidget,             SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotDoubleClickedListWidget(QListWidgetItem*)));

    //connect(listWidget,SIGNAL(clicked(QListWidgetItem*)),this,SLOT(clicked(QListWidgetItem*)));

}
void FormManualModify::slotDoubleClickedListWidget(QListWidgetItem* item)
{
    if(modelData->currentPageIndex() == 1)
    {
        if(item->text() == tr("Воздушный объект"))
        {
            QPointF pF = modelData->currentScene->aircraft->posC();
            modelData->currentScene->createNewAirTarget(pF);

        }
        if(item->text() == tr("Наземный объект"))
        {
            QPointF pF = modelData->currentScene->aircraft->posC();
            modelData->currentScene->createNewGroundTarget(pF);
        }
    }
    if(modelData->currentPageIndex() == 0)
    {
        if(item->text() == tr("Вариант начальных условий"))  emit createNewScene(false);
        //if(item->text()==tr("Вариант движения по кругам")) emit createNewScene(true);
    }

    modelData->refreshModelData();

    listWidget->close();
    listWidget->clear();
}
FormManualModify::~FormManualModify()
{
    delete ui;
}
void FormManualModify::setLastNameFile(QString name)

{
    ui->lineEditLastFileName->setText(name);
}

void FormManualModify::setComment(QString name)
{
    ui->lineEditComment->setText(name);
}

QString FormManualModify::comment()
{
    return ui->lineEditComment->text();
}
void FormManualModify::slotPushButtonBegin()
{
    modelData->setPageIndex(0);
    delegateTable->setPageIndex(0,modelData->currentScene);

    ui->lineEditPath->setText(modelData->path);

    ui->pushButtonAdd->setEnabled(true);
    ui->pushButtonDelete->setEnabled(true);
    ui->pushButtonClone->setEnabled(true);
}
void FormManualModify::slotPushButtonClone()
{
    QItemSelectionModel* select = ui->tableView->selectionModel();
    QModelIndexList list = select->selectedRows(0);
    for(auto i:list)
    {
        if(modelData->currentPageIndex() == 0)
        {
            cl_Scene* scene=static_cast<cl_Scene*>(i.internalPointer());
            emit cloneScene(scene);
        }
        if(modelData->currentPageIndex() == 1)
        {
             ObjectGraphNode* node = static_cast<ObjectGraphNode*>(i.internalPointer());
             if(node->type()==GraphNode::TARGET_V)
             {
                 AirTargetObject* airtarget = static_cast<AirTargetObject*> (node);
                 modelData->currentScene->cloneAirTarget(airtarget);
             }else if(node->type() == GraphNode::TARGET_G)
             {
                 GroundTargetObject* groundtarget = static_cast<GroundTargetObject*> (node);
                 modelData->currentScene->cloneGroundTarget(groundtarget);
             }
        }
        //resetModelData();
        modelData->refreshModelData();
    }
}

void FormManualModify::slotPushButtonAdd()
{
    listWidget->close();

    if(modelData->currentPageIndex() == 1 /*||
       modelData->currentPageIndex()==0*/)
    {
        listWidget->clear();
        if(modelData->currentScene->circleVariant == false)
        {
            listWidget->setWindowTitle(tr("Добавить объект"));
            listWidget->addItem(new QListWidgetItem(QIcon(":/res/svg/target"),tr("Воздушный объект")));
            listWidget->addItem(new QListWidgetItem(QIcon(":/res/svg/gtarget"),tr("Наземный объект")));
        }
    }
    if(modelData->currentPageIndex()==0)
    {
        listWidget->clear();
        listWidget->setWindowTitle(tr("Добавить вариант"));

        listWidget->addItem(new QListWidgetItem(QIcon(":/png/var"),tr("Вариант начальных условий")));
        //listWidget->addItem(new QListWidgetItem(QIcon(":/png/circ"),tr("Вариант движения по кругам")));
    }
    if(modelData->currentPageIndex() == 0 || modelData->currentPageIndex() == 1)
    {
        listWidget->move(mapToGlobal(ui->pushButtonAdd->pos()));
        listWidget->setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
        listWidget->show();
    }
    if(modelData->currentScene != nullptr && modelData->currentPageIndex() != 0)
    {
        if(modelData->currentScene->circleVariant == true)
        {
            listWidget->close();
        }
    }
}
void FormManualModify::slotPushButtonDelete()
{
    QItemSelectionModel* select=ui->tableView->selectionModel();
    QModelIndexList list=select->selectedRows(0);
    for(auto i:list)
    {
        //! нужно удалить вариант
        if(modelData->currentPageIndex() == 0)
        {
            cl_Scene* scene=static_cast<cl_Scene*>(i.internalPointer());

            //scene->view->close();
            //scenes->remove(scenes->indexOf(scene));
            ////delete scene;
            //scene->use=true;
            //! пересчет индексов
            int index=scenes->indexOf(scene);
            if(index!=-1)
            {
                for(int i=index;i<scenes->size();i++)
                {
                    ((*scenes)[i])->index=((*scenes)[i])->index - 1;
                }
            }
              scene->subW->close();
        }
        if(modelData->currentPageIndex() == 1)
        {
             ObjectGraphNode* node = static_cast<ObjectGraphNode*>(i.internalPointer());
             if(node->type() == GraphNode::TARGET_V)
             {
                 AirTargetObject* airtarget = static_cast<AirTargetObject*> (node);
                 modelData->currentScene->airTargets.removeAt(modelData->currentScene->airTargets.indexOf(airtarget));
                 delete airtarget;
             }else if(node->type() == GraphNode::TARGET_G)
             {
                 GroundTargetObject* groundtarget = static_cast<GroundTargetObject*> (node);
                 modelData->currentScene->groundTargets.removeAt(modelData->currentScene->groundTargets.indexOf(groundtarget));
                 delete groundtarget;
             }
        }
        //resetModelData();
        modelData->refreshModelData();
    }
}
void FormManualModify::slotMouseClicked(QModelIndex index)
{
    if(modelData->currentPageIndex() == 0)
    {
        emit currentActiveWindow(((*scenes)[index.row()])->view->windowTitle());
    }else
    {
        emit currentActiveWindow(modelData->currentScene->view->windowTitle());
    }
    ui->tableView->setFocus();
}
void FormManualModify::slotMouseDoubleClicked(QModelIndex index)
{
  //if((modelData->currentPageIndex()==0 || modelData->currentPageIndex()==1 ) && (index.column()==0 || index.column()==4)) return;
    if((modelData->currentPageIndex() == 0 &&
       (index.column() == 0 || index.column() == 4)) ||
       (modelData->currentPageIndex() == 1 && index.column() == 0)) return;

    modelData->changePageIndex(index);
    delegateTable->setPageIndex(modelData->currentPageIndex(),modelData->currentScene);

    ui->lineEditPath->setText(modelData->path);

    if(modelData->currentPageIndex()==2)
    {
        ui->pushButtonAdd->setEnabled(false);
        ui->pushButtonDelete->setEnabled(false);
        ui->pushButtonClone->setEnabled(false);
    }else
    {
        ui->pushButtonAdd->setEnabled(true);
        ui->pushButtonDelete->setEnabled(true);
        ui->pushButtonClone->setEnabled(true);
    }
}
void FormManualModify::slotPushButtonBack()
{
    //! формируем путь
    modelData->setPageIndex(modelData->currentPageIndex() - 1);
    delegateTable->setPageIndex(modelData->currentPageIndex() - 1,modelData->currentScene);

    ui->lineEditPath->setText(modelData->path);
    if(modelData->currentPageIndex() == 2)
    {
        ui->pushButtonAdd->setEnabled(false);
        ui->pushButtonDelete->setEnabled(false);
        ui->pushButtonClone->setEnabled(false);
    }else
    {
        ui->pushButtonAdd->setEnabled(true);
        ui->pushButtonDelete->setEnabled(true);
        ui->pushButtonClone->setEnabled(true);
    }
}
void FormManualModify::deleteScene(int)
{
    modelData->setPageIndex(0);
}
void FormManualModify::resetModelData()
{
    modelData->refreshModelData();
}
void FormManualModify::resetModelData(QModelIndex index)
{
    modelData->refreshModelData(index);
}
void FormManualModify::slotKeyboardDel()
{
    QItemSelectionModel* select=ui->tableView->selectionModel();
    QModelIndexList list = select->selectedRows(0);
    for(auto i:list)
    {
        if(modelData->currentPageIndex() == 0)
        {
            cl_Scene* scene = static_cast<cl_Scene*>(i.internalPointer());
            scene->use = false;
        }
        if(modelData->currentPageIndex() == 1)
        {
             ObjectGraphNode* node = static_cast<ObjectGraphNode*>(i.internalPointer());
             node->setEnable(false);
        }
        modelData->refreshModelData(i);
    }
}
void FormManualModify::slotKeyboardInsert()
{
    QItemSelectionModel* select=ui->tableView->selectionModel();
    QModelIndexList list=select->selectedRows(0);
    for(auto i:list)
    {
        if(modelData->currentPageIndex() == 0)
        {
            cl_Scene* scene=static_cast<cl_Scene*>(i.internalPointer());
            scene->use=true;
        }
        if(modelData->currentPageIndex() == 1)
        {
             ObjectGraphNode* node=static_cast<ObjectGraphNode*>(i.internalPointer());
             node->setEnable(true);
        }
        modelData->refreshModelData(i);
    }
}

void FormManualModify::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
}
