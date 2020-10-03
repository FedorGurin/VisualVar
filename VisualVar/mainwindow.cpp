#include <QMdiSubWindow>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cl_Scene.h"
#include "./mppm/CommonEngineData.h"
#include "./mppm/libmppm.h"
#include <QLibrary>

//#define OLD_STEND
typedef IEngineData* (*CreateEngine)();

namespace VisualVariant
{

GeographicLib::Geocentric* earth = nullptr;

cl_MouseFilterVariant::cl_MouseFilterVariant(QObject* pobj,QWidget *slotForm_):QObject(pobj)
{
    slotForm = slotForm_;
}
bool cl_MouseFilterVariant::eventFilter(QObject* pobj,QEvent *event)
{
    if(event->type() == QEvent::ContextMenu)
    {
        MainWindowVisVar *ptr = qobject_cast<MainWindowVisVar* >(slotForm);
        if(ptr != nullptr)
        {
            ptr->rightButtonMouseClicked();
            return true;
        }else
            return false;
    }else if(event->type() == QEvent::Close)
    {
        MainWindowVisVar *ptr = qobject_cast<MainWindowVisVar* >(slotForm);
        if(ptr != nullptr)
        {
            for(auto i:ptr->scenes)
            {
                QObject* pObject = i->view;
                if(pObject == pobj)
                {
                    ptr->scenes.removeAll(i);
                    ptr->formManualModify->deleteScene(0);
                    ptr->formManualModify->resetModelData();
                }
            }
            return true;
        }else
            return false;
    }
    return false;
}

MainWindowVisVar::MainWindowVisVar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowVariant)
{
    statusBar = nullptr;
    ui->setupUi(this);
    ObjectGraphNode::initObjectsProperty();
    settingVV = new SettingVV(this);
    formProjH = new FormProjectH;
    //ui->toolBarH->addWidget(formProjH);

#ifdef QT_DEBUG
    QLibrary libMPPM("libmppmd");
#else
    QLibrary libMPPM("libmppm");
#endif

    CreateEngine func = reinterpret_cast<CreateEngine > (libMPPM.resolve("createEngine"));
    if(func == nullptr)
    {
        QMessageBox::warning(this, tr("Внимание!"),
                             tr("libMPPM: (CreateEngine)libMPPM.resolve(\"createEngine\") = 0. \n"
                                "Библиотека libMPPM не подключена/не загружена"),
                             QMessageBox::Ok);

    }
    engine = func();

  //! получение обратного сигнала
    connect(engine, SIGNAL(reciveEventsRequest(TRequestEvent)), this, SLOT(slotIds(TRequestEvent)));
    connect(engine, SIGNAL(reciveEventsRequest(TRequestEvent)), this, SLOT(slotEventsRequest(TRequestEvent)));

  //создание QActions
    createActions();
    //создание главного меню
    createMainMenuBar();

   //! Система координат для пересчета положения целей (применяется для сцены при unitedStands==true)
    earthWGS = new GeographicLib::Geocentric(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
    earthPZ90 = new GeographicLib::Geocentric(6378136., 1/298.25784);
    earth = earthPZ90;//по умолчанию - РЗ-90

    menu            = new QMenu;
    statusBar       = new FormStatusBar;
    typeObjectsVis  = new TypeObjectsVis;
    dialogSave      = new DialogSaveCurVar;
    formManualModify= new FormManualModify(&scenes);
    //! форма добавления новой метки
    formAddLabel    = new FormAddLabelMap;

    //! окно со списком вариантов
    dockWidg = new QDockWidget(this);
    dockWidg->setWindowTitle("Список вариантов");
    dockWidg->setAllowedAreas(Qt::BottomDockWidgetArea);
    dockWidg->setWidget(formManualModify);
    dockWidg->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockWidg->setTitleBarWidget(new QWidget());
    dockWidg->titleBarWidget()->setVisible(false);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidg);


    //! окно со списком вариантов
    dockWidgH = new QDockWidget(this);
    dockWidgH->setWindowTitle("Высоты");
    dockWidgH->setAllowedAreas(Qt::RightDockWidgetArea);
    dockWidgH->setWidget(formProjH);
//    dockWidgH->setWidget(stackedWidgProjH);
    dockWidgH->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockWidgH->setTitleBarWidget(new QWidget());
    dockWidgH->titleBarWidget()->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, dockWidgH);

    //! текущая сцена не определена
    currentScenes   = nullptr;
    //! по умолчанию используем цифровую карту
    useMap          = true;
    toFlushButton   = false;

    checkBoxGeography=new QCheckBox(tr("Привязка к карте"));

    pathFileToSend=new QLineEdit(tr("По умолчанию"));
    //ui->toolBarSendFile->addWidget(formManualModify);
    ui->toolBarSendFile->setVisible(false);

    parser=new Parser(statusBar,&scenes,typeObjectsVis);

    formZoomPanel = new FormZoomPanel;
    ui->statusBar->addWidget(statusBar);

    ui->toolBarOperation->addWidget(formZoomPanel);
    ui->toolBarOperation->addSeparator();
    ui->toolBarOperation->addAction(actBtnHandMoveMap);
    ui->toolBarOperation->addAction(actBtnCursor);
    ui->toolBarOperation->addAction(actBtnRuler);
    ui->toolBarOperation->addSeparator();
    ui->toolBarOperation->addAction(actBtnCentering);
    ui->toolBarOperation->addAction(actBtnRouteDynObject);
    ui->toolBarOperation->addAction(actBtnAllInfoObject);
    ui->toolBarOperation->addAction(actBtnFocusMoveObj);
    ui->toolBarOperation->addSeparator();
    ui->toolBarOperation->addAction(actProjH);
    ui->toolBarOperation->addAction(actRotateVisible);

    ui->toolBarCreate->addAction(ui->actionSaveAll);
    ui->toolBarCreate->addSeparator();
    ui->toolBarCreate->addAction(ui->actionCreateVar);
    //ui->toolBarCreate->addAction(ui->actionCreateVarCircle);

    ui->toolBarCreate->addSeparator();

   // ui->toolBarCreate->addSeparator();

    ui->toolBarMain->addAction(actBtnSend);
    //ui->toolBarMain->addSeparator();
   // ui->toolBarMain->addAction(ui->actMppmOpen);
//#ifdef OLD_STAND
//    ui->toolBarMain->addAction(actBtnSendToOtherStand);
//#endif

    fileName.clear();

    dialog = new QFileDialog(this);
    dialog->setNameFilter(tr("XML-variants(*.xml)"));
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setDirectory(settingVV->dirSaveVar);
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mdiArea->setTabsClosable(false);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    this->setCentralWidget(mdiArea);

    formManualModify->setLastNameFile(parser->readLastNameFile("./xml/LastSend.xml"));
    bool isOpen=openFile(parser->readLastNameFile("./xml/LastSend.xml"));
    if(isOpen == false)
    {
        openFile("./xml/LastSend.xml");
    }
    slotButtonHandMoveMap(actBtnHandMoveMap->isChecked());
    slotButtonCentering();

    //!
    createConnections();

