#include "modelDataTable.h"
#include <QIcon>

namespace VisualVariant
{


ModelDataTable::ModelDataTable(QVector<cl_Scene* > *scenes_,
                               QObject *parent):QAbstractTableModel(parent)
{
    scenes              = scenes_;
    pageIndex           = 0;
    currentScene        = nullptr;
    currentGraphNode    = nullptr;
    path                = "/";
}

QModelIndex ModelDataTable::index(int row,int column, const QModelIndex &parent )const
{
    if (!hasIndex(row, column, parent))   return QModelIndex();

    QModelIndex tempModelIndex = QModelIndex();

    if(pageIndex == 0)
        tempModelIndex = createIndex(row,column,(*scenes)[row]);
    else if(pageIndex == 1)
    {
        int sizeAirTarget    = currentScene->airObj.size();
        int sizeGroundTarget = currentScene->groundObj.size();
        if(row == 0)
            tempModelIndex = createIndex(row,column,currentScene->aircraft);
        if((row>=1) && (row<(sizeAirTarget+1)))
            tempModelIndex=createIndex(row,column,currentScene->airObj[row-1]);
        if((row>=(sizeAirTarget + 1)) && (row<(sizeGroundTarget + sizeAirTarget + 2)))
            tempModelIndex=createIndex(row,column,currentScene->groundObj[row-1-sizeAirTarget]);
    
    }else if(pageIndex == 2)
    {
        if(currentGraphNode != nullptr)
            tempModelIndex=createIndex(row,column,currentGraphNode);
    }else if(pageIndex == 3)
    {
        if(currentGraphNode!=nullptr)
            tempModelIndex=createIndex(row,column,currentGraphNode);
    }
    return tempModelIndex;
}
void ModelDataTable::setPageIndex(int index_)
{
    if(index_<0) index_=0;
    pageIndex=index_;
    checkPath();

    endResetModel();
}
void ModelDataTable::checkPath()
{
    path="/";

    if(pageIndex==1 && currentScene!=nullptr)         path+=currentScene->returnNameVariant();
    if(pageIndex==2 && currentGraphNode!=nullptr)     path+=currentScene->returnNameVariant()+"/"+currentGraphNode->name;
}
void ModelDataTable::changePageIndex(QModelIndex index)
{
    if(pageIndex==2) return ;
    pageIndex++;


    if(pageIndex==1)
        currentScene=(*scenes)[index.row()];
    if(pageIndex==2)
    {
        int row=index.row();

        int sizeAirTarget=currentScene->airObj.size();
        int sizeGroundTarget=currentScene->groundObj.size();

        if(row==0)
            currentGraphNode=currentScene->aircraft;

        if((row>=1) && (row<(sizeAirTarget+1)))
            currentGraphNode=currentScene->airObj[row-1];

        if((row>=(sizeAirTarget+1)) && (row<(sizeGroundTarget+sizeAirTarget+2)))
            currentGraphNode=currentScene->groundObj[row-1-sizeAirTarget];

    }
    if(pageIndex==3)
    {

    }
    checkPath();
    endResetModel();
}
void ModelDataTable::refreshModelData()
{
    endResetModel();
}
/*QModelIndex ModelDataTable::parent(const QModelIndex &child)const
{
    return child;
}*/
bool ModelDataTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if (!index.isValid())return false;

    if(pageIndex==0)
    {
        if(index.column()==0)
        {
            QString name=value.toString();
            if(name==tr("Да"))
                ((*scenes)[index.row()])->use=true;
            else if(name==tr("Нет"))
                ((*scenes)[index.row()])->use=false;
        }else if(index.column()==5)
        {
            ((*scenes)[index.row()])->setComment(value.toString());
        }

    }
    if(pageIndex==1)
    {
        if(index.column()==0)
        {
            QString name=value.toString();
            ObjectGraphNode* node=static_cast<ObjectGraphNode*>(index.internalPointer());
            if(name==tr("Да"))
                node->setEnable(true);
            else if(name==tr("Нет"))
                node->setEnable(false);


        }
    }

