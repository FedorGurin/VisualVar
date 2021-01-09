#ifndef FORMPROJECTH_H
#define FORMPROJECTH_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>

#include "cl_scene.h"
namespace Ui {
class FormProjectH;
}

class GraphObject:public QGraphicsSvgItem
{
public:
    GraphObject(QString name_,QString nameFile,QGraphicsItem *parent):QGraphicsSvgItem(nameFile,parent)
    {
        name=new QGraphicsTextItem(name_,this);
        name->setScale(20.0);
        name->setPos(0.0,270);
    }
private:
    QGraphicsTextItem *name;
};

class FormProjectH : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormProjectH(QWidget *parent = nullptr);
    ~FormProjectH();
    //! указатель на текущую сцену
    void setCurrentScene(cl_Scene* scene_);
    //! переместить объект на указанную высоту
    void moveToH(GraphObject* obj, double h, int index);
    //! очистка объектов
    void clearObj();
    //! расчитать масштаб по высоте
    void calcMaxScale();
public slots:
    //! обновить форму
    void updateScene();
private:
    Ui::FormProjectH *ui;
    QGraphicsScene *scene;
    QGraphicsSvgItem *svgItem;
    QGraphicsTextItem *textScale;
    cl_Scene* currentScene;
    GraphObject *aircraft;
    QVector<GraphObject* > airtarget;
    double maxScale;
};

#endif // FORMPROJECTH_H