#ifndef OLD_STAND
    //! запрос на чтение текущих параметров
    TCommonRequest listReq;
    listReq.setReciver("MPPM");
    listReq.setSender("VAR");
    QString prefixPath      = settingVV->pathToObj + "Общие параметры.Kin.Work.";
    listReq.append(prefixPath + "fi_geo");
    listReq.append(prefixPath + "lam_geo" );
    listReq.append(prefixPath + "Psi");
    listReq.append(prefixPath + "c_g.x");
    listReq.append(prefixPath + "c_g.z");
    //! текущее состояние моделей
    QString prefixPathReg      = settingVV->pathToObj + "Управление моделированием.Reg.Cur.";
    listReq.append(prefixPathReg  + "startReg");
    listReq.append(prefixPathReg  + "restartReg");
    listReq.append(prefixPathReg  + "stopReg");


    reqEventCurPos = engine->getValue(listReq,IEngineData::CYCLIC_ENGINE);
    qDebug("Status=%d\n",reqEventCurPos.status);
    qDebug("Sub Status=%d\n",reqEventCurPos.sub_status[0]);
#endif
}
void MainWindowVisVar::createActions()
{
    //! добавление воздушного объекта
    actAddAirObject= new QAction(QIcon(":/res/svg/target"), QString(tr("Добавить воздушный объект")));
    //! добавление наземного объекта
    actAddGroundObject = new QAction(QIcon(":/res/svg/gtarget"), QString(tr("Добавить наземный объект")));
    //! добавление тумана
    actAddFog   = new QAction(QIcon(":/res/svg/target"), QString(tr("Добавить туман")));;
    //! добавление облачности
    actAddCloud = new QAction(QIcon(":/res/svg/target"), QString(tr("Добавить облачность")));;

    actBtnSend = new QAction(QIcon(":/png/send"), QString(tr("Загрузить варианты в ИМК")));
    actBtnSend->setShortcut(QKeySequence(tr("Ctrl+Shift+,")));

//    actBtnSendToOtherStand = new QAction(QIcon(":/png/send"), QString(tr("Загрузить варианты во второй УЦВС")));
//    actBtnSendToOtherStand->setShortcut(QKeySequence(tr("Ctrl+Shift+.")));

    //!
    actBtnHandMoveMap = new QAction(QIcon(":/png/hand"),tr("Перемещение карты"));
    actBtnHandMoveMap->setCheckable(true);
    actBtnHandMoveMap->setChecked(true);

    actBtnCursor = new QAction(QIcon(":/png/cursor"), tr(""));
    actBtnCursor->setCheckable(true);

    actBtnRuler = new QAction(QIcon(":/png/ruler"),tr("Линейка"));
    actBtnRuler->setCheckable(true);

    actBtnCentering = new QAction(QIcon(":/png/center"), tr("Центрирование по вертолету"));

    actBtnRouteDynObject = new QAction(QIcon(":/png/routeObjects"), tr("Отображение текущих координат объектов"));
    actBtnRouteDynObject->setCheckable(true);

    actBtnAllInfoObject = new QAction(QIcon(":/png/info_obj"), tr("Отображение всей информации об объекте"));
    actBtnAllInfoObject->setCheckable(true);
    actBtnAllInfoObject->setChecked(true);

    actBtnFocusMoveObj=new QAction(QIcon(":/png/center_move_obj"), tr("Центрирование по подвижному объекту нашего вертолета"));
    actBtnFocusMoveObj->setCheckable(true);

    actProjH = new QAction(QIcon(":/png/projH"),tr("Высоты"));
    actProjH->setCheckable(true);
    actProjH->setChecked(true);

    actRotateVisible = new QAction(QIcon(":/png/rotate"),tr("Видимость значка поворота у объектов"));
    actRotateVisible->setCheckable(true);
    actRotateVisible->setChecked(false);

}


void MainWindowVisVar::createMainMenuBar()
{

}

void MainWindowVisVar::createConnections()
{
    //! QActions
    connect(ui->actionCreateVar,        SIGNAL(triggered()),    this,SLOT(slotCreateMenu()));
    connect(ui->actionCreateVarCircle,  SIGNAL(triggered()),    this,SLOT(slotCreateMenuCircleVariant()));
    connect(ui->actionOpenXML,          SIGNAL(triggered()),    this,SLOT(slotOpenXML()));
    connect(ui->actionExit,             SIGNAL(triggered()),    this,SLOT(slotExitProgram()));
    connect(ui->actionSaveAs,           SIGNAL(triggered()),    this,SLOT(slotSaveAs()));
    connect(ui->actionSaveAll,          SIGNAL(triggered(bool)),    this,SLOT(slotSaveAll(bool)));
    connect(ui->actionCloseAll,         SIGNAL(triggered()),    this,SLOT(closeAllVariant()));
    connect(ui->actionExport,           SIGNAL(triggered()),    this,SLOT(slotExport()));

    connect(ui->actionAddLabel,         SIGNAL(triggered()),    this,SLOT(slotAddLabel()));
    connect(ui->actionGoto,             SIGNAL(triggered()),    this,SLOT(slotGotoLabel()));

    connect(ui->menuMapGoogle,          SIGNAL(triggered(QAction*)),  this,SLOT(slotMapGoogle(QAction*)));
    connect(ui->menuMapYandex,          SIGNAL(triggered(QAction*)),  this,SLOT(slotMapYandex(QAction*)));
    connect(ui->menuNokia,              SIGNAL(triggered(QAction*)),  this,SLOT(slotMapNokia(QAction*)));
    connect(ui->menuMapBing,            SIGNAL(triggered(QAction*)),  this,SLOT(slotMapBing(QAction*)));
    connect(ui->menuGoogle_Earth,       SIGNAL(triggered(QAction*)),  this,SLOT(slotMapGoogle_EA(QAction*)));
    connect(ui->menuLayerGoogle,        SIGNAL(triggered(QAction*)),  this,SLOT(slotLayerGoogle(QAction*)));
    connect(ui->menuLayerYandex,        SIGNAL(triggered(QAction*)),  this,SLOT(slotLayerYandex(QAction*)));

    connect(ui->actionWGS_84, SIGNAL(triggered(bool)), this, SLOT(slotActionWGS_84Triggered(bool)));
    connect(ui->actionPZ_90, SIGNAL(triggered(bool)), this, SLOT(slotActionPZ_90Triggered(bool)));

    //!
    connect(checkBoxGeography, SIGNAL(stateChanged(int)), this, SLOT(slotCheckBoxGeography(int)));
    //!
    connect(formAddLabel, SIGNAL(gotoLatLon(double,double)), this, SLOT(slotGotoLatLon(double,double)));

    connect(formManualModify, SIGNAL(createNewScene(bool)),this,SLOT(createVariant(bool)));
    connect(formManualModify, SIGNAL(cloneScene(cl_Scene*)),this,SLOT(slotCloningScene(cl_Scene*)));
    connect(formManualModify, SIGNAL(currentActiveWindow(QString)),this,SLOT(setCurrentActiveWindow(QString)));

    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(slotRunMenuScene(QAction*)));
    connect(ui->menuWindow,SIGNAL(triggered(QAction*)),this,SLOT(slotMenuWindow(QAction*)));

    connect(formZoomPanel,SIGNAL(signalZoom(int)),this,SLOT(slotZoomLevel(int)));

    connect(actBtnHandMoveMap   ,  SIGNAL(triggered(bool)), this, SLOT(slotButtonHandMoveMap(bool)));
    connect(actBtnCursor        ,  SIGNAL(triggered(bool)),this,SLOT(slotButtonCursor(bool)));
    connect(actBtnRuler         ,  SIGNAL(triggered(bool)),this,SLOT(slotButtonRuler(bool)));
    connect(actBtnCentering     ,  SIGNAL(triggered(bool)),this,SLOT(slotButtonCentering()));
    connect(actBtnRouteDynObject,  SIGNAL(triggered(bool)),this,SLOT(slotButtonRouteObject(bool)));
    connect(actBtnAllInfoObject ,  SIGNAL(triggered(bool)),this,SLOT(slotButtonAllInfoObject(bool)));
    connect(actBtnFocusMoveObj  ,  SIGNAL(triggered(bool)),this,SLOT(slotButtonFocusMoveObj(bool)));

    connect(mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(slotMdiSubWindowIsActivated(QMdiSubWindow*)));