    if(pageIndex == 2)
    {
        if(index.column() == 1)
        {

                if(currentGraphNode->type() == GraphNode::E_AIRCRAFT)
                {
                    AircraftObject *tempAircraft = static_cast<AircraftObject* >(currentGraphNode);
                    if(index.row() == 0) tempAircraft->setVc(value.toDouble());
                    if(index.row() == 1) tempAircraft->setY(value.toDouble());
                    if(index.row() == 2) tempAircraft->setPsi(value.toDouble());
                    if(index.row() == 3) tempAircraft->setTeta(value.toDouble());
                    if(index.row() == 4)
                    {
                        if(value.toString() == tr("С земли"))
                            tempAircraft->setStartEarth(true);
                        else
                            tempAircraft->setStartEarth(false);
                    }


                }else if(currentGraphNode->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj* >(currentGraphNode);

                    if(index.row()==0) tempAirTarget->setV(value.toDouble());
                    if(index.row()==1) tempAirTarget->setY(value.toDouble());
                    if(index.row()==2) tempAirTarget->setPsi(value.toDouble());
                    if(index.row()==3) tempAirTarget->setTeta(value.toDouble());
                    if(index.row()==4) tempAirTarget->setFi(value.toDouble());
                    if(index.row()==5) tempAirTarget->setD(value.toDouble());
                    if(index.row()==6) tempAirTarget->setCode(value.toInt());



                    if(tempAirTarget->currentCode() != tempAirTarget->prevCode())
                    {
                        endResetModel();
                    }

                    //! отправим сигнал о том, что содержимое изменилось
                    currentScene->sendSignalUpdateValueObj();
                }else if(currentGraphNode->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(currentGraphNode);
                    if(index.row()==0) tempGroundTarget->setV(value.toDouble());
                    if(index.row()==1) tempGroundTarget->setPsi(value.toDouble());
                    if(index.row()==2) tempGroundTarget->setCode(value.toInt());
                    if(index.row()==3) tempGroundTarget->setXt(value.toDouble());
                    if(index.row()==4) tempGroundTarget->setZt(value.toDouble());
                    if(index.row()==5) tempGroundTarget->setSKS(value.toInt());
                }

        }
        if(index.column()==2)
        {

                if(currentGraphNode->type() == GraphNode::E_AIRCRAFT)
                {
                    AircraftObject *tempAircraft = static_cast<AircraftObject*> (currentGraphNode);
                    if(index.row()==0) tempAircraft->setCurMessV(value.toString());
                    if(index.row()==1) tempAircraft->setCurMessY(value.toString());
                    if(index.row()==2) tempAircraft->setCurMessPsi(value.toString());
                    if(index.row()==3) tempAircraft->setCurMessTeta(value.toString());

                }else if(currentGraphNode->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj*> (currentGraphNode);

                    if(index.row() == 0) tempAirTarget->setCurMessV(value.toString());
                    if(index.row() == 1) tempAirTarget->setCurMessY(value.toString());
                    if(index.row() == 2) tempAirTarget->setCurMessPsi(value.toString());
                    if(index.row() == 3) tempAirTarget->setCurMessTeta(value.toString());
                    if(index.row() == 4) tempAirTarget->setCurMessFi(value.toString());
                    if(index.row() == 5) tempAirTarget->setCurMessD(value.toString());
                    if(index.row() == 7) tempAirTarget->setCurMessLength(value.toString());
                }else if(currentGraphNode->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj*>(currentGraphNode);
                    if(index.row()==0) tempGroundTarget->setCurMessV(value.toString());
                    if(index.row()==1) tempGroundTarget->setCurMessPsi(value.toString());
                    //if(index.row()==2) tempGroundTarget->setCode(value.toInt());
                    if(index.row()==3) tempGroundTarget->setCurMessX(value.toString());
                    if(index.row()==4) tempGroundTarget->setCurMessZ(value.toString());
                }

        }
    }
    emit dataChanged(index,index);
    return true;
}
void  ModelDataTable::refreshModelData(QModelIndex index)
{
    emit dataChanged(index,index);
}

