#include "cl_scene.h"
#include "../globalFunc/gl_func.h"
#include "factoryObj.h"

#define MAX_ZOOM_LEVEL 18
#define MIN_ZOOM_LEVEL 3


cl_Scene::cl_Scene(FormStatusBar* form,
                   TypeObjectsVis* typeObjectsVis_,
                   QList<InfoObject* > *info,

                   SettingVV *settingVV,
                   QWidget *parent):QObject(parent)
{

    airObj      .clear();
    groundObj   .clear();
    cloudObj    .clear();
    fogObj      .clear();
    aerodroms       .clear();
    beaconObjects   .clear();
    routeObjects    .clear();

    allInfoObjects  =   false;
    labelObjects    =   nullptr;
    infoObjects     =   info;
    statusBar       =   form;
    typeObjectsVis  =   typeObjectsVis_;

    set             =   settingVV;


    // текущее увеличение
    currentZoom         = settingVV->zoom;
    use                 = true;
    index               = 1;
    numberNameVariant   = 1;
    useMoveObj          = set->moveObjects;
    activeAddLabel      = false;
    activeRoute         = false;
    firstConnectingSlots= false;

    map = new GeographySysCoord;
    map->setZValue(0);
    map->setFlag(QGraphicsItem::ItemStacksBehindParent,true);
    map->setTypeMap(set->typeMap);
    map->setTypeLayer(set->typeLayer);


    scene = new GScene;

    connect(scene,SIGNAL(zoomUp()),this,SLOT(slotZoomUp()));
    connect(scene,SIGNAL(zoomDown()),this,SLOT(slotZoomDown()));
    connect(scene,SIGNAL(dragMove()),this,SLOT(slotUpdate()));
//    connect(scene,SIGNAL(sigRightMouse()),this,SLOT(slotRightButton()));
    connect(scene,SIGNAL(signalDoubleClickMouse()),this,SLOT(slotRightButton()));
    connect(scene,SIGNAL(clickLeftMouse()),this,SLOT(slotClickLeftMouse()));
    connect(scene, SIGNAL(signalCtrlPress()), this, SLOT(slotRotateOn()));
    connect(scene, SIGNAL(signalCtrlRelease()), this, SLOT(slotRotateOff()));

    view=new GView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(view,SIGNAL(movePos(QPoint)),this,SLOT(slotMove(QPoint)));
    connect(view,SIGNAL(sigResize()),this,SLOT(refreshZoomLevel()));

    setScrollViewHand(true);

    scaleLine = new ScaleLine(view->geometry());
    scene->addItem(scaleLine);
    timer = new QTimer(this);
    timer->setInterval(2000);
    connect(timer, SIGNAL(timeout()), this, SLOT(setInvisibleScaleLine()));

    aircraft=new AircraftObject(tr("Наш вертолет"),":/res/svg/aircraft",map);
    aircraft->setZoomLevel(currentZoom);
    aircraft->map = map;

    aircraftMove=new AircraftObject(tr("Наш вертолет"),":/res/svg/aircraft_move",map);
    aircraftMove->setZoomLevel(currentZoom);
    aircraftMove->setMovingObject(true);
    aircraftMove->map = map;
    // признак отрисовки траектории
    aircraftMove->trajectory = true;
    // прочитаем координаты центра окна
    curLatView = settingVV->startLat;
    curLonView = settingVV->startLon;
    // текущие координаты
    curLat = curLatView;
    curLon = curLonView;
    QRect rect=view->viewport()->rect();
    // выставляем по координатам в центр окна
    mousePos.setX(rect.width()/2.0);
    mousePos.setY(rect.height()/2.0);

    setCenterWindowView(curLatView,curLonView,currentZoom);

    //double lat,lon;
    //pixelXYToLatLong(aircraft->posC(),currentZoom-1,lat,lon);
    int posX,posY;
    latLongToPixelXY(curLatView,curLonView,currentZoom-1,posX,posY);
    aircraft->setPosC(posX,posY);
    aircraft->setLat(curLatView);
    aircraft->setLon(curLonView);

    aircraftMove->setPosC(posX,posY);
    aircraftMove->setLat(curLatView);
    aircraftMove->setLon(curLonView);
    aircraftMove->setVisible(useMoveObj);

    scene->addItem(map);

    connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
}
cl_Scene::cl_Scene(cl_Scene* thisScene,QWidget *parent):QObject(parent)
{
    allInfoObjects  = false;
    labelObjects    = nullptr;

    airObj      .clear();
    groundObj   .clear();
    cloudObj    .clear();
    fogObj      .clear();
    aerodroms       .clear();
    beaconObjects   .clear();
    routeObjects    .clear();

    infoObjects      = thisScene->infoObjects;
    statusBar        = thisScene->statusBar;
    typeObjectsVis   = thisScene->typeObjectsVis;

    set              = thisScene->set;
    useMoveObj       = thisScene->useMoveObj;
    numberNameVariant= thisScene->numberNameVariant;


    // текущее увеличение
    currentZoom = thisScene->currentZoom;
    use         = thisScene->use;

    index                   = 1;
    activeAddLabel          = thisScene->activeAddLabel;
    activeRoute             = thisScene->activeRoute;
    firstConnectingSlots    = thisScene->firstConnectingSlots;

    map = new GeographySysCoord;
    map->setZValue(0);
    map->setFlag(QGraphicsItem::ItemStacksBehindParent,true);
    map->setTypeMap(thisScene->map->isTypeMap());
    map->setTypeLayer(thisScene->map->isTypeLayer());

    scene = new GScene;

    connect(scene,SIGNAL(zoomUp())          ,this,SLOT(slotZoomUp()));
    connect(scene,SIGNAL(zoomDown())        ,this,SLOT(slotZoomDown()));
    connect(scene,SIGNAL(dragMove())        ,this,SLOT(slotUpdate()));
//    connect(scene,SIGNAL(sigRightMouse())   ,this,SLOT(slotRightButton()));
    connect(scene,SIGNAL(signalDoubleClickMouse()),this,SLOT(slotRightButton()));
    connect(scene,SIGNAL(clickLeftMouse())  ,this,SLOT(slotClickLeftMouse()));

    connect(scene, SIGNAL(signalCtrlPress()), this, SLOT(slotRotateOn()));
    connect(scene, SIGNAL(signalCtrlRelease()), this, SLOT(slotRotateOff()));

    view = new GView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(view,SIGNAL(movePos(QPoint)),this,SLOT(slotMove(QPoint)));
    connect(view,SIGNAL(sigResize()),this,SLOT(refreshZoomLevel()));

    setScrollViewHand(true);

    scaleLine = new ScaleLine(view->geometry());
    scene->addItem(scaleLine);
    timer = new QTimer(this);
    timer->setInterval(2000);
    connect(timer, SIGNAL(timeout()), this, SLOT(setInvisibleScaleLine()));

    aircraft = new AircraftObject(thisScene->aircraft,":/res/svg/aircraft",map);

    aircraft->setZoomLevel(currentZoom);
    aircraft->map = map;

    aircraftMove = new AircraftObject(tr("Наш вертолет"),":/res/svg/aircraft_move",map);
    aircraftMove->setZoomLevel(currentZoom);
    aircraftMove->setMovingObject(true);
    aircraftMove->map           = map;
    aircraftMove->trajectory    = true;

    // прочитаем координаты центра окна
    curLatView = thisScene->curLatView;
    curLonView = thisScene->curLonView;

    curLat = curLatView;
    curLon = curLonView;
    QRect rect = view->viewport()->rect();
    // выставляем по координатам в центр окна
    mousePos.setX(rect.width()/2.0);
    mousePos.setY(rect.height()/2.0);

    setCenterWindowView(curLatView,curLonView,currentZoom);

    //double lat,lon;
    //pixelXYToLatLong(aircraft->posC(),currentZoom-1,lat,lon);
    int posX,posY;
    latLongToPixelXY(curLatView,curLonView,currentZoom-1,posX,posY);
    aircraft->setPosC(posX,posY);
    aircraft->setLat(curLatView);
    aircraft->setLon(curLonView);

    aircraftMove->setPosC(posX,posY);
    aircraftMove->setLat(curLatView);
    aircraftMove->setLon(curLonView);
    aircraftMove->setVisible(useMoveObj);

    connect(aircraft,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
    for(auto i:thisScene->airObj)
    {
        cloneAirObj(i);
    }
    for(auto i:thisScene->groundObj)
    {
        cloneGroundObj(i);
    }
    scene->addItem(map);
}

void cl_Scene::setScrollViewHand(bool flag)
{
    if(flag == true)
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    else
        view->setDragMode(QGraphicsView::RubberBandDrag);
}
cl_Scene::cl_Scene(QDomElement &node,
                   FormStatusBar* form,
                   TypeObjectsVis* typeObjectsVis_,
                   QWidget *parent):QObject(parent)
{

    // обнуление спиcков
    airObj      .clear();
    groundObj   .clear();
    aerodroms   .clear();
    cloudObj    .clear();
    fogObj      .clear();
    routeObjects.clear();

    // значения по умолчанию
    allInfoObjects      = false;
    labelObjects        = nullptr;
    numberNameVariant   = 1;
    statusBar           = form;
    typeObjectsVis      = typeObjectsVis_;
    infoObjects         = nullptr;
    index               = 1;
    useMoveObj          = false;
    activeAddLabel      = false;
    activeRoute         = false;
    firstConnectingSlots= false;


    // текущий уровень детализации
    currentZoom = node.attribute("scale",QString::number(MIN_ZOOM_LEVEL)).toInt();
    // тип карты и слоя
    QString tempTypeMap     = node.attribute("typeMaps","");
    QString tempTypeLayer   = node.attribute("layerMaps","");

    map = new GeographySysCoord;
    map->setZValue(0);
    map->setFlag(QGraphicsItem::ItemStacksBehindParent,true);
    map->setTypeMap(tempTypeMap);
    map->setTypeLayer(tempTypeLayer);

    // сцена
    scene = new GScene;
    connect(scene,SIGNAL(zoomUp()),         this,SLOT(slotZoomUp()));
    connect(scene,SIGNAL(zoomDown()),       this,SLOT(slotZoomDown()));
    connect(scene,SIGNAL(dragMove()),       this,SLOT(slotUpdate()));
//    connect(scene,SIGNAL(sigRightMouse()),  this,SLOT(slotRightButton()));
    connect(scene,SIGNAL(signalDoubleClickMouse()),this,SLOT(slotRightButton()));
    connect(scene, SIGNAL(signalCtrlPress()), this, SLOT(slotRotateOn()));
    connect(scene, SIGNAL(signalCtrlRelease()), this, SLOT(slotRotateOff()));
    connect(scene,SIGNAL(clickLeftMouse())  ,this,SLOT(slotClickLeftMouse()));

    view = new GView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(view,SIGNAL(movePos(QPoint)),this,SLOT(slotMove(QPoint)));
    connect(view,SIGNAL(sigResize()),this,SLOT(refreshZoomLevel()));
    setScrollViewHand(true);

    scaleLine = new ScaleLine(view->geometry());
    scene->addItem(scaleLine);
    timer = new QTimer(this);
    timer->setInterval(2000);
    connect(timer, SIGNAL(timeout()), this, SLOT(setInvisibleScaleLine()));

//    vScale=new VerticalScale(view->geometry());
//    scene->addItem(vScale);
    aircraft = new AircraftObject(tr("Наш вертолет"),":/res/svg/aircraft",map);
    aircraft->setZoomLevel(currentZoom);
    aircraft->map = map;

    //создаем двигающийся объект
    aircraftMove = new AircraftObject(tr("Наш вертолет"),":/res/svg/aircraft_move",map);
    aircraftMove->setZoomLevel(currentZoom);
    aircraftMove->setMovingObject(true);
    aircraftMove->setZValue(2);
    aircraftMove->map        = map;
    aircraftMove->trajectory = true;
    aircraftMove->setVisible(useMoveObj);

    connect(aircraft,
            SIGNAL(isModifyPosition(QPointF,TGeoPoint)),
            this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));

    //прочитаем координаты центра окна
    curLatView = (node.attribute("viewport_center_lat","0")).toDouble();
    curLonView = (node.attribute("viewport_center_lon","0")).toDouble();

    curLat = curLatView;
    curLon = curLonView;

    QPolygonF polygon = view->mapToScene(view->viewport()->rect());
    QRectF rect = polygon.boundingRect();

    //выставляем по координатам в центр окна
    mousePos.setX(rect.width()/2.0);
    mousePos.setY(rect.height()/2.0);


    comment_ = node.attribute("comment","");

    //заполняем данные по варианту
    nameVariant = node.attribute("name","");
    use = node.attribute("use","1").toInt();

    //заполняем данные по вертолету
    QDomElement tempNode = node.firstChildElement("Aircraft");
    aircraft->loadXML(tempNode);
    aircraftMove->loadXML(tempNode);

    //номер имени варианта
    numberNameVariant = (node.attribute("numberNameVariant","1")).toInt();
    //заполняем данные по воздушным целям
    tempNode=node.firstChildElement("AirTarget");
    airObj.clear();

    while(!tempNode.isNull())
    {
        //оьъект цели
        AirObj *target = new AirObj("",":/res/svg/target",map);
        target->map = map;
        target->connectToObj(aircraft);
        target->setZoomLevel(currentZoom);

        //создаем двигающийся объект
        AirObj *target_move = new AirObj("",":/res/svg/target_move",map);
        target_move->map = map;
        target_move->connectToObj(aircraftMove);
        target_move->setZoomLevel(currentZoom);
        target_move->setMovingObject(true);
        target_move->loadXML(tempNode);
        target_move->setZValue(1);
        target_move->trajectory = true;
        target_move->setVisible(useMoveObj);
        target->formSetting->setListObjectVis(typeObjectsVis->listAirObjects());
        target->loadXML(tempNode);


        int tempCode = tempNode.attribute("codeObjectVis","100").toInt();
        tempCode = qMax(tempCode,100);
//        if(tempCode<100)
//            tempCode=100;
        target->setCode(tempCode,typeObjectsVis->codeAir(tempCode));

        connect(target,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));

        airObj.push_back(target);
        target->setIndex(airObj.size());
        target_move->setIndex(airObj.size());

        airObjMove.push_back(target_move);

        tempNode = tempNode.nextSiblingElement("AirTarget");
    }
    //заполняем данные по наземным целям
    tempNode = node.firstChildElement("GroundTarget");
    groundObj.clear();
    while(!tempNode.isNull())
    {
        GroundObj *target = new GroundObj("",":/res/svg/gtarget",map);
        target->map = map;
        target->connectToObj(aircraft);
        target->setZoomLevel(currentZoom);
        target->formSetting->setListObjectVis(typeObjectsVis->listGroundObjects());
        ///target->hide();
        target->loadXML(tempNode);

        int tempCode = tempNode.attribute("codeObjectVis","0").toInt();
        target->setCode(tempCode,typeObjectsVis->codeGround(tempCode));

        connect(target,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
        groundObj.push_back(target);
        target->setIndex(groundObj.size());

        tempNode=tempNode.nextSiblingElement("GroundTarget");
    }
    scene->addItem(map);
    setTypeMAP(GeographySysCoord::GOOGLE_SAT_EARTH);
    setTypeLayer(GeographySysCoord::GOOGLE_HYB);
    setZoomLevel(currentZoom);
}
void cl_Scene::saveToXMLForModel(QDomDocument &domDocument,QDomElement &node)
{
   QDomElement tempNode = domDocument.createElement("Variation");

   tempNode.setAttribute("comment",comment_);
   QDomText domTextVar = domDocument.createTextNode(QString::number(index));
   tempNode.appendChild(domTextVar);
   node.appendChild(tempNode);

   aircraft->saveXMLForModel(domDocument,tempNode);
   QDomElement eleNode=domDocument.createElement("NumberOfAirTarget");
   domTextVar = domDocument.createTextNode(QString::number(airObj.size()));
   eleNode.appendChild(domTextVar);
   tempNode.appendChild(eleNode);
   //сохраним параметры воздушный целей
   for(auto i:airObj)
   {
       if(i->isEnable() == true)
           i->saveXMLForModel(domDocument,tempNode);
   }
   eleNode      = domDocument.createElement("NumberOfGroundTarget");
   domTextVar   = domDocument.createTextNode(QString::number(groundObj.size()));
   eleNode.appendChild(domTextVar);
   tempNode.appendChild(eleNode);

   //сохраним параметры наземных объектов
   for(auto i:groundObj)
   {
       if(i->isEnable() == true)
           i->saveXMLForModel(domDocument,tempNode);
   }
}
void cl_Scene::saveToXML(QDomDocument &domDocument,QDomElement &node)
{
    QDomElement tempNode=domDocument.createElement("variant");
    node.appendChild(tempNode);
    tempNode.setAttribute("name",nameVariant);
    tempNode.setAttribute("comment",comment());
    tempNode.setAttribute("scale",QString::number(currentZoom));

    //вариант используется или нет
    tempNode.setAttribute("use",use);

    tempNode.setAttribute("viewport_center_lat",curLatView);
    tempNode.setAttribute("viewport_center_lon",curLonView);
    tempNode.setAttribute("typeMaps",map->strTypeMap());
    tempNode.setAttribute("layerMaps",map->strTypeLayer());
    tempNode.setAttribute("numberNameVariant",numberNameVariant);
    ////////////////////////////////////////////////////////////////

    //сохранить XML файл
    aircraft->saveXML(domDocument,tempNode);
    //сохраним параметры воздушный целей
    for(auto i:airObj)
    {
        i->saveXML(domDocument,tempNode);
    }
    //сохраним параметры наземной цели
    for(auto i:groundObj)
    {
       i->saveXML(domDocument,tempNode);
    }
}
void cl_Scene::getRequest(TCommonRequest *request,QString prefix,int num)
{
    aircraft->getRequest(prefix,request);
}

