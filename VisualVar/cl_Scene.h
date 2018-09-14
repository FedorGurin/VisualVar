/*
 * cl_Scene.h
 *
 *  Created on: 16.06.2009
 *      Author: Fedor
 */

#ifndef CL_SCENE_H_
#define CL_SCENE_H_

#include <QSvgRenderer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QDomElement>
#include <QObject>
#include <QMdiSubWindow>

#include "gscene.h"
#include "gview.h"
#include "NodeVisual.h"
#include "formstatusbar.h"
#include "TypeObjectsVis.h"
#include "SettingVV.h"

namespace VisualVariant
{
class cl_Scene:public QObject
{
    Q_OBJECT
public:
    //! конструктор для создания нового объекта из меню
    cl_Scene(FormStatusBar* form,
             TypeObjectsVis* typeObjectsVis_,
             QList<InfoObject* > *info,
             bool circleVariant_=false,
             SettingVV *settingVV=0,
             QWidget *parent=0);

    //! конструктор для создания объекта из файла
    cl_Scene(QDomElement &node,
             FormStatusBar* form,
             TypeObjectsVis* typeObjectsVis_,
             QWidget *parent=0);
    //! конструктор дял клонирования объекта
    cl_Scene(cl_Scene* thisScene,QWidget *parent=0);
    //! клонирование данных, вовращает свою копию
    cl_Scene *clone();
    //! переопределнный класс отображения сцены
    GView  *view;
    //! переопредленный класс сцены
    GScene *scene;
    //! настройки модуля считанные из XML файла
    SettingVV *set;
    //! текущий уровень детализации
    int currentZoom;
    //! первое подключение к слотам
    bool firstConnectingSlots;

    //! текущие координаты
    double curLat;
    double curLon;
    double curLatView;
    double curLonView;

    QPoint mousePos;
    QPointF mousePosScene;

    //! сохранить сцену в XML
    void saveToXML(QDomDocument &domDocument,QDomElement &node);
    void saveToXMLForModel(QDomDocument &domDocument,QDomElement &node);
    //! эта функция альтернативный способ передачи варианта на УЦВС.
    void getRequest(TCommonRequest *request,QString prefix,int num);

    //! клонирование воздуного объекта
    void cloneAirTarget(AirTargetObject *target);

    //! клонирование нашего носителя
    void cloneAircraft(AircraftObject *aircraft_);

    //! клонирование наземного объекта
    void cloneGroundTarget(GroundTargetObject *target);
    void createNewAirTarget(QPointF);
    void createNewGroundTarget(QPointF);
    void createNewAerodrom(QPointF);
    void createNewInfoObject(QPointF);
    void createNewBeaconObject(QPointF);
    void createNewPointRoute(QPointF);//создать новую точку в маршруте
    //! добавить метаданные в
    void addMetaDataToAircraft(QVector<MetaData> list);//в наш самолет
    //! в разработке!!!!!!!
    void addMetaDataToAirTarget(QVector<MetaData> list){Q_UNUSED(list);}//в воздушную цель
    void addMetaDataToGroundTarget(QVector<MetaData> list){Q_UNUSED(list);}//в наземную цель

    //! удалить узлы
    void deleteRoutes();
    //! curLat, curLon относительно их будет увеличение, задается координатами центра экрана
    void setCurrentGeoOnCenter();
    //! задать окно просмотра view
    void setCenterWindowView(double lat,double lon,int zoom);
    //! задать список объектов "метка"
    void setLabelObjects(const QList<LabelObject* > *list)
    {
        labelObjects=list;
    }

    //! задать уровень детализации
    void setZoomLevel(int);

    //! задать положение viewport
    void setViewportCentert();