//    connect(actBtnSend, SIGNAL(triggered(bool)), this, SLOT(slotButtonSend(bool)));
    connect(actBtnSend, SIGNAL(triggered(bool)), this, SLOT(slotButtonSendPlus(bool)));

    connect(actProjH, SIGNAL(triggered(bool)),this,SLOT(slotProjHVisible(bool)));

    connect(actRotateVisible, SIGNAL(triggered(bool)), this, SLOT(slotRotateVisible(bool)));


    connect(ui->actMppmOpen, SIGNAL(triggered(bool)), this, SLOT(slotActMppmOpen(bool)));
}

void MainWindowVisVar::slotEventsRequest(TRequestEvent answer)
{
    if(answer.uid == reqEventCurPos.uid)
    {

        if(answer.value.isEmpty() == false)
        {
            //qDebug("answer sub_status=%d\n",answer.sub_status[0]);
            emit sAircraftPos2D(rad_deg(  answer.value[1].toDouble()),
                                rad_deg( (answer.value[0].toDouble())),
                                rad_deg(-(answer.value[2].toDouble())));

            //! если останов
            if(answer.value[7].toInt() == 1 && toFlushButton == true)
            {
                toFlushButton = false;
                TCommonRequest listReq0;
                listReq0.setReciver("MPPM");
                listReq0.setSender("VAR");
                QString prefixPathReg       = settingVV->pathToObj + "Управление моделированием.InputButton.Input.";
                //! текущее состояние моделей
                listReq0.append(prefixPathReg  + "restartButton","0");
                engine->setValue(listReq0,IEngineData::ASYNCH_ENGINE);
            }
            //! если останов
            if(answer.value[6].toInt() == 1 )
            {
                slotRestart();
            }

        }
    }
}
void MainWindowVisVar::slotCheckBoxGeography(int value)
{
    useMap = value;
    if(useMap == true)
    {
        //! поменять точку привязки
        if(currentScenes!=nullptr)
            setGeoParamOfAircraft();
    }
}

void MainWindowVisVar::slotButtonAllInfoObject(bool value)
{
    if(currentScenes != nullptr)
        currentScenes->setAllInfo(value);
}
void MainWindowVisVar::slotButtonFocusMoveObj(bool value)
{
    if(currentScenes != nullptr)
        currentScenes->setFocusMoveObj(value);
}

void MainWindowVisVar::slotProjHVisible(bool b)
{
    dockWidgH->setVisible(b);
}

void MainWindowVisVar::slotRotateVisible(bool b)
{
    if(currentScenes != nullptr){
        if(b)
            currentScenes->slotRotateOn();
        else
            currentScenes->slotRotateOff();
    }
}

void MainWindowVisVar::slotButtonCentering()
{
    if(currentScenes != nullptr)
        currentScenes->centerByAircarft();
}
//! копирование сцены
void MainWindowVisVar::slotCloningScene(cl_Scene* s)
{
    cl_Scene *scene = new cl_Scene(s,this);

    formManualModify->resetModelData();
    scene->setLabelObjects(&(formAddLabel->labelObjects));

    scenes.push_back(scene);
    scene->view->installEventFilter(new cl_MouseFilterVariant(scene->view,this));
    QString strName = tr("Вариант #") + QString::number(scenes.size());

    strName+=tr(" - начальные условия");
    //!
    scene->view->setWindowTitle(strName);
    scene->index=scenes.size();
    //! укажем какой тип карты использует пользователь
    checkTypeMapAndLayer(scene);
    //! обновим тайлы
    scene->refreshTiles();
    scene->setNameVariant(scene->view->windowTitle());
    scene->setSubWindowMDI(mdiArea->addSubWindow(scene->view));

    scene->view->show();
    createWindowMenu();

    slotButtonHandMoveMap(actBtnHandMoveMap->isChecked());
}
void MainWindowVisVar::slotRestart()
{
    if(currentScenes != nullptr)
        currentScenes->slotFlushState();
}

void MainWindowVisVar::slotMdiSubWindowIsActivated(QMdiSubWindow *window)
{
    if(window == nullptr)
        return ;

    GView* view = static_cast<GView* >(window->widget());

    if(view == nullptr)
        return;
    if(currentScenes != nullptr){
        disconnect(currentScenes, SIGNAL(signalRotateVisible(bool)), actRotateVisible, SLOT(setChecked(bool)));
    }
    for(auto i : scenes)
    {
         if((i->view) == view)
         {
             if(currentScenes != nullptr)
                 currentScenes->delLabelMap();

             currentScenes = findScene(view);
             currentScenes->showLabelMap();
             setMapAndLayerContextMenu();

             formProjH->setCurrentScene(currentScenes);
//             stackedWidgProjH->setCurrentIndex(i);

             if(currentScenes->firstConnectingSlots == true)
             {
                 disconnect(currentScenes,SIGNAL(signalChangeZoom(int)),formZoomPanel,SLOT(setBeginValue(int)));
                 disconnect(formAddLabel, SIGNAL(deleteLabel(LabelObject*)),currentScenes,SLOT(deleteLabelMap(LabelObject*)));
                 disconnect(currentScenes,SIGNAL(signalUpdateValueObj(int)),formProjH,SLOT(updateScene()));
                 disconnect(this,SIGNAL(sAircraftPos2D(double,double,double)),currentScenes,SLOT(slotAircraftPos2D(double,double,double)));

             }else
                 currentScenes->firstConnectingSlots = false;

             connect(currentScenes,SIGNAL(signalChangeZoom(int)),formZoomPanel,SLOT(setBeginValue(int)));
             connect(currentScenes,SIGNAL(signalUpdateValueObj()),formProjH,SLOT(updateScene()));
             connect(formAddLabel, SIGNAL(deleteLabel(LabelObject*)),currentScenes,SLOT(deleteLabelMap(LabelObject*)));
             if(currentScenes != nullptr){
                connect(currentScenes, SIGNAL(signalRotateVisible(bool)), actRotateVisible, SLOT(setChecked(bool)));
                if(actRotateVisible->isChecked()) currentScenes->slotRotateOn();
             }

             //! обновление информации об движущемся объекте
             connect(this,SIGNAL(sAircraftPos2D(double,double,double)),currentScenes,SLOT(slotAircraftPos2D(double,double,double)));

             //! передача текущего времени
             connect(this,SIGNAL(sigTime(double)),currentScenes,SLOT(slotTime(double)));

             formZoomPanel->setBeginValue(currentScenes->currentZoom);
         }
    }
}