void cl_Scene::slotUpdate()
{
    QPolygonF polygon = view->mapToScene(view->viewport()->rect());
    QRectF rect = polygon.boundingRect();
    map->setZoomLevel(currentZoom,rect);

    if(activeAddLabel == true)
    {
        activeAddLabel = false;
        view->setCursor(Qt::ArrowCursor);
        emit activeFormAddLabel(curLat,curLon);
    }
}
void cl_Scene::slotRightButton()
{

}

void cl_Scene::slotDoubleClickedMouse()
{
    if(activeRoute == true)
    {
        createNewPointRoute(mousePosScene);
    }
//    if(activeAddLabel==true)
//    {
//        activeAddLabel=false;
//        view->setCursor(Qt::ArrowCursor);
//    }
//    setActiveRoute(false);
}

void cl_Scene::slotClickLeftMouse()
{
    setInvisibleScaleLine();
}
void cl_Scene::slotZoomUp()
{
    currentZoom++;
    setZoomLevel(currentZoom);
    emit signalChangeZoom(currentZoom);
}
void cl_Scene::slotZoomDown()
{
    currentZoom--;
    setZoomLevel(currentZoom);
    emit signalChangeZoom(currentZoom);
}
void cl_Scene::slotMove(QPoint tpos)
{
    mousePos    = tpos;
    QPointF pos = view->mapToScene(tpos);
    mousePosScene=pos;
    pixelXYToLatLong(pos.x(),pos.y(),currentZoom-1,curLat,curLon);
    TGeoPoint geo(curLat,curLon);
    statusBar->setPos(pos,geo);
}

