#include "formprojecth.h"
#include "ui_formprojecth.h"

FormProjectH::FormProjectH(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormProjectH)
{
    ui->setupUi(this);
    scene     = new QGraphicsScene();
    svgItem   = new QGraphicsSvgItem(":/res/svg/scale");
    textScale = new QGraphicsTextItem("10km",svgItem);
    textScale->setPos(30,0);

    scene->addItem(svgItem);
    scene->addItem(textScale);

    ui->graphicsViewH->setScene(scene);

    aircraft = new GraphObject(tr("Н"),":/res/svg/aircraft",svgItem);
    aircraft->setScale(0.05);

    QGraphicsLineItem *line = new QGraphicsLineItem(40,200,50,200);
    scene->addItem(line);
    scene->addItem(aircraft);
    maxScale = 10000;
}
void FormProjectH::updateScene()
{
    //! очищение всех объектов
    clearObj();
    //! расчет масштаба
    calcMaxScale();

    if(currentScene != nullptr)
    {
        moveToH(aircraft,currentScene->aircraft->currentY(),0);
        for(int i=0;i<currentScene->airTargets.size();i++)
        {
            airtarget.push_back(new GraphObject(tr("ВО ") + QString::number(i+1),":/res/svg/t_up",svgItem));
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
    if(currentScene == scene_)
        return;
    currentScene=scene_;
    updateScene();
}
void FormProjectH::calcMaxScale()
{
    if(currentScene != nullptr)
    {
        maxScale = currentScene->aircraft->currentY();
        for(auto i:currentScene->airTargets)
        {
            if(maxScale < i->currentY())
                maxScale = i->currentY();
        }
    }
    if(maxScale < 2500)
        maxScale = 2500;
    else if(maxScale < 5000)
        maxScale = 5000;
    else if(maxScale < 10000)
        maxScale =  10000;
    else if(maxScale < 15000)
        maxScale = 15000;
    else if(maxScale < 20000)
        maxScale = 20000;

    textScale->setPlainText("H = "+QString::number(maxScale/1000,'f',1));

}

void FormProjectH::clearObj()
{
    for(auto i: airtarget)
    {
        scene->removeItem(i);
    }
    airtarget.clear();
}

void FormProjectH::moveToH(GraphObject* obj,double h,int index)
{
    QRectF rec = obj->sceneBoundingRect();
    obj->setPos(QPointF( 75 - index * rec.width(),400 - (h / maxScale) * 400));
}

FormProjectH::~FormProjectH()
{
    delete ui;
}
