#ifndef COMMONENGINEDATA_H
#define COMMONENGINEDATA_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QThread>
#include <QVector>

//! результат выполнения запроса
class TRequestEvent
{
public:

    enum TYPE_STATUS
    {
        ADDED_TO_QUEUE  = 1,/*сообщение о том, что запрос добавлен в обработку*/
        PROCESSED       = 2,/*сообщение обработано*/
        PROCESSED_FAULT = 3,/*сообщение не обработано, смотри лог ошибок*/
        WRONG_REQUEST   = 4,/*ошибка в запросе*/
        IGNORER         = 5,/*запрос не поддерживается, модуль его проигнорировал */
        TIMEOUT         = 6 /*исчерпан таймаут*/
    };

    TRequestEvent()
    {
        sender      = "unknown";
        reciver     = "unknown";

        name.clear();
        value.clear();

        uid     = qrand();
        status  = IGNORER;

        sub_status.clear();
        sub_uid.clear();
    }

    //! модуль отправитель
    QString sender;
    //! модуль получатель
    QString reciver;
    //! набор строк запроса
    QStringList name;
    //! набор значений
    QStringList value;
    //! идентификатор запроса
    QVector<int> sub_uid;
    //! статус обработки
    QVector<TYPE_STATUS> sub_status;
    //! глобальный статус запроса
    int uid;
    //! глобальный статус
    TYPE_STATUS status;
};
//! формат обращения к обработчику запросов
class TCommonRequest
{
public:
    TCommonRequest()
    {
        listName.clear();
        listValue.clear();        
        listUid.clear();
        uid     = qrand();
        sender  = "unknown";
        reciver = "unknown";
    }
    //! добавить имя запроса
    void append(QString name,QString value="")
    {
        listName.append(name);
        listValue.append(value);
        listUid.push_back(qrand());
    }
    //! глобальный уникальный идентификатор
    quint64 uidAt(int index)
    {
        if(index>=listName.size())
            index=listName.size()-1;

        return listUid[index];
    }
    //! имя запроса
    QString nameAt(int index)
    {
        if(index>=listName.size())
            index=listName.size()-1;

        return listName[index];
    }

    //! значение запроса
    QString valueAt(int index)
    {
        if(index>=listName.size())
            index=listName.size()-1;

        return listValue[index];
    }

    void setValueAt(int index,QString value)
    {
        if(index>=listName.size())
            index=listName.size()-1;

        listValue[index] = value;
    }

    //! кол-во подзапросов
    int size(){return listName.size();}
    bool isEmpty(){return listName.isEmpty();}
    //! глобальный идентификатор
    int guid(){return uid;}
    //! отправитель
    QString isSender(){return sender;}
    QString isReciver(){return reciver;}
    void setReciver(QString name){reciver=name;}
    void setSender(QString name){sender=name;}
private:
    //! список названий запросов
    QStringList listName;
    //! список аргументов
    QStringList listValue;
    //! имя модуля отправителя
    QString sender;
    QString reciver;
    //! идентификатор запроса
    QVector<int> listUid;
    //! глобальный идентфикатор
    int uid;
};

class IEngineData: public QThread
{
    Q_OBJECT
public:
    enum TYPE_REQUEST
    {
        SYNCH_ENGINE,/*синхронная обработка запроса*/
        ASYNCH_ENGINE,/*асинхронная обработка запроса*/
        CYCLIC_ENGINE /*циклический запрос*/
    };
    IEngineData(QObject* parent=0):QThread(parent){}
    //! задать данные
    virtual TRequestEvent setValue(TCommonRequest request, TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;
        Q_UNUSED(request);
        Q_UNUSED(type);
        return answer0;
    }
    //! задать данные
    /*virtual uint setValue(QStringList idName,QStringList value, TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        Q_UNUSED(idName);
        Q_UNUSED(value);
        Q_UNUSED(type);
        return 0;
    }*/
    //! выполнение специальной команды
   /* virtual uint specialCommand(QStringList str1,QStringList str2,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        Q_UNUSED(str1);
        Q_UNUSED(str2);
        Q_UNUSED(type);
        return 0;
    }*/
    //! получить данные
    virtual TRequestEvent getValue(TCommonRequest request,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;
        Q_UNUSED(request);
        Q_UNUSED(type);
        return answer0;
    }
    //! выполнение команды для чтения данных
    /*virtual uint getCommand(QStringList strCommand,QStringList& values,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        Q_UNUSED(strCommand);
        Q_UNUSED(values);
        Q_UNUSED(type);
        return 0;
    }*/
    //! выполнение команды для записи данных
    virtual TRequestEvent command(TCommonRequest request, TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;
        Q_UNUSED(request);
        Q_UNUSED(type);
        return answer0;
    }

    //! имя программного модуля
    QString senderName()
    {
        return senderName_;
    }
    void setSenderName(QString name)
    {
        senderName_=name;
    }
signals:
    //! сигнал о выполнении операции
    void reciveEventsRequest(TRequestEvent eventRequest);
private:
    QString senderName_;
};

//! класс скрывающий разнородные данные(к этому классу происходит обращение)
class CommonEngineData : public IEngineData
{
    Q_OBJECT
public:
    explicit CommonEngineData(QObject *parent = 0);
    void addEngine(IEngineData* eng)
    {
        engines.push_back(eng);
        connect(eng,SIGNAL(signalEventsRequest(TRequestEvent)),this,SIGNAL(signalEventsRequest(TRequestEvent)));       
    }

    //! выполнение команды для чтения данных
    /*virtual uint getCommand(QStringList strCommand,QStringList& values,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        for(int i=0;i<engines.size();i++)
        {
            uint id=engines[i]->getCommand(strCommand,values,type);
            if(id>0)
                return id;
        }
        return 0;
    }*/
    virtual TRequestEvent command(TCommonRequest request,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;
        for(int i=0;i<engines.size();i++)
        {
            TRequestEvent answer0=engines[i]->command(request,type);
            if(answer0.status != TRequestEvent::IGNORER)
                return answer0;
        }
        return answer0;
    }

    //! выполнение специальной команды
    /*virtual uint specialCommand(QStringList str1,QStringList str2,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        for(int i=0;i<engines.size();i++)
        {
            uint id=engines[i]->specialCommand(str1,str2,type);
            if(id>0)  return id;
        }
        return 0;
    }*/

    virtual TRequestEvent setValue(TCommonRequest request,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;

        for(int i=0;i<engines.size();i++)
        {
            TRequestEvent answer0=engines[i]->setValue(request,type);
            if(answer0.status != TRequestEvent::IGNORER)
                return answer0;
        }
        return answer0;
    }

   /* virtual uint setValue(QStringList idName,QStringList value,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        for(int i=0;i<engines.size();i++)
        {
            uint id=engines[i]->setValue(idName,value,type);
            if(id>0) return id;
        }
        return 0;
    }*/
    virtual TRequestEvent getValue(TCommonRequest request,TYPE_REQUEST type=ASYNCH_ENGINE)
    {
        TRequestEvent answer0;

        for(int i=0;i<engines.size();i++)
        {
            TRequestEvent answer0=engines[i]->getValue(request,type);
            if(answer0.status != TRequestEvent::IGNORER)
                return answer0;
        }
        return answer0;
    }
    QList<IEngineData*> engines;
};

#endif // COMMONENGINEDATA_H