void cl_Scene::slotRotateOn()
{
    emit signalRotateVisible(true);
    aircraft->setVisibleRotateRect(true);
   
    for(auto i: airObj){
        i->setVisibleRotateRect(true);
    }
    for(auto i: groundObj){
        i->setVisibleRotateRect(true);
    }
}

void cl_Scene::slotRotateOff()
{
    emit signalRotateVisible(false);
    aircraft->setVisibleRotateRect(false);
   
    for(auto i: airObj){
        i->setVisibleRotateRect(false);
    }
    for(auto i: groundObj){
        i->setVisibleRotateRect(false);
    }
}

void cl_Scene::refreshZoomLevel()
{
    setZoomLevel(currentZoom);
}
void cl_Scene::centerByAircarft()
{
    setCenterWindowView(aircraft->lat,aircraft->lon,currentZoom);
}
void cl_Scene::setCurrentGeoOnCenter()
{
    QRectF rect=view->viewport()->rect();

    mousePos.setX(rect.width()/2);
    mousePos.setY(rect.height()/2);

    QPointF point=view->mapToScene(mousePos);
    pixelXYToLatLong(point.x(),point.y(),currentZoom-1,curLat,curLon);

}
void cl_Scene::setCenterWindowView(double lat,double lon, int zoom)
{
    zoom = qMin(zoom, MAX_ZOOM_LEVEL);
    currentZoom = zoom;
    statusBar->setZoom(zoom);
    int wh = 2<<(zoom-1);

    scene->setSceneRect(0.0,0.0,wh*256,wh*256);
    int x;int y;
    latLongToPixelXY(lat,lon,zoom-1,x,y);
    view->centerOn(x,y);
    refreshTiles();
}