void MainWindowVisVar::slotButtonSend(bool b)
{
    Q_UNUSED(b)

    if(scenes.isEmpty() == false)
    {
        slotTransferStarted();

#ifdef OLD_STEND

        setGeoParamOfAircraft();
#endif
        byteArray.clear();
        //! cохраним последнее отправленное имя файла
        formManualModify->setLastNameFile(fileName);
        parser->saveVariants("./xml/LastSend.xml",  formManualModify->comment(),useMap,id,fileName);
#ifndef OLD_STEND
        //! отправление единым куском памяти
        TCommonRequest listReq;
        listReq.setReciver("MPPM");
        listReq.setSender("VAR");
        QString prefixVar           = settingVV->pathToObj  + "InitialState.Init.";
        QString prefixPathReg       = settingVV->pathToObj + "InputButton.Input.";
        currentScenes->getRequest(&listReq,prefixVar,0);
        //! здесь должен отправить признак нажатия исходного
        listReq.append(prefixPathReg  + "restartButton","1");
        requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);

        toFlushButton = true;  
#else
        //! отправление единым куском памяти
        TCommonRequest listReq;
        listReq.setReciver("MPPM");
        listReq.setSender("VAR");
        QString prefix0(prefix_folder[0]);
        if(!prefix0.isEmpty())
            prefix0 += ".";
        QString prefixVar       = prefix0 + "VAR.Setup.";
        QString prefixVarCircle = prefix0 + "VARC.Setup.";
        QString prefix          ="";

        //! кол-во вариантов по кругам
        int numsVarCircle=0;
        //! кол-во вариантов НУ
        int numsVar=0;

        for(auto i:scenes)//кол-во вариантов
        {
            if(i->use == false)
                continue;

            int num=0;
            if(i->circleVariant==true)
            {
                numsVarCircle++;
                prefix = prefixVarCircle;
                num    = numsVarCircle;
            }
            else
            {
                numsVar++;
                prefix = prefixVar;
                num    = numsVar;
            }
            i->getRequest(&listReq,prefix,num);
        }
        listReq.append(prefixVarCircle+"numberOf_Variant", QString::number(numsVarCircle));
        listReq.append(prefixVar+      "numberOfVariant",  QString::number(numsVar));

        requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);
    #endif
    }
}
void MainWindowVisVar::slotButtonSendPlus(bool b)
{
    Q_UNUSED(b)
    if(currentScenes == nullptr)
        return;

    if(scenes.isEmpty() == false)
    {
        slotTransferStarted();

#ifdef OLD_STAND
        if(useMap == true)
            setGeoParamOfAircraft();
#endif
        byteArray.clear();
        //! cохраним последнее отправленное имя файла
        formManualModify->setLastNameFile(fileName);
        parser->saveVariants("./xml/LastSend.xml",  formManualModify->comment(),useMap,id,fileName);
#ifndef OLD_STAND
        //! отправление единым куском памяти
        TCommonRequest listReq;
        listReq.setReciver("MPPM");
        listReq.setSender("VAR");
        QString prefixVar           = settingVV->pathToObj  + "InitialState.Init.";
        QString prefixPathReg       = settingVV->pathToObj + "Управление моделированием.InputButton.Input.";
        currentScenes->getRequest(&listReq,prefixVar,0);
        //! здесь должен отправить признак нажатия исходного
        listReq.append(prefixPathReg  + "restartButton","1");
        requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);

        toFlushButton = true;
#else
        //! формирование запроса на запись в УЦВС
        TCommonRequest listReq;
        listReq.setReciver("MPPM");
        listReq.setSender("VAR");
        QString prefix0(prefix_folder[0]);
        if(!prefix0.isEmpty())
            prefix0 += ".";
        QString prefixVar       = prefix0 + "VAR.Setup.";
        QString prefixVarCircle = prefix0 + "VARC.Setup.";
        QString prefix          = "";

        TCommonRequest listReq2;
        listReq2.setReciver("MPPM");
        QString prefix1(prefix_folder[1]);
        if(!prefix1.isEmpty())
            prefix1 += ".";
        listReq2.setSender(prefix1 + "VAR");
        QString prefix2      = prefix1 + "VAR.Setup.";

        //! кол-во вариантов по кругам
        int numsVarCircle=0;
        //! кол-во вариантов НУ
        int numsVar=0;
        unitedStands = false;
        for(auto i:scenes)//кол-во вариантов
        {
            if(i->use == false)
                continue;

            int num=0;
            if(i->circleVariant==true)
            {
                numsVarCircle++;
                prefix = prefixVarCircle;
                num    = numsVarCircle;
            }
            else
            {
                numsVar++;
                prefix = prefixVar;
                num    = numsVar;
            }
            i->getRequest(&listReq,prefix,num);
            if(i->isUnitedStands() && i->circleVariant==false){
                i->getRequest(&listReq2, prefix2, num);
                unitedStands = true;
            }
        }
        listReq.append(prefixVarCircle+"numberOf_Variant", QString::number(numsVarCircle));
        listReq.append(prefixVar+      "numberOfVariant",  QString::number(numsVar));
        requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);

        if(unitedStands){
            listReq2.append(prefix2 + "numberOfVariant",  QString::number(numsVar));
            requestEvent2=engine->setValue(listReq2,IEngineData::ASYNCH_ENGINE);

            //отправка на второй стенд географии земли + положение вертолета
            TCommonRequest listReq;
            listReq.setReciver("MPPM");            
            QString prefix1(prefix_folder[1]);
            if(!prefix1.isEmpty())
                prefix1 += ".";
            listReq.setSender(prefix1 + "InitGEO");
            QString prefixSetupGeo = prefix1 + "InitGEO.Setup.";

            listReq.append(prefixSetupGeo + "Aust_0_geography",     QString::number(0.0));
            listReq.append(prefixSetupGeo + "fiust_0_geography",    QString::number(currentScenes->aircraft->lat,'g', 10));
            listReq.append(prefixSetupGeo + "limdaust_0_geography", QString::number(currentScenes->aircraft->lon,'g', 10));

            GeographicLib::LocalCartesian locals1(currentScenes->aircraft->lat, currentScenes->aircraft->lon, 0/*h*/, *earth);
            double xx, zz;
            double unused_val;
            locals1.Forward(currentScenes->aircraft2->lat, currentScenes->aircraft2->lon, 0/*h*/, zz, xx, unused_val);
            currentScenes->aircraft2->setX_ust(xx);
            currentScenes->aircraft2->setZ_ust(zz);

            listReq.append(prefixSetupGeo + "x0_ust", QString::number(currentScenes->aircraft2->x_ust(), 'g', 10));
            listReq.append(prefixSetupGeo + "z0_ust", QString::number(currentScenes->aircraft2->z_ust(), 'g', 10));
            requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);
        }