int  ModelDataTable::columnCount(const QModelIndex &parent)const
{
    Q_UNUSED(parent);
    if(pageIndex == 0) return 13;
    if(pageIndex == 1) return 12;
    if(pageIndex == 2) return 4;
    if(pageIndex == 3) return 4;

    return 0;
}
int ModelDataTable::rowCount(const QModelIndex &parent)const
{
    Q_UNUSED(parent);
    if(pageIndex == 0)
    {
        //! здесь нужно изменить
        return scenes->size();
    }
    else if(pageIndex == 1)
    {
        return currentScene->airObj.size()+
                1+
                currentScene->groundObj.size();
    }else if(pageIndex == 2)
    {
        if(currentGraphNode->type()==GraphNode::E_AIRCRAFT)
            {
                ObjectGraphNode *obj = static_cast<ObjectGraphNode *> (currentGraphNode);

                return 5 ;

            }
            if(currentGraphNode->type() == GraphNode::E_OBJ_V)
            {
              AirObj *tempAirTarget = static_cast<AirObj* >(currentGraphNode);
              if(tempAirTarget->currentCode() >= 600 && tempAirTarget->currentCode()<700)
              {
                  return 9+4;
              }

              return 9;
            }
            if(currentGraphNode->type() == GraphNode::E_OBJ_G)
            {
                return 6;
            }

    }else if(pageIndex == 3)
    {
        return 4;
    }
    return 0;
}
QVariant ModelDataTable::headerData(int section,Qt::Orientation orientation, int role)const
{
    if(role == Qt::DisplayRole)
    {
        if(pageIndex == 0)
        {
            if(orientation == Qt::Horizontal)
            {
                if(section == 0)return QString(tr("Активен"));
                if(section == 1)return QString(tr("№"));
                if(section == 2)return QString(tr("Объект"));
                if(section == 3)return QString(tr("ВО"));
                if(section == 4)return QString(tr("НО"));
                if(section == 5)return QString(tr("Комментарий"));
                if(section == 6)return QString(tr("Скорость"));
                if(section == 7)return QString(tr("Высота"));
                if(section == 8)return QString(tr("Курс"));
                if(section == 9)return QString(tr("Тангаж"));
                if(section == 10)return QString(tr("Тип старта"));
            }
        }
        if(pageIndex == 1)
        {
            if(orientation == Qt::Horizontal)
            {

                if(section==0)return QString(tr("Активен"));
                if(section==1)return QString(tr("Объект"));
                if(section==2)return QString(tr("Тип"));
                if(section==3)return QString(tr("Скорость"));
                if(section == 4)return QString(tr("Высота"));
                if(section == 5)return QString(tr("Курсовой угол"));
                if(section == 7)return QString(tr("Угол вертолет/цель"));
                if(section == 8)return QString(tr("Дальность"));
                if(section == 9)return QString(tr("Тип объекта"));

                if(section ==6)return QString(tr("Угол тангажа"));
                if(section ==10)return QString(tr("Координата X"));
                if(section ==11)return QString(tr("Координата Z"));

            }
        }else if(pageIndex == 2)
        {
            if(orientation == Qt::Horizontal)
            {
                if(section == 0)return QString(tr("Название"));
                if(section == 1)return QString(tr("Значение"));
                if(section == 2)return QString(tr("Ед. измерения"));
                if(section == 3)return QString(tr("Порядок"));
            }
        }else if(pageIndex == 3)
        {
            if(orientation == Qt::Horizontal)
            {
                if(section == 0)return QString(tr("Название"));
                if(section == 1)return QString(tr("Значение"));
                if(section == 2)return QString(tr("Ед. измерения"));
                if(section == 3)return QString(tr("Порядок"));
            }
        }
    }
    return QVariant();
}
Qt::ItemFlags ModelDataTable::flags(const QModelIndex &index) const
{
      if (!index.isValid())  return 0;
      if((pageIndex == 0 || pageIndex == 1) && index.column() == 0) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
      if(pageIndex == 0 && index.column() == 2) return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
      if(pageIndex == 2 && index.column() == 1)
      {
          if(currentGraphNode->type() == GraphNode::E_AIRCRAFT)
          {

          }
          if(currentGraphNode->type() == GraphNode::E_OBJ_V)
          {


              if((index.row() == 6 || index.row() == 7))
              {
                  if(index.row() == 6)
                      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
              }else
              {
                  if(index.row() == 7)
                      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
              }
          }
          return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
      }
      if(pageIndex == 2 && index.column() == 2)
          return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
      if(pageIndex == 0 && index.column() == 5)
          return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;



      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant ModelDataTable::data(const QModelIndex &index, int role) const
{
    switch(pageIndex)
    {
        case 0: {return page0(index,role);break;}
        case 1: {return page1(index,role);break;}
        case 2: {return page2(index,role);break;}
        default: QVariant();
    };
    return QVariant();
}
QVariant ModelDataTable::page0(const QModelIndex &index,int role) const
{
    cl_Scene* scene = static_cast<cl_Scene*>(index.internalPointer());

    if(scene != nullptr)
    {
        if (role == Qt::DisplayRole)
        {
            if(index.column() == 2)
            {
                return scene->returnNameVariant();
            }else if(index.column() == 1 )
            {
                return scene->index;
            }else if(index.column() == 0)
            {
                if(scene->use == true)
                    return QString(tr("Да"));
                else
                    return QString(tr("Нет"));
            }else if(index.column() == 3)
            {
                return scene->airObj.size();
            }else if(index.column() == 4)
            {
                return scene->groundObj.size();
            }else if(index.column() == 5)
            {
                return scene->comment();
            }else if(index.column() == 6)
            {
                return (QString::number(scene->aircraft->currentV())+QString(tr(" "))+scene->aircraft->curMessV());
            }else if(index.column() == 7)
            {
                return (QString::number(scene->aircraft->currentY())+QString(tr(" "))+scene->aircraft->curMessY());
            }else if(index.column() == 8)
            {
                return (QString::number(scene->aircraft->currentPsi())+QString(tr(" "))+scene->aircraft->curMessPsi());

            }else if(index.column()==9)
            {
                return (QString::number(scene->aircraft->currentTeta())+QString(tr(" "))+scene->aircraft->curMessTeta());
            }else if(index.column()==10)
            {
                return (QString::number(scene->aircraft->currentStart()) + QString(tr(" признак")));
            }else if(index.column()==11)
            {

            }else if(index.column()==12)
            {

            }
        }else if(role==Qt::BackgroundRole)
        {
            if(scene->use == false)
                return QBrush(QColor(Qt::red));
        }else if(role == Qt::DecorationRole)
        {
            if(index.column() == 2)
            {
                return QIcon(":/png/var");
            }
        }else if(role == Qt::TextAlignmentRole){
            return Qt::AlignCenter;
        }
    }
    return QVariant();
}

QVariant ModelDataTable::page1(const QModelIndex &index,int role) const
{
    if (role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            ObjectGraphNode* node = static_cast<ObjectGraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->isEnable() == true)
                    return QString(tr("Да"));
                else
                    return QString(tr("Нет"));
            }
        }else if(index.column() == 1)
        {
            GraphNode* node = static_cast<GraphNode*>(index.internalPointer());
            if(node != nullptr)
                return node->name;
        }else if(index.column() == 2)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->type() == GraphNode::E_AIRCRAFT)
                {
                    return QString(tr("Носитель"));
                }else if(node->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj* >(node);
                    return tempAirTarget->currentCodeStr();
                }else if(node->type() == GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget = static_cast<GroundObj* >(node);
                    return tempGroundTarget->currentCodeStr();
                }
            }
        }else if(index.column() == 3)
        {
            GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->type()==GraphNode::E_AIRCRAFT)
                {

                        AircraftObject *tempAircraft = static_cast<AircraftObject* >(node);
                        return (QString::number(tempAircraft->currentV())+QString(tr(" "))+tempAircraft->curMessV());

                }else if(node->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj* >(node);
                    return (QString::number(tempAirTarget->currentV())+QString(tr(" "))+tempAirTarget->curMessV());
                }else if(node->type() == GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget = static_cast<GroundObj* >(node);
                    return (QString::number(tempGroundTarget->currentV())+QString(tr(" "))+tempGroundTarget->curMessV());
                }
            }
        }else if(index.column() == 4)
        {
            GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->type() == GraphNode::E_AIRCRAFT)
                {

                        AircraftObject *tempAircraft = static_cast<AircraftObject*>(node);
                        return (QString::number(tempAircraft->currentY()) + QString(tr(" ")) + tempAircraft->curMessY());

                }else if(node->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj*>(node);
                    return (QString::number(tempAirTarget->currentY()) + QString(tr(" ")) + tempAirTarget->curMessY());
                }
            }

        }else if(index.column() == 5)
        {
            GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->type() == GraphNode::E_AIRCRAFT)
                {

                        AircraftObject *tempAircraft = static_cast<AircraftObject*>(node);
                        return (QString::number(tempAircraft->currentPsi()) + QString(" ") + tempAircraft->curMessPsi());

                }else if(node->type() == GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget = static_cast<AirObj*>(node);
                    return (QString::number(tempAirTarget->currentPsi())+QString(" ")+tempAirTarget->curMessPsi());
                }else if(node->type() == GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget = static_cast<GroundObj*>(node);
                    return (QString::number(tempGroundTarget->currentPsi())+QString(" ")+tempGroundTarget->curMessPsi());
                }
            }
        }else if(index.column() == 6)
        {
            GraphNode* node = static_cast<GraphNode*>(index.internalPointer());

            if(node != nullptr)
            {
                if(node->type() == GraphNode::E_OBJ_V)
                {

                        AirObj *tempAirTarget = static_cast <AirObj*>(node);
                        return (QString::number(tempAirTarget->currentTeta())+QString(" ")+tempAirTarget->curMessTeta());

                }
            }
        }else if(index.column() == 7)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node!=nullptr)
            {
                if(node->type()==GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget=static_cast<AirObj* >(node);
                    return (QString::number(tempAirTarget->currentFi())+QString(" ")+tempAirTarget->curMessFi());
                }
            }
        }else if(index.column()==8)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node!=nullptr)
            {
                if(node->type()==GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget=static_cast<AirObj* >(node);
                    return (QString::number(tempAirTarget->currentD())+QString(" ")+tempAirTarget->curMessD());
                }
            }
        }else if(index.column()==9)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node!=nullptr)
            {
                if(node->type()==GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget=static_cast<AirObj* >(node);
                    return tempAirTarget->currentCode();
                }else if(node->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(node);
                    return tempGroundTarget->currentCode();
                }
            }
        }else if(index.column()==10)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node!=nullptr)
            {
                if(node->type()==GraphNode::E_AIRCRAFT)
                {

                }else if(node->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(node);
                    return (QString::number(tempGroundTarget->currentX())+QString(" ")+tempGroundTarget->curMessX());
                }
            }
        }else if(index.column()==11)
        {
            GraphNode* node=static_cast<GraphNode*>(index.internalPointer());

            if(node!=nullptr)
            {
                if(node->type()==GraphNode::E_AIRCRAFT)
                {

                }else if(node->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(node);
                    return (QString::number(tempGroundTarget->currentZ())+QString(" ")+tempGroundTarget->curMessZ());
                }
            }
        }
    }else if(role == Qt::BackgroundRole)
    {
        ObjectGraphNode* node=static_cast<ObjectGraphNode*>(index.internalPointer());

        if(node != nullptr)
        {
            if(node->isEnable()==false)
                return QBrush(QColor(Qt::red));
        }
    }else if(role == Qt::TextAlignmentRole){
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant ModelDataTable::page2(const QModelIndex &index,int role) const
{
    if (role == Qt::DisplayRole)
    {

            // варианты с начальными условиями
            if(index.column()==0)
            {
                if(currentGraphNode->type()==GraphNode::E_AIRCRAFT)
                {
                    AircraftObject *tempAircraft=static_cast<AircraftObject* >(currentGraphNode);
                    if(index.row()==0) return QString(tr("Скорость"));
                    if(index.row()==1) return QString(tr("Высота"));
                    if(index.row()==2) return QString(tr("Курсовой угол"));

                    if(index.row()==3) return QString(tr("Тангаж"));
                    if(index.row()==4) return QString(tr("Тип старта"));


                }else if(currentGraphNode->type()==GraphNode::E_OBJ_V)
                {
                     AirObj *tempAirTarget=static_cast<AirObj* >(currentGraphNode);
                    if(index.row()==0) return QString(tr("Скорость"));
                    if(index.row()==1) return QString(tr("Высота"));
                    if(index.row()==2) return QString(tr("Курсовой угол"));
                    if(index.row()==3) return QString(tr("Угол тангажа"));
                    if(index.row()==4) return QString(tr("Угол вертолет/объект"));
                    if(index.row()==5) return QString(tr("Дальность"));
                    if(index.row()==6) return QString(tr("Тип объекта"));
                    if(index.row()==7) return QString(tr("Длина объекта"));
                    if(index.row()==8) return QString(tr("Тип или длина"));

                    if(tempAirTarget->currentCode()>=600 && tempAirTarget->currentCode()<700)
                    {
                        if(index.row()==9) return QString(tr("Время отделения"));
                        if(index.row()==10) return QString(tr("Скорость отделения"));
                        if(index.row()==11) return QString(tr("Идент. носителя"));
                        if(index.row()==12) return QString(tr("Индекс объекта"));
                    }
                }else if(currentGraphNode->type()==GraphNode::E_OBJ_G)
                {
                    if(index.row()==0) return QString(tr("Скорость"));
                    if(index.row()==1) return QString(tr("Курсовой угол"));
                    if(index.row()==2) return QString(tr("Тип объекта"));
                    if(index.row()==3) return QString(tr("Координата X"));
                    if(index.row()==4) return QString(tr("Координата Z"));
                    if(index.row()==5) return QString(tr("CК вертолета"));
                }
            }
            if(index.column()==1)
            {
                if(currentGraphNode->type()==GraphNode::E_AIRCRAFT)
                {
                    AircraftObject *tempAircraft=static_cast<AircraftObject* >(currentGraphNode);
                    if(index.row()==0) return tempAircraft->currentV();
                    if(index.row()==1) return tempAircraft->currentY();
                    if(index.row()==2) return tempAircraft->currentPsi();
                    if(index.row()==3) return tempAircraft->currentTeta();

                    if(index.row()==4)
                    {
                        if(tempAircraft->currentStart()==true)
                            return QString(tr("С земли"));
                        else
                            return QString(tr("В воздухе"));
                    }


                }
                else if(currentGraphNode->type()==GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget=static_cast<AirObj* >(currentGraphNode);

                    if(index.row()==0) return tempAirTarget->currentV();
                    if(index.row()==1) return tempAirTarget->currentY();
                    if(index.row()==2) return tempAirTarget->currentPsi();
                    if(index.row()==3) return tempAirTarget->currentTeta();
                    if(index.row()==4) return tempAirTarget->currentFi();
                    if(index.row()==5) return tempAirTarget->currentD();
                    if(index.row()==6) return tempAirTarget->currentCode();

                }else if(currentGraphNode->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(currentGraphNode);
                    if(index.row()==0) return tempGroundTarget->currentV();
                    if(index.row()==1) return tempGroundTarget->currentPsi();
                    if(index.row()==2) return tempGroundTarget->currentCode();
                    if(index.row()==3) return tempGroundTarget->currentX();
                    if(index.row()==4) return tempGroundTarget->currentZ();
                    if(index.row()==5) return tempGroundTarget->currentSKS();
                }
            }
            if(index.column()==2)
            {
                if(currentGraphNode->type() == GraphNode::E_AIRCRAFT)
                {
                     AircraftObject *tempAircraft=static_cast<AircraftObject* >(currentGraphNode);

                    if(index.row()==0) return tempAircraft->curMessV();
                    if(index.row()==1) return tempAircraft->curMessY();
                    if(index.row()==2) return tempAircraft->curMessPsi();
                    if(index.row()==3) return tempAircraft->curMessTeta();

                    if(index.row()==4) return QString(tr("признак"));


                }else if(currentGraphNode->type()==GraphNode::E_OBJ_V)
                {
                    AirObj *tempAirTarget=static_cast<AirObj* >(currentGraphNode);

                    if(index.row()==0) return tempAirTarget->curMessV();
                    if(index.row()==1) return tempAirTarget->curMessY();
                    if(index.row()==2) return tempAirTarget->curMessPsi();
                    if(index.row()==3) return tempAirTarget->curMessTeta();
                    if(index.row()==4) return tempAirTarget->curMessFi();
                    if(index.row()==5) return tempAirTarget->curMessD();
                    if(index.row()==6) return QString(tr("код"));
                    if(index.row()==7) return tempAirTarget->curMessLength();
                    if(index.row()==8) return QString(tr("признак"));
                    if(tempAirTarget->currentCode()>=600 && tempAirTarget->currentCode()<700)
                    {
                        if(index.row()==9) return tempAirTarget->curMessTime();
                        if(index.row()==10) return tempAirTarget->curMessV();
                        if(index.row()==11) return QString(tr("код"));
                        if(index.row()==12) return QString(tr("код"));
                    }
                }else if(currentGraphNode->type()==GraphNode::E_OBJ_G)
                {
                    GroundObj *tempGroundTarget=static_cast<GroundObj* >(currentGraphNode);

                    if(index.row()==0) return tempGroundTarget->curMessV();
                    if(index.row()==1) return tempGroundTarget->curMessPsi();
                    if(index.row()==2) return QString(tr("число"));
                    if(index.row()==3) return tempGroundTarget->curMessX();
                    if(index.row()==4) return tempGroundTarget->curMessZ();
                    if(index.row()==5) return QString(tr("признак"));
                }
            }

    }else if(role == Qt::BackgroundRole)
    {

            if(currentGraphNode->type() == GraphNode::E_AIRCRAFT)
            {

            }else if(currentGraphNode->type() == GraphNode::E_OBJ_V)
            {


                    if(index.row() == 7) return QBrush(QColor(Qt::red));

            }

    }else if(role == Qt::TextAlignmentRole){
        if(index.column()==1)
            return Qt::AlignRight|Qt::AlignVCenter;
        else
            return Qt::AlignCenter;
    }
    return QVariant();
}

}