void cl_Scene::setZoomLevel(int z)
{
    int x,y;
    z = qBound(MIN_ZOOM_LEVEL,z, MAX_ZOOM_LEVEL);
    currentZoom = z;
    statusBar->setZoom(currentZoom);
    int wh = 2<<(z-1);
    scene->setSceneRect(0.0,0.0,wh*256,wh*256);
    latLongToPixelXY(curLat,curLon,currentZoom-1,x,y);
    view->centerOn(x,y);

    QPointF mousePosScene = view->mapToScene(mousePos);
    view->centerOn(2*x-mousePosScene.x(),2*y-mousePosScene.y());
    refreshTiles();
}
void cl_Scene::refreshTiles()
{
    QPolygonF polygon_temp = view->mapToScene(view->viewport()->rect());
    QRectF rect_temp = polygon_temp.boundingRect();
    //загрузка и отображение тайлов
    map->setZoomLevel(currentZoom,rect_temp);
    //пересчет всех объектов на карте
    calcItemPosScene();

    //пересчет шкалы масштаба дальности
    timer->stop();
    scaleLine->updateScaleLine(rect_temp, currentZoom);
    timer->start();
}
void cl_Scene::setInvisibleScaleLine()
{
    scaleLine->setVisibleScaleLine(false);
    timer->stop();
}