#endif
//        QTimer::singleShot(1000, this, SLOT(slotVPultRestartPressed()));
    }
}
void MainWindowVisVar::slotButtonHandMoveMap(bool flag)
{
    actBtnHandMoveMap->setChecked(true);
    if(!actBtnRuler->isChecked() && !actBtnCursor->isChecked())
        return;
    if(flag == true)
    {
        actBtnRuler->setChecked(false);
        if(currentScenes!=nullptr)
            currentScenes->deleteRoutes();

        actBtnCursor->setChecked(false);
    }

    if(currentScenes != nullptr)
        currentScenes->activeAddLabel=false;

    for(auto i:scenes)
    {
        i->setScrollViewHand(flag);
    }
}
void MainWindowVisVar::slotButtonCursor(bool flag)
{
    if(flag==true)
    {
        slotButtonHandMoveMap(false);
        actBtnRuler->setChecked(false);

        if(currentScenes!=nullptr)
            currentScenes->deleteRoutes();

        actBtnHandMoveMap->setChecked(false);
    }
    actBtnCursor->setChecked(true);
}
void MainWindowVisVar::slotButtonRuler(bool flag)
{
    if(flag == true)
    {
        slotButtonHandMoveMap(false);

        actBtnCursor->setChecked(false);
        actBtnHandMoveMap->setChecked(false);
        actBtnRuler->setChecked(true);

    }else
    {
         if(currentScenes!=nullptr)
             currentScenes->deleteRoutes();

         slotButtonHandMoveMap(true);
    }
    if(currentScenes!=nullptr)
        currentScenes->setActiveRoute(flag);
}
void MainWindowVisVar::checkTypeMapAndLayer(cl_Scene *scene)
{
    if(scene != nullptr)
    {
        QString mapStr = "";
        if(ui->actionLandGoogle->isChecked() == true)
        {
            mapStr = tr("Google(Ладншафт)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_LAND);
        }
        if(ui->actionMapsGoogle->isChecked() == true)
        {
            mapStr = tr("Google(Карта)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_MAP);
        }
        if(ui->actionSatGoogle->isChecked() == true)
        {
            mapStr = tr("Google(Спутник)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_SAT);
        }
        if(ui->actionSatYandex->isChecked() == true)
        {
            mapStr = tr("Яндекс(Спутник)");
            scene->setTypeMAP(GeographySysCoord::YANDEX_SAT);
        }
        if(ui->actionSatBing->isChecked() == true)
        {
            mapStr = tr("Bing(Спутник)");
            scene->setTypeMAP(GeographySysCoord::BING_SAT);
        }
        if(ui->actionSatGoogleEa->isChecked() == true)
        {
            mapStr = tr("Google Earth(Спутник)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_SAT_EARTH);
        }
        if(ui->actionNokiaSatHyb->isChecked() == true)
        {
            mapStr=tr("Nokia(Спутник+гибрид)");
            scene->setTypeMAP(GeographySysCoord::NOKIA_HYB);
        }
        if(ui->actionNokiaMaps->isChecked() == true)
        {
            mapStr=tr("Nokia(Карты)");
            scene->setTypeMAP(GeographySysCoord::NOKIA_MAP);
        }

        QString layerStr = tr("");
        if(ui->actionHybYandex->isChecked() == true)
        {
            layerStr = tr("гибрид(Яндекс)");
            scene->setTypeLayer(GeographySysCoord::YANDEX_HYB);
        }else if(ui->actionHybGoogle->isChecked()==true)
        {
            layerStr = tr("гибрид(Google)");
            scene->setTypeLayer(GeographySysCoord::GOOGLE_HYB);
        }else
        {
            layerStr = tr("");
            scene->setTypeLayer(GeographySysCoord::NO_LAYER);
        }

        if(layerStr.isEmpty() == true)
            statusBar->setSource(mapStr);
        else
            statusBar->setSource(mapStr + "+" + layerStr);

        scene->refreshZoomLevel();
    }
}

void MainWindowVisVar::checkTypeMapAndLayer()
{
    for(auto i:scenes)
    {
        checkTypeMapAndLayer(i);
    }
}
void MainWindowVisVar::slotMapGoogle(QAction* act)
{
    if(act->isChecked() == true)
    {
        if(act == ui->actionSatGoogle)
        {
            ui->actionLandGoogle->setChecked(false);
            ui->actionMapsGoogle->setChecked(false);
        }else if(act == ui->actionLandGoogle)
        {
            ui->actionSatGoogle->setChecked(false);
            ui->actionMapsGoogle->setChecked(false);
        }else if(act == ui->actionMapsGoogle)
        {
            ui->actionSatGoogle->setChecked(false);
            ui->actionLandGoogle->setChecked(false);
        }
        ui->actionSatYandex->setChecked(false);
        ui->actionSatBing->setChecked(false);
        ui->actionSatGoogleEa->setChecked(false);
        ui->actionNokiaMaps->setChecked(false);
        ui->actionNokiaSatHyb->setChecked(false);
        checkTypeMapAndLayer();
    }
}
void MainWindowVisVar::slotMapYandex(QAction* act)
{
    if(act->isChecked() == true)
    {
        ui->actionSatGoogle->setChecked(false);
        ui->actionMapsGoogle->setChecked(false);
        ui->actionLandGoogle->setChecked(false);
        ui->actionSatBing->setChecked(false);
        ui->actionSatGoogleEa->setChecked(false);
        ui->actionNokiaMaps->setChecked(false);
        ui->actionNokiaSatHyb->setChecked(false);

        checkTypeMapAndLayer();
    }
}
void MainWindowVisVar::slotMapBing(QAction* act)
{
    if(act->isChecked() == true)
    {
        ui->actionSatGoogle->setChecked(false);
        ui->actionMapsGoogle->setChecked(false);
        ui->actionLandGoogle->setChecked(false);
        ui->actionSatYandex->setChecked(false);
        ui->actionSatGoogleEa->setChecked(false);
        ui->actionNokiaMaps->setChecked(false);
        ui->actionNokiaSatHyb->setChecked(false);
        checkTypeMapAndLayer();
    }
}
void MainWindowVisVar::slotMapGoogle_EA(QAction* act)
{
    if(act->isChecked() == true)
    {
        ui->actionSatGoogle->setChecked(false);
        ui->actionMapsGoogle->setChecked(false);
        ui->actionLandGoogle->setChecked(false);
        ui->actionSatYandex->setChecked(false);
        ui->actionSatBing->setChecked(false);
        ui->actionNokiaMaps->setChecked(false);
        ui->actionNokiaSatHyb->setChecked(false);
        checkTypeMapAndLayer();
    }
}
void MainWindowVisVar::slotMapNokia(QAction* act)
{
    if(act->isChecked() == true)
    {
        if(act == ui->actionNokiaMaps)
        {
            ui->actionNokiaSatHyb->setChecked(false);
        }else if(act == ui->actionNokiaSatHyb)
        {
            ui->actionNokiaMaps->setChecked(false);
        }

        ui->actionSatGoogle->setChecked(false);
        ui->actionMapsGoogle->setChecked(false);
        ui->actionLandGoogle->setChecked(false);
        ui->actionSatYandex->setChecked(false);
        ui->actionSatBing->setChecked(false);
        ui->actionSatGoogleEa->setChecked(false);
        checkTypeMapAndLayer();
    }
}
void MainWindowVisVar::slotLayerGoogle(QAction* act)
{
    if(act->isChecked() == true)
    {
        ui->actionHybYandex->setChecked(false);
    }
    checkTypeMapAndLayer();
}
void MainWindowVisVar::slotLayerYandex(QAction* act)
{
    if(act->isChecked() == true)
    {
        ui->actionHybGoogle->setChecked(false);
    }
    checkTypeMapAndLayer();
}

void MainWindowVisVar::slotExport()
{
    if(scenes.isEmpty() == false)
    {
        parser->createXMLForModel(formManualModify->comment(),useMap,id);
    }
}
void MainWindowVisVar::slotButtonSendFile()
{
}
void MainWindowVisVar::slotSaveAs()
{
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setNameFilter(tr("XML-variants(*.xml)"));
    dialog->exec();

    QStringList fileNames = dialog->selectedFiles();
    for(auto i:fileNames)
    {
        bool res = i.contains(".xml");
        if(res == false)
        {
            if(fileNames[0] == "")
                fileName = "";
            else
                fileNames[0] += ".xml";
        }else
            fileName = fileNames[0];

    }
    if(fileNames.isEmpty() == false && fileName!="")
    {
        setWindowTitle(tr("Редактор вариантов")+" [" + fileName + "]");
        parser->saveVariants(fileName,formManualModify->comment(),useMap,id);
    }
}

void MainWindowVisVar::slotSaveAll(bool b)
{
    Q_UNUSED(b)
    if(fileName.isEmpty() == false)
        parser->saveVariants(fileName,formManualModify->comment(),useMap,id);
    else
        slotSaveAs();
}
void MainWindowVisVar::slotAddLabel()
{
    if(currentScenes != nullptr)
    {
        slotButtonCursor(true);
        currentScenes->activeAddLabel = true;
        currentScenes->view->setCursor(Qt::CrossCursor);
        disconnect(currentScenes,
                SIGNAL(activeFormAddLabel(double,double)),
                this,
                SLOT(slotRunFormAddLabel(double,double)));
        connect(currentScenes,
                SIGNAL(activeFormAddLabel(double,double)),
                this,
                SLOT(slotRunFormAddLabel(double,double)));
    }


}
void MainWindowVisVar::slotGotoLabel()
{
    formAddLabel->show();
}
void MainWindowVisVar::slotGotoLatLon(double lat,double lon)
{
    if(currentScenes != nullptr)
    {
        currentScenes->setCenterWindowView(lat,lon,currentScenes->currentZoom);
    }
}
void MainWindowVisVar::slotRunFormAddLabel(double lat,double lon)
{
    formAddLabel->addNewLabel(lat,lon);
    formAddLabel->show();

    if(currentScenes != nullptr)
        currentScenes->calcItemPosScene();
}
//! создание варианта(обычный или по кругам)
void MainWindowVisVar::createVariant()
{
    if(scenes.isEmpty()==true)
    {
        fileName = "";
        id = qrand();
        this->setWindowTitle(tr("Начальные условия моделирования"));
    }
    cl_Scene *scene = new cl_Scene(statusBar,typeObjectsVis,&infoObjects,settingVV,this);
    //!
    formManualModify->resetModelData();
    //!
    scene->setLabelObjects(&(formAddLabel->labelObjects));
    //scene->view->setWindowFlags(Qt::WindowMinMaxButtonsHint);

    int index_prev = 1;
    scene->numberNameVariant = index_prev;
    if(scenes.isEmpty() == false)
    {
        index_prev = (scenes.back())->numberNameVariant;
        scene->numberNameVariant = index_prev + 1;
    }
    scenes.push_back(scene);
    scene->view->installEventFilter(new cl_MouseFilterVariant(scene->view,this));
    QString strName = tr("Вариант #")+QString::number(scene->numberNameVariant);


        strName+=tr(" - начальные условия");
    //!
    scene->view->setWindowTitle(strName);
    scene->index = scenes.size();
    //! укажем какой тип карты использует пользователь
    checkTypeMapAndLayer(scene);
    //! обновим тайлы
    scene->refreshTiles();
    scene->setNameVariant(scene->view->windowTitle());
    scene->setSubWindowMDI(mdiArea->addSubWindow(scene->view));

    scene->view->show();
    createWindowMenu();


    slotButtonHandMoveMap(actBtnHandMoveMap->isChecked());
}
void MainWindowVisVar::slotCreateMenu()
{
    createVariant();
}

void MainWindowVisVar::setCurrentActiveWindow(QString name)
{
    QList<QMdiSubWindow*> list = mdiArea->subWindowList();

    for(auto i:list)
    {
        GView* view = static_cast <GView* > (i->widget());
        if(view->windowTitle() == name)
        {
            mdiArea->setActiveSubWindow(i);
            //list[i]->setFocus();//showNormal();
        }
    }
}
void MainWindowVisVar::slotMenuWindow(QAction * act)
{
    QList<QMdiSubWindow*> list = mdiArea->subWindowList();

    for(auto i:actList)
    {
        if(i != act)
            i->setChecked(false);
        else
            i->setChecked(true);
    }
    ui->menuWindow->update();

    for(auto i:list)
    {
        GView* view = static_cast <GView* > (i->widget());
        if(view->windowTitle() == act->text())
        {
            mdiArea->setActiveSubWindow(i);
            i->showNormal();
        }
    }
}
void MainWindowVisVar::slotZoomLevel(int zoom)
{
    QMdiSubWindow* subWindow=mdiArea->currentSubWindow();
    if(subWindow == nullptr){
        if(!mdiArea->subWindowList().isEmpty())
            subWindow = mdiArea->subWindowList().first();
        else
            return;
    }

    GView* view = static_cast <GView* > (subWindow->widget());
    cl_Scene *scene = findScene(view);
    if(scene != nullptr)
    {
        scene->setCurrentGeoOnCenter();
        scene->setZoomLevel(zoom);
        formZoomPanel->setBeginValue(zoom);
    }
}
void MainWindowVisVar::createWindowMenu()
{
    ui->menuWindow->clear();
    actList.clear();

    QList<QMdiSubWindow*> list      = mdiArea->subWindowList();
    QMdiSubWindow* currentSubWindow = mdiArea->currentSubWindow();

    for(auto curWin:list)
    {
        GView* view = static_cast<GView* > (curWin->widget());
        QAction* act = ui->menuWindow->addAction(view->windowTitle());
        actList.push_back(act);
        act->setCheckable(true);

        if(curWin == currentSubWindow)
            act->setChecked(true);
        else
            act->setChecked(false);
    }
}

void MainWindowVisVar::rightButtonMouseClicked()
{
    if(currentScenes->activeRoute == true || currentScenes->activeAddLabel == true)
        return;

    QAction *act = nullptr;
    menu->clear();
    menu->addAction(actAddAirObject);;//.tr("Добавить воздушный объект"));
    menu->addAction(actAddGroundObject);;//tr("Добавить наземный объект"));
    menu->addAction(actAddCloud);
    menu->addAction(actAddFog);

    //menu->addAction(tr("Добавить туман"));
    //menu->addAction(tr("Добавить облачность"));
    menu->addSeparator();
    act=menu->addAction(tr("Добавить маяк РСБН"));
    act->setEnabled(false);
    act=menu->addAction(tr("Добавить маяк VOR"));
    act->setEnabled(false);

    act=menu->addAction(tr("Добавить аэродром"));
    act->setEnabled(false);
    //menu->addSeparator();
    //menu->addAction(tr("Добавить информационную точку"));
    //menu->addSeparator();
    //menu->addAction(tr("Добавить ППМ для ПЗ"));

    //menu->addSeparator();
    //menu->addAction(tr("Скопировать объекты из..."));

    menu->setContextMenuPolicy(Qt::ActionsContextMenu);
    posMouseMenu=QCursor::pos();
    menu->exec(QCursor::pos());
}

void MainWindowVisVar::slotRunMenuScene(QAction* act)
{
    QMdiSubWindow* subWindow=mdiArea->currentSubWindow();
    if(subWindow == nullptr){
        if(!mdiArea->subWindowList().isEmpty())
            subWindow = mdiArea->subWindowList().first();
        else
            return;
    }

    GView* view = static_cast<GView* > (subWindow->widget());

    cl_Scene *scene = findScene(view);

    if(scene == nullptr)
        return;

    QPointF posScene = view->mapToScene(view->mapFromGlobal(posMouseMenu));

    if(act == actAddAirObject)
    {
        scene->createNewAirObj(posScene);
    }else if(act == actAddGroundObject)
    {
        scene->createNewGroundObj(posScene);
    }else if(act == actAddCloud)
    {
        //if(scene->circleVariant == false)
        //    scene->createNewGroundObj(posScene);
    }else if(act == actAddFog)
    {
        //if(scene->circleVariant == false)
        //    scene->createNewGroundObj(posScene);
    }else if(act->text() == tr("Добавить аэродром"))
    {
        scene->createNewAerodrom(posScene);
    }else if(act->text() == tr("Добавить информационную точку"))
    {
        scene->createNewInfoObject(posScene);
        for(auto i:scenes)
        {
            i->refreshInfo();
        }
    }else if(act->text() == tr("Добавить маяк РСБН"))
        scene->createNewBeaconObject(posScene);

    else if(act->text() == tr("Добавить маяк VOR"))
        scene->createNewBeaconObject(posScene);

    formManualModify->resetModelData();
}

void MainWindowVisVar::slotExitProgram()
{
    qApp->exit();
}
void MainWindowVisVar::showAllVariant()
{
    for(auto i:scenes)
    {
        i->view->installEventFilter(new cl_MouseFilterVariant(i->view,this));
        i->view->setWindowTitle(i->returnNameVariant());
        i->setLabelObjects(&(formAddLabel->labelObjects));
        i->setSubWindowMDI(mdiArea->addSubWindow(i->view));
        i->view->showMaximized();
    }
    if(!scenes.isEmpty())
        currentScenes = scenes[scenes.count()-1];
    createWindowMenu();
    setMapAndLayerContextMenu();
}
void MainWindowVisVar::setMapAndLayerContextMenu()
{
    ui->actionSatGoogle ->setChecked(false);
    ui->actionMapsGoogle->setChecked(false);
    ui->actionLandGoogle->setChecked(false);
    ui->actionSatBing   ->setChecked(false);
    ui->actionSatGoogleEa->setChecked(false);
    ui->actionNokiaMaps  ->setChecked(false);
    ui->actionNokiaSatHyb->setChecked(false);

    ui->actionHybYandex->setChecked(false);
    ui->actionHybGoogle->setChecked(false);

    if(currentScenes!=nullptr)
    {
        switch(currentScenes->map->isTypeMap())
        {
            case GeographySysCoord::GOOGLE_SAT:         {ui->actionSatGoogle->setChecked(true);break;}
            case GeographySysCoord::GOOGLE_MAP:         {ui->actionMapsGoogle->setChecked(true);break;}
            case GeographySysCoord::GOOGLE_LAND:        {ui->actionLandGoogle->setChecked(true);break;}
            case GeographySysCoord::BING_SAT:           {ui->actionSatBing->setChecked(true);break;}
            case GeographySysCoord::GOOGLE_SAT_EARTH:   {ui->actionSatGoogleEa->setChecked(true);break;}
            case GeographySysCoord::NOKIA_MAP:          {ui->actionNokiaMaps->setChecked(true);break;}
            case GeographySysCoord::NOKIA_HYB:          {ui->actionNokiaSatHyb->setChecked(true);break;}
            case GeographySysCoord::YANDEX_SAT:         {ui->actionSatYandex->setChecked(true);break;}
            case GeographySysCoord::YANDEX_MAP:         {ui->actionMapsYandex->setChecked(true);break;}
        };

        switch(currentScenes->map->isTypeLayer())
        {
            case GeographySysCoord::GOOGLE_HYB:         {ui->actionHybGoogle->setChecked(true);break;}
            case GeographySysCoord::YANDEX_HYB:         {ui->actionHybYandex->setChecked(true);break;}
        };

    }
}
void MainWindowVisVar::closeAllVariant()
{
    this->setWindowTitle(tr("Начальные условия моделирования"));
    currentScenes = nullptr;
    QList<QMdiSubWindow*> list = mdiArea->subWindowList();
    for(auto i:list)
    {
        i->close();
    }

    for(auto i:scenes)
    {
        i->view->close();
        cl_Scene *tempScene = i;
        delete tempScene;
    }
    scenes.clear();
}
//! обработка событий
bool MainWindowVisVar::event(QEvent *event)
{
    if(statusBar != nullptr)
        statusBar->setGeometry(statusBar->x(), statusBar->y(), width(), statusBar->height());
    return QMainWindow::event(event);
}
//! открыть файл с вариантами
void MainWindowVisVar::slotOpenXML()
{
    //! здесь нужна проверка того были ли созданы уже варинаты, если да, то нужно предложить сохранить перед загрузкой
    if(scenes.isEmpty() == false)
    {
        dialogSave->exec();
        if(dialogSave->ok == true)
        {
            slotSaveAs();
        }
    }

    closeAllVariant();

    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilter(tr("XML-variants(*.xml)"));

    dialog->selectFile("");
    dialog->exec();

    QStringList fileNames = dialog->selectedFiles();
    if(fileNames.isEmpty() == false)
    {
        if(fileNames[0].isEmpty() == false)
            fileName = fileNames[0];
        this->setWindowTitle(tr("Начальные условия моделирования")+" ["+fileName+"]");

        if(fileNames.isEmpty() == false)
        {
            openFile(fileNames[0]);
        }
    }
}
//! открыть файл с вариантами
bool MainWindowVisVar::openFile(QString fileName)
{
    bool openFile = parser->openFileVariants(fileName,
                                           &comment,
                                           useMap,id);
    //! использование цифровой карты
    checkBoxGeography->setChecked(useMap);
    //! комментарий
    if(formManualModify != nullptr)
        formManualModify->setComment(comment);

    if(openFile == true)
    {
        //! показать все варианты
        showAllVariant();
        //! читаем параметры географии
        //readSetupGeography();
        //! пересчет положения объектов, если нет привязки к цифровой карте
        //reCalcVar();
        //! сбросить модельные данные
        if(formManualModify !=0)
            formManualModify->resetModelData();
    }
    return openFile;
}
void MainWindowVisVar::slotSaveXML()
{
    //parser->addNewVariant("name",this->scenes);
    //parser->saveVariants();
}
void MainWindowVisVar::slotTransferStarted()
{
    statusBar->setStatusTransmit(tr("Идет отправление варианта..."),FormStatusBar::ST_PREPARE);
}
void MainWindowVisVar::slotTransferFinished(int code, QString errString)
{
    QString time = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
    if(code == 0)
        statusBar->setStatusTransmit(tr("Успешно: дата ") +  time,
                                     FormStatusBar::ST_GOOD);
    else
        statusBar->setStatusTransmit(/*tr("Ошибка: ")+*/errString,
                                     FormStatusBar::ST_BAD);
}
cl_Scene* MainWindowVisVar::findScene(GView *view)
{
    for(auto i:scenes)
    {
        if(i->view == view)
            return i;
    }
    return nullptr;
}
//! запись в структуру "Установка географии"
void MainWindowVisVar::setGeoParamOfAircraft()
{
    if(scenes.isEmpty() == false)
    {
        TCommonRequest listReq;
        listReq.setReciver("MPPM");
        listReq.setSender("VAR");        
//        QString prefix0(prefix_folder[0]);
//        if(!prefix0.isEmpty())
//            prefix0 += ".";
        QString prefixSetupGeo       = /*prefix0 +*/ "InitGEO.Setup.";

        listReq.append(prefixSetupGeo + "Aust_0_geography",     QString::number(0.0));
        listReq.append(prefixSetupGeo + "fiust_0_geography",    QString::number(currentScenes->aircraft->lat,'g', 10));
        listReq.append(prefixSetupGeo + "limdaust_0_geography", QString::number(currentScenes->aircraft->lon,'g', 10));

        listReq.append(prefixSetupGeo + "x0_ust",               QString::number(0.0));
        listReq.append(prefixSetupGeo + "z0_ust",               QString::number(0.0));


        requestEvent = engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);
    }
}
//! чтение структуры "География земли"
void MainWindowVisVar::readSetupGeography()
{
    /*engine->getValue("MPPM.InitGEO.Setup.Aust_0_geography",       psi_model,IEngineData::ASYNCH_ENGINE);
    engine->getValue("MPPM.InitGEO.Setup.fiust_0_geography",      lat_model,IEngineData::SYNCH_ENGINE);
    engine->getValue("MPPM.InitGEO.Setup.limdaust_0_geography",   lon_model,IEngineData::SYNCH_ENGINE);
    engine->getValue("MPPM.InitGEO.Setup.x0_ust",                 x_model,  IEngineData::SYNCH_ENGINE);
    engine->getValue("MPPM.InitGEO.Setup.z0_ust",                 z_model,  IEngineData::SYNCH_ENGINE);*/
}
void MainWindowVisVar::slotIds(TRequestEvent eventRequest)
{
    if(eventRequest.sender == "MPPM")
    {
        if(requestEvent.uid == eventRequest.uid)
        {
            if(eventRequest.status == TRequestEvent::PROCESSED)
                slotTransferFinished(0x0,"");
            else
                slotTransferFinished(0x1,tr("Ошибка передачи"));
        }
    }
}
void MainWindowVisVar::slotOpenAndSendVariant(QString nameFile)
{
    closeAllVariant();
    bool isOpen = parser->openFileVariants(nameFile,&comment,useMap,id);
    if(isOpen == true)
    {
        showAllVariant();
        formManualModify->resetModelData();
//        slotButtonSend(true);
        slotButtonSendPlus(true);
    }
}
void MainWindowVisVar::slotButtonRouteObject(bool value)
{
    for(auto i:scenes)
    {
        i->slotUseMoveObj(value);
    }
}
MainWindowVisVar::~MainWindowVisVar()
{
    delete ui;
}
void MainWindowVisVar::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//! пересчет положения объектов, если нет привязки к цифровой карте
void MainWindowVisVar::reCalcVar()
{
    if(useMap == false)//! если не используется цифровая карта
    {
        for(auto i:scenes)//! для всех сцен расчитываем новое положение объектов
        {
            //! персчет объектов относительно модельной СК
            i->reCalcObject(lat_model.toDouble(),
                            lon_model.toDouble(),
                            psi_model.toDouble());
        }
    }
}
void MainWindowVisVar::slotActionWGS_84Triggered(bool b)
{
    ui->actionWGS_84->setChecked(true);
    if(b){
        earth = earthWGS;
        ui->actionPZ_90->setChecked(false);
    }
}

void MainWindowVisVar::slotActionPZ_90Triggered(bool b)
{
    ui->actionPZ_90->setChecked(true);
    if(b){
        earth = earthPZ90;
        ui->actionWGS_84->setChecked(false);
    }
}

}
