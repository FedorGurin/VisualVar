#pragma once

#include "nodeVisual.h"


class FactoryObj
{
public:
    //! создание объекта
    GraphNode* createObj(GraphNode::TypeGraphNode type,QGraphicsItem *parent);
    //! точка входа(инстанциирование)
    static FactoryObj* obj();
    FactoryObj* operator () ()
    {
        return fObj;
    }
protected:
    FactoryObj();
private:
    //! объект
    static FactoryObj* fObj;
};