void cl_Scene::refreshInfo()
{

}
void cl_Scene::calcItemPosScene()
{
    int curX = 0;
    int curY = 0;
    // уровень детализации для нашего вертолета
    aircraft->setZoomLevel(currentZoom);
    // пересчет географических координат в прямоугольные
    latLongToPixelXY(aircraft->lat,aircraft->lon,currentZoom-1,curX,curY);
    // координаты носителя
    aircraft->setPosC(curX,curY);
    // уровень детализации для фантомного нашего вертолета
    aircraftMove->setZoomLevel(currentZoom);
    // пересчет географических координат в прямоугольные
    latLongToPixelXY(aircraftMove->lat,aircraftMove->lon,currentZoom-1,curX,curY);
    // пересчитать всю траекторию с учетом новым уровнем детализации
    aircraftMove->refreshTrajectory(currentZoom-1);
    // текущие координаты
    aircraftMove->setPosC(curX,curY);
    // воздушные объекты
    for(auto i:airObj)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom - 1,curX,curY);
        i->setPosC(curX,curY);
    }
    // двигующиеся воздушные объекты
    for(auto i:airObjMove)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom-1,curX,curY);
        i->refreshTrajectory(currentZoom-1);
        i->setPosC(curX,curY);
    }
    // наземные объекты
    for(auto i:groundObj)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom - 1,curX,curY);
        i->setPosC(curX,curY);
    }
    // пересчет облачности
    for(auto i:cloudObj)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom - 1,curX,curY);
        i->setPosC(curX,curY);
    }
    // пересчет тумана
    for(auto i:fogObj)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom - 1,curX,curY);
        i->setPosC(curX,curY);
    }
    // объекты метки
    if(labelObjects!=nullptr)
    {
        for(auto i:*labelObjects)
        {
            i->setZoomLevel(currentZoom);
            latLongToPixelXY(i->lat,i->lon,currentZoom-1,curX,curY);
            i->setPosC(curX,curY);
        }
    }
    // объекты маршрута
    for(auto i:routeObjects)
    {
        i->setZoomLevel(currentZoom);
        latLongToPixelXY(i->lat,i->lon,currentZoom-1,curX,curY);
        i->setPosC(curX,curY);
    }
}
void cl_Scene::deleteRoutes()
{
    for(auto i:routeObjects)
    {
        scene->removeItem(i);
    }
    routeObjects.clear();

    setActiveRoute(false);
}
void cl_Scene::cloneAircraft(AircraftObject *aircraft_)
{
    Q_UNUSED(aircraft_);
//    aircraft->setZoomLevel(currentZoom);
//    aircraft->setPosC(posX,posY);

//    aircraft->setLat(aircraft_->ccurLatView);
//    aircraft->setLon(curLonView);

}
void cl_Scene::cloneAirObj(AirObj *target_)
{
    AirObj *target = new AirObj(target_,aircraft,map);
    target->setName(tr("Объект №")+QString::number(airObj.size()+1));
    target->setZoomLevel(currentZoom);
    target->setAllInfo(allInfoObjects);
    target->formSetting->setListObjectVis(typeObjectsVis->listAirObjects());
    airObj.push_back(target);

    QPointF pos=map->mapFromScene(target_->posC());
    target->map=map;

    target->setIndex(airObj.size());
    double lat,lon;
    pixelXYToLatLong(target_->posC(),currentZoom-1,lat,lon);
    target->setLat(lat);
    target->setLon(lon);
    target->slotIsModifyPosition();

    target->setPosC(pos.x(),pos.y());
    target->refresh();

//    connect(target,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
}

