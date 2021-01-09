#pragma once

#include <QSvgRenderer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QDomElement>
#include <QObject>
#include <QMdiSubWindow>
#include <QTimer>

#include "gscene.h"
#include "gview.h"
#include "nodeVisual.h"
#include "formstatusbar.h"
#include "typeObjectsVis.h"
#include "settingVV.h"

#include "nodes/ourObj/ourObj.h"
#include "nodes/airObj/airObj.h"
#include "nodes/groundObj/groundObj.h"
#include "nodes/aerodromObj/aerodObj.h"
#include "nodes/mapObj/mapObj.h"

class cl_Scene:public QObject
{
    Q_OBJECT
public:
    //! конструктор для создания нового объекта из меню
    cl_Scene(FormStatusBar* form,
             TypeObjectsVis* typeObjectsVis_,
             QList<InfoObject* > *info,            
             SettingVV *settingVV   = nullptr,
             QWidget *parent        = nullptr);

    //! конструктор для создания объекта из файла
    cl_Scene(QDomElement &node,
             FormStatusBar* form,
             TypeObjectsVis* typeObjectsVis_,
             QWidget *parent    = nullptr);
    //! конструктор дял клонирования объекта
    cl_Scene(cl_Scene* thisScene,QWidget *parent = nullptr);
    //! клонирование данных, возвращает свою копию
    cl_Scene *clone();
    //! переопределнный класс отображения сцены
    GView  *view;
    //! переопредленный класс сцены
    GScene *scene;
    //!
    ScaleLine* scaleLine;
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

    //! клонирование воздушного объекта
    void cloneAirObj(AirObj *target);

    //! клонирование нашего носителя
    void cloneAircraft(AircraftObject *aircraft_);

    //! клонирование наземного объекта
    void cloneGroundObj(GroundObj *target);
    //! создание объекта
    void createObj(QPointF p, GraphNode::TypeGraphNode type);

    void createNewAirObj(QPointF);
    void createNewGroundObj(QPointF);
    void createNewCloud(QPointF);
    void createNewFog(QPointF);
    void createNewAerodrom(QPointF);
    void createNewInfoObject(QPointF);
    void createNewBeaconObject(QPointF);
    void createNewPointRoute(QPointF);//создать новую точку в маршруте

    //! удалить узлы
    void deleteRoutes();
    //! curLat, curLon относительно их будет увеличение, задается координатами центра экрана
    void setCurrentGeoOnCenter();
    //! задать окно просмотра view
    void setCenterWindowView(double lat,double lon,int zoom);
    //! задать список объектов "метка"
    void setLabelObjects(const QList<LabelObject* > *list)
    {
        labelObjects = list;
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
    //! центрирование по вертолету
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
    //! объект вертолета
    AircraftObject *aircraft;
    //! объект который может перемещаться
    AircraftObject *aircraftMove;
    //! карта с географией земли
    GeographySysCoord *map;
    //! типы объектов
    TypeObjectsVis* typeObjectsVis;
    //! список воздушных целей
    QList<AirObj* >    airObj;
    QList<AirObj* >    airObjMove;
    //! список наземных целей
    QList<GroundObj* >  groundObj;
    QList<GroundObj* >  groundObjMove;
    //! список зон с облачностью
    QList<CloudObject* > cloudObj;
    //! список зон с туманами
    QList<FogObject* > fogObj;
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
    //! список всех объектов
    QList<ObjectGraphNode* > objects;

    QMdiSubWindow *subW;
    //! индекс сцены [1...N](сквозная нумерация вариантов)
    int index;
    //! номер в имени варианта
    int numberNameVariant;
    //! признак использовать ли данный вариант при отправке всех вариантов
    bool use;
    //! признак добавления новой метки пользователем
    bool activeAddLabel;
    //! признак добавления маршрута
    bool activeRoute;
    //! отображать всю информацию для объектов
    void setAllInfo(bool value);
    //! перемещать окно за движущимся объектом
    void setFocusMoveObj(bool value);

    //! отправить сигнал о том, что обновились параметры объекта
    void sendSignalUpdateValueObj();
    //! пересчет текущего положения объектов
    void reCalcObject(double lat,   /*гео. коорд. нашего вертолета*/
                      double lon,   /*гео. коорд. нашего вертолета*/
                      double aust); /*угол поворота модельной СК*/
signals:
    //! сигнал об изменении увеличения
    void signalChangeZoom(int);
    //! форма для добавления метки
    void activeFormAddLabel(double lat,double lon);
    //! сигнал о том, что параметры объектов изменились
    void signalUpdateValueObj();
    void signalRotateVisible(bool);
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
    void slotDoubleClickedMouse();
    void slotClickLeftMouse();
    void slotMove(QPoint);
    //!
    void slotRotateOn();
    void slotRotateOff();
    //! обновление при изменении уровня детализации
    void refreshZoomLevel();
    //! удалить метку карты из сцены
    void deleteLabelMap(LabelObject *);
    //! реакция на изменение координат и угла курса нашего объекта
    void slotAircraftPsi(double psi);
    void slotAircraftLat(double lat);
    void slotAircraftLon(double lon);
    void slotAircraftPos2D(double lon,double lat, double psi);

    //! реакция на изменение координат и угла курса воздушных целей
    void slotTargetPsi(int index,double psi);
    void slotTargetLat(int index,double lat);
    void slotTargetLon(int index,double lon);

    void setInvisibleScaleLine();
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
    QTimer* timer;
};