    //!
    void setSubWindowMDI(QMdiSubWindow *sub){subW=sub;}
    //! прокручивание
    void setScrollViewHand(bool);
    //! обновить список информационных точек
    void refreshInfo();
    //! обновить карту
    void refreshTiles();
    //! показать на карте "Метки"
    void showLabelMap();
    //! удалить метку на карте
    void delLabelMap();
    //! центрирование по самолету
    void centerByAircarft();
    void setActiveRoute(bool value);
    //! задать комментарий
    void setComment(QString value){comment_=value;}
    //! комментарий
    QString comment(){return comment_;}
    //! задать имя варианта
    void setNameVariant(QString name){nameVariant=name;}
    //! задать тип карты
    void setTypeMAP(GeographySysCoord::TypeMAP value){map->setTypeMap(value);}
    //! задать тип слоя
    void setTypeLayer(GeographySysCoord::TypeLayer value){ map->setTypeLayer(value);}
    //! вернуть имя файла
    QString returnNameVariant(){return nameVariant;}
    //! пересчитать координаты объектов при изменении масштаба сцены
    void calcItemPosScene();
    //! отображение состояния
    FormStatusBar *statusBar;
    //! вертикальная шкала
    VerticalScale *vScale;
    //! объект самолета
    AircraftObject *aircraft;
    //! объект который может перемещаться
    AircraftObject *aircraftMove;
    //! система координат
    //SystemCoordObject *sysCoord;
    //! карта с географией земли
    GeographySysCoord *map;
    //! типы объектов
    TypeObjectsVis* typeObjectsVis;
    //! список воздушных целей
    QList<AirTargetObject* >    airTargets;
    QList<AirTargetObject* >    airTargetsMove;
    //! список наземных целей
    QList<GroundTargetObject* > groundTargets;
    QList<GroundTargetObject* > groundTargetsMove;
    //! список аэродромов
    QList<AerodromObject* >     aerodroms;
    //! список информационных точек
    QList<InfoObject* >         *infoObjects;
    //! список маяков
    QList<BeaconObject* >       beaconObjects;
    //! список маршрутов
    QList<RouteObject* >        routeObjects;
    //! список маршрутов
    QList<PPMObject* >          ppmObjects;
    //! список меток(метки одинаковы в независимости от экземпляра сцены)
    const QList<LabelObject* >  *labelObjects;

    QMdiSubWindow *subW;
    //! индекс сцены [1...N](сквозная нумерация вариантов)
    int index;
    //! номер в имени варианта
    int numberNameVariant;
    //! текущий уровень масштаба
    //double valueScale;
    //! признак варианта движения по кругам(false/true)
    bool circleVariant;
    //! признак использовать ли данный вариант при отправке всех вариантов
    bool use;
    //! признак добавления новой метки пользователем
    bool activeAddLabel;
    //! признак добавления маршрута
    bool activeRoute;
    //! отображать всю информацию для объектов
    void setAllInfo(bool value);
    //! отправить сигнал о том, что обновились параметры объекта
    void sendSignalUpdateValueObj();
    //! пересчет текущего положения объектов
    void reCalcObject(double lat,   /*гео. коорд. нашего самолета*/
                      double lon,   /*гео. коорд. нашего самолета*/
                      double aust); /*угол поворота модельной СК*/
signals:
    //! сигнал об изменении увеличения
    void signalChangeZoom(int);
    //! форма для добавления метки
    void activeFormAddLabel(double lat,double lon);
    //! сигнал о том, что параметры объектов изменились
    void signalUpdateValueObj();
public slots:
    //! сбросить состояние
    void slotFlushState();
    //! получение времени
    void slotTime(double);
    //! отображение перемещающихся объектов
    void slotUseMoveObj(bool);
    //! увеличение/уменьшение масштаба
    void slotZoomUp();
    void slotZoomDown();
    //! обновление координат объектов на сцене при смене уровня детализации
    void slotUpdate();
    void slotRightButton();
    void slotClickLeftMouse();
    void slotMove(QPoint);
    //! обновление при изменении уровня детализации
    void refreshZoomLevel();
    //! удалить метку карты из сцены
    void deleteLabelMap(LabelObject *);
    //! реакция на изменение координат и угла курса нашего объекта
    void slotAircraftPsi(double psi);
    void slotAircraftLat(double lat);
    void slotAircraftLon(double lon);
    //! реакция на изменение координат и угла курса воздушных целей
    void slotTargetPsi(int index,double psi);
    void slotTargetLat(int index,double lat);
    void slotTargetLon(int index,double lon);
private:
    //! признак отображения всей информации
    bool allInfoObjects;
    //! признак отображения перемещения объектов
    bool useMoveObj;
    //! имя варианта
    QString  nameVariant;
    QString  comment_;
    QAction* actionAddAirTarget;
    QAction* actionAddGroundTarget;
    QAction* actionAddVorBeam;
    QAction* actionAddRSBNBeam;
    QAction* actionAddPPM;
    QAction* actionAddInfo;
    QAction* actionAddAerodroms;

};
}
#endif /* CL_SCENE_H_ */