void cl_Scene::cloneGroundObj(GroundObj *target_)
{
    GroundObj *target=new GroundObj(target_,aircraft,map);
    target->setName(tr("Объект №")+QString::number(groundObj.size()+1));
    target->setZoomLevel(currentZoom);
    target->setAllInfo(allInfoObjects);
    target->formSetting->setListObjectVis(typeObjectsVis->listGroundObjects());
    groundObj.push_back(target);
}
void cl_Scene::createObj(QPointF p, GraphNode::TypeGraphNode type)
{
    FactoryObj::obj()->createObj(type,map);
}
void cl_Scene::createNewAirObj(QPointF p)
{
    AirObj *target=new AirObj(tr("Объект №")+QString::number(airObj.size()+1),":/res/svg/target",map);
    target->setCode(100,typeObjectsVis->codeAir(100));

    target->connectToObj(aircraft);
    target->setZoomLevel(currentZoom);
    target->setAllInfo(allInfoObjects);
    target->formSetting->setListObjectVis(typeObjectsVis->listAirObjects());
    airObj.push_back(target);

    // создаем новую перемещащуюся цель
    AirObj *target_move = new AirObj(tr("Объект №") + QString::number(airObjMove.size()+1),":/res/svg/target_move",map);

    target_move->connectToObj(aircraftMove);
    target_move->setZoomLevel(currentZoom);
    target_move->formSetting->setListObjectVis(typeObjectsVis->listAirObjects());
    target_move->setMovingObject(true);
    target_move->trajectory=true;
    target_move->setVisible(useMoveObj);
    target_move->setZValue(1);


    airObjMove.push_back(target_move);

    QPointF pos=map->mapFromScene(p);
    target->map=map;

    target->setIndex(airObj.size());
    double lat,lon;
    pixelXYToLatLong(p,currentZoom-1,lat,lon);
    target->setLat(lat);
    target->setLon(lon);
    target->slotIsModifyPosition();

    target->setPosC(pos.x(),pos.y());
    target->refresh();

    target_move->setLat(lat);
    target_move->setLon(lon);


    target_move->setPosC(pos.x(),pos.y());
    target_move->refresh();
    target_move->setIndex(airObj.size());

    connect(target,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),
            this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
    sendSignalUpdateValueObj();
}
//!
void cl_Scene::sendSignalUpdateValueObj()
{
    emit signalUpdateValueObj();
}
void cl_Scene::createNewGroundObj(QPointF p)
{
    GroundObj *target=new GroundObj(tr("Назем. Объект №")+QString::number(groundObj.size()+1),":/res/svg/gtarget",map);
    target->setCode(311,typeObjectsVis->codeGround(311));
    target->connectToObj(aircraft);
    target->setZoomLevel(currentZoom);
    target->setAllInfo(allInfoObjects);
    target->formSetting->setListObjectVis(typeObjectsVis->listGroundObjects());
    groundObj.push_back(target);

    QPointF pos=map->mapFromScene(p);
    target->map=map;
    target->setIndex(groundObj.size());
    double lat,lon;
    pixelXYToLatLong(p,currentZoom-1,lat,lon);
    target->setLat(lat);
    target->setLon(lon);


    //target->setScale(1/map->scale());
    target->slotIsModifyPosition();
    //target->setXZMap(pos.x(),pos.y());
    target->refresh();
    scene->addItem(target);
    connect(target,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));

    sendSignalUpdateValueObj();
}
void cl_Scene::showLabelMap()
{
    if(labelObjects!=nullptr)
    {
        for(auto i :*labelObjects)
        {
            scene->addItem(i);
        }
    }
}
void cl_Scene::deleteLabelMap(LabelObject *label)
{
    scene->removeItem(label);
}

