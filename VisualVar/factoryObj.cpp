#include "factoryObj.h"

#include "./nodes/ourObj/ourObj.h"

FactoryObj* FactoryObj::fObj = nullptr;
FactoryObj::FactoryObj()
{

}
FactoryObj* FactoryObj::obj()
{
    if(fObj == nullptr)
        fObj = new FactoryObj;

    return fObj;
}
GraphNode* FactoryObj::createObj(GraphNode::TypeGraphNode type,QGraphicsItem *parent)
{
    GraphNode *graphObj = nullptr;
    switch(type)
    {
        case GraphNode::TypeGraphNode::E_HELLICOPTER : {graphObj = new AircraftObject("Наш вертолет",":/res/svg/aircraft", parent);break;}
        //case GraphNode::TypeGraphNode::E_OBJ_V : {graphObj = new AirObj("Наш вертолет",":/res/svg/aircraft", parent);break;}
    };

    return graphObj;

}
