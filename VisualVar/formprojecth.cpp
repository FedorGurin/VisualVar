#include "formprojecth.h"
#include "ui_formprojecth.h"

FormProjectH::FormProjectH(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormProjectH)
{
    ui->setupUi(this);
    scene=new QGraphicsScene();
    svgItem=new QGraphicsSvgItem(":/res/svg/scale");
    textScale=new QGraphicsTextItem("10km",svgItem);
    textScale->setPos(30,0);
    scene->addItem(svgItem);
    scene->addItem(textScale);
    ui->graphicsViewH->setScene(scene);

    aircraft=new GraphObject(tr("Н"),":/res/svg/aircraft",svgItem);
    aircraft->setScale(0.05);

    QGraphicsLineItem *line=new QGraphicsLineItem(40,200,50,200);
    scene->addItem(line);
    scene->addItem(aircraft);
    maxScale=10000;
}
void FormProjectH::updateScene()
{
    //! очищение всех объектов
    clearObj();
    //! расчет масштаба
    calcMaxScale();

    if(currentScene!=0)
    {
        moveToH(aircraft,currentScene->aircraft->currentY(),0);
        for(int i=0;i<currentScene->airTargets.size();i++)
        {
            airtarget.push_back(new GraphObject(tr("Ц")+QString::number(i+1),":/res/svg/t_up",svgItem));
            airtarget[i]->setScale(0.05);
            moveToH(airtarget[i],currentScene->airTargets[i]->currentY(),i+1);
            scene->addItem(airtarget[i]);
        }
    }
    //ui->graphicsViewH->update();
    scene->update();//graphicsViewH->update();
}
void FormProjectH::setCurrentScene(VisualVariant::cl_Scene* scene_)
{
    currentScene=scene_;
    clearObj();

    calcMaxScale();
    if(currentScene!=0)
    {
        moveToH(aircraft,currentScene->aircraft->currentY(),0);
        for(int i=0;i<currentScene->airTargets.size();i++)
        {
            airtarget.push_back(new GraphObject(tr("Ц")+QString::number(i+1),":/res/svg/t_up",svgItem));
            airtarget[i]->setScale(0.05);
            moveToH(airtarget[i],currentScene->airTargets[i]->currentY(),i+1);
            scene->addItem(airtarget[i]);
        }
    }
}
void FormProjectH::calcMaxScale()
{
    if(currentScene!=0)
    {
        maxScale=currentScene->aircraft->currentY();
        for(int i=0;i<currentScene->airTargets.size();i++)
        {
            if(maxScale<currentScene->airTargets[i]->currentY())
                maxScale=currentScene->airTargets[i]->currentY();
        }
    }
    if(maxScale<2500)
        maxScale=2500;
    else if(maxScale<5000)
        maxScale=5000;
    else if(maxScale<10000)
        maxScale=10000;
    else if(maxScale<15000)
        maxScale=15000;
    else if(maxScale<20000)
        maxScale=20000;

    textScale->setPlainText("H="+QString::number(maxScale/1000,'f',1));

}

void FormProjectH::clearObj()
{
    for(int i=0;i<airtarget.size();i++)
    {
        scene->removeItem(airtarget[i]);
    }
    airtarget.clear();
}

void FormProjectH::moveToH(GraphObject* obj,double h,int index)
{
    QRectF rec=obj->sceneBoundingRect();
    obj->setPos(QPointF(75-index*rec.width(),400-(h/maxScale)*400));
}

FormProjectH::~FormProjectH()
{
    delete ui;
}