void cl_Scene::delLabelMap()
{
    if(labelObjects!=nullptr)
    {
         for(auto i :*labelObjects)
        {
            scene->removeItem(i);
        }
    }
}
void cl_Scene::createNewCloud(QPointF p)
{
    CloudObject *cloud = new CloudObject(":/res/svg/cloud",map);

    cloud->map = map;
    QPointF pos = map->mapFromScene(p);
    cloud->setPos(pos.x(),pos.y());
    cloud->setZoomLevel(currentZoom);
    pixelXYToLatLong(p,currentZoom - 1,cloud->lat,cloud->lon);
    cloudObj.push_back(cloud);
    scene->addItem(cloud);

    connect(cloud,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
    // приводит к обновлению сцены
    setZoomLevel(currentZoom);
}
void cl_Scene::createNewFog(QPointF p)
{
    FogObject *fog = new FogObject(":/res/svg/fog",map);

    fog->map = map;
    QPointF pos = map->mapFromScene(p);
    fog->setPos(pos.x(),pos.y());
    fog->setZoomLevel(currentZoom);
    pixelXYToLatLong(p,currentZoom - 1,fog->lat,fog->lon);
    fogObj.push_back(fog);
    scene->addItem(fog);

    connect(fog,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
    // приводит к обновлению сцены
    setZoomLevel(currentZoom);
}
void cl_Scene::createNewAerodrom(QPointF p)
{
    AerodromObject *aero = new AerodromObject("./res/aerodrom.svg",aerodroms.size(),map);
    aero->setAircraft(aircraft);
    aerodroms.push_back(aero);
    aero->map = map;
    QPointF pos = map->mapFromScene(p);
    aero->setPos(pos.x(),pos.y());

    scene->addItem(aero);
    connect(aero,SIGNAL(isModifyPosition(QPointF,TGeoPoint)),this->statusBar,SLOT(setPos(QPointF,TGeoPoint)));
}
void cl_Scene::createNewInfoObject(QPointF p)
{
    InfoObject *info = new InfoObject("./res/info.svg",map);
    infoObjects->push_back(info);
    info->map = map;
    QPointF pos = map->mapFromScene(p);
    info->setPos(pos.x(),pos.y());
    scene->addItem(info);
}
void cl_Scene::createNewBeaconObject(QPointF p)
{
    BeaconObject *beacon = new BeaconObject(":/res/svg/vor",map);

    beacon->map = map;
    QPointF pos = map->mapFromScene(p);
    beacon->setPos(pos.x(),pos.y());
    beacon->setZoomLevel(currentZoom);
    pixelXYToLatLong(p,currentZoom - 1,beacon->lat,beacon->lon);
    beaconObjects.push_back(beacon);
    scene->addItem(beacon);
    setZoomLevel(currentZoom);
}
void cl_Scene::setActiveRoute(bool value)
{
    activeRoute = value;
    if(value == true)
    {
        view->setCursor(Qt::CrossCursor);
    }else
    {
        view->setCursor(Qt::ArrowCursor);
    }
}
void cl_Scene::createNewPointRoute(QPointF p)
{
    RouteObject *route = new RouteObject("./res/route.svg",map);
    routeObjects.push_back(route);

    route->setZoomLevel(currentZoom);
    route->map = map;
    QPointF pos = map->mapFromScene(p);
    pixelXYToLatLong(p,currentZoom-1,route->lat,route->lon);
    route->setPosC(pos.x(),pos.y());

    if(routeObjects.size()>1)
    {
        int i = routeObjects.size()-1;
        routeObjects[i-1]->setRightRoute(route);
        routeObjects[i]->setLeftRoute(routeObjects[i-1]);
    }
}
void cl_Scene::slotAircraftPos2D(double lon,double lat, double psi)
{
     aircraftMove->setPsi(psi);
     aircraftMove->slotLonLatToXZ(lat,lon);
}
void cl_Scene::slotAircraftPsi(double psi)
{
    aircraftMove->setPsi(psi);
}
void cl_Scene::slotAircraftLat(double lat)
{
    //aircraftMove->setLat(lat);
    aircraftMove->slotLatToZ(lat);
}
void cl_Scene::slotAircraftLon(double lon)
{
    aircraftMove->slotLonToX(lon);
    //aircraftMove->setLon(lon);
}
void cl_Scene::slotTargetPsi(int index,double psi)
{
    if(airObjMove.isEmpty() == false)
    {
        int i = clip(index,0,airObjMove.size()-1);
        airObjMove[i]->setPsi(psi);
    }
}
void cl_Scene::slotTargetLat(int index,double lat)
{
    if(airObjMove.isEmpty() == false)
    {
        int i = clip(index,0,airObjMove.size()-1);
        airObjMove[i]->slotLatToZ(lat);
    }
}
void cl_Scene::slotTargetLon(int index,double lon)
{
    if(airObjMove.isEmpty() == false)
    {
        int i = clip(index,0,airObjMove.size()-1);
        airObjMove[i]->slotLonToX(lon);
    }
}
void cl_Scene::setAllInfo(bool value)
{
    allInfoObjects = value;

    aircraft->setAllInfo(allInfoObjects);

    for(auto i:airObj)
        i->setAllInfo(allInfoObjects);

    for(auto i:groundObj)
        i->setAllInfo(allInfoObjects);

}
void cl_Scene::setFocusMoveObj(bool value)
{

}

void cl_Scene::slotUseMoveObj(bool value)
{
    useMoveObj = value;
    if(aircraftMove!=nullptr)
    {
        aircraftMove->setVisible(useMoveObj);
        aircraftMove->setVisibleTraj(useMoveObj);
        //обнуление траектории, если отключили отображение  траектории
        if(value == false)
            aircraftMove->clearTraj();
    }
    for(auto i:airObjMove)
    {
        i->setVisible(useMoveObj);
        i->setVisibleTraj(useMoveObj);
        if(value == false)
            i->clearTraj();
    }

}
void cl_Scene::slotTime(double value)
{
    if(aircraftMove != nullptr)
    {
        aircraftMove->setTime(value);
    }
    for(auto i:airObjMove)
    {
        i->setTime(value);
    }
}
//пересчет текущего положения графических объектов
void cl_Scene::reCalcObject(double lat,   /*гео. коорд. нашего вертолета (град)*/
                            double lon,   /*гео. коорд. нашего вертолета (град)*/
                            double aust)  /*угол поворота модельной СК (град) */
{
    aircraft->slotLatToZ(lat);
    aircraft->slotLonToX(lon);
    aircraft->setPsi(aust+aircraft->psi);

    for(auto i:airObj)
    {
        i->setPsi(aust+i->psi);
    }
    for(auto i:groundObj)
    {
        //groundObj[i]->setPsi(aust+airObj[i]->psi);
    }
}
void cl_Scene::slotFlushState()
{
    if(aircraftMove != nullptr)
    {
        aircraftMove->clearTraj();
    }
    for(auto i:airObjMove)
    {
        i->clearTraj();
    }
}


