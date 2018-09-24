#include <QMdiSubWindow>
#include <QMouseEvent>
#include <QFileDialog>
//#include <QBitmap>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cl_Scene.h"
#include "./mppm/CommonEngineData.h"
#include "./mppm/libmppm.h"
#include <QLibrary>


typedef IEngineData* (*CreateEngine)();

namespace VisualVariant
{
cl_MouseFilterVariant::cl_MouseFilterVariant(QObject* pobj,QWidget *slotForm_):QObject(pobj)
{
    slotForm=slotForm_;
}
bool cl_MouseFilterVariant::eventFilter(QObject* pobj,QEvent *event)
{
    if(event->type()==QEvent::ContextMenu)
    {
        MainWindowVisVar *p=qobject_cast<MainWindowVisVar* >(slotForm);
        if(p!=0)
        {
            p->rightButtonMouseClicked();
            return true;
        }else return false;
    }else if(event->type()==QEvent::Close)
    {
        MainWindowVisVar *p=qobject_cast<MainWindowVisVar* >(slotForm);
        if(p!=0)
        {

        for(int i=0;i<p->scenes.size();i++)
        {
            QObject* ptr=p->scenes[i]->view;
            if(ptr==pobj)
            {
                p->scenes.remove(i);
                p->formManualModify->deleteScene(i);
                p->formManualModify->resetModelData();
            }
        }
        return true;
        }else return false;
    }
    return false;
}

MainWindowVisVar::MainWindowVisVar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowVariant)
{
    ui->setupUi(this);
    ObjectGraphNode::initObjectsProperty();
    settingVV=new SettingVV(this);
    formProjH=new FormProjectH;
    ui->toolBarH->addWidget(formProjH);

#ifdef QT_DEBUG
    QLibrary libMPPM("libmppmd");
#else
    QLibrary libMPPM("libmppm");
#endif

    CreateEngine func = (CreateEngine)libMPPM.resolve("createEngine");
    if(func == 0)
    {
        QMessageBox::warning(this, tr("Warning!"),
                             tr("libMPPM: (CreateEngine)libMPPM.resolve(\"createEngine\") = 0. \n"
                                "Для устранения неполадки обратитесь к разработчику."),
                             QMessageBox::Ok);

    }
    engine = func();

    menu            = new QMenu;
    statusBar       = new FormStatusBar;
    typeObjectsVis  = new TypeObjectsVis;
    dialogSave      = new DialogSaveCurVar;
    formManualModify= new FormManualModify(&scenes);
    engineVariant   = new VariantEngineData;
    //! форма добавления новой метки
    formAddLabel    = new FormAddLabelMap;

    //! текущая сцена не определена
    currentScenes   = 0;
    //! по умолчанию используем цифровую карту
    useMap          = true;

    connect(engineVariant,
            SIGNAL(signalLoadVariant(QString)),
            this,
            SLOT(slotOpenAndSendVariant(QString)));

    buttonSend=new QToolButton;
    buttonSend->setIcon(QIcon(":/png/send"));
    buttonSend->setToolTip(tr("Загрузить варианты в УЦВС"));
    buttonSendFile=new QToolButton;

    buttonSaveAll=new QToolButton;
    buttonSaveAll->setIcon(QIcon(":/png/save"));
    buttonSaveAll->setToolTip(tr("Сохранить все варианты"));

    buttonVar=new QToolButton;
    buttonVar->setIcon(QIcon(":/png/var"));
    buttonVar->setToolTip(tr("Создать вариант начальных условий с воздушными и наземными целями(Кл. Ctrl+1)"));

    buttonCir=new QToolButton;
    buttonCir->setIcon(QIcon(":/png/circ"));
    buttonCir->setToolTip(tr("Создать вариант начальных условий движения по кругам(Кл. Ctrl+2)"));

    checkBoxGeography=new QCheckBox(tr("Привязка к карте"));
    connect(checkBoxGeography,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(slotCheckBoxGeography(int)));

    pathFileToSend=new QLineEdit(tr("По умолчанию"));
    ui->toolBarSendFile->addWidget(formManualModify);

    buttonHandMoveMap=new QToolButton;
    buttonHandMoveMap->setCheckable(true);
    buttonHandMoveMap->setChecked(true);
    buttonHandMoveMap->setIcon(QIcon(":/png/hand"));
    buttonHandMoveMap->setToolTip(tr("Перемещение карты"));

    buttonRuler=new QToolButton;
    buttonRuler->setCheckable(true);
    buttonRuler->setIcon(QIcon(":/png/ruler"));
    buttonRuler->setToolTip(tr("Линейка"));

    buttonCentering=new QToolButton;
    buttonCentering->setIcon(QIcon(":/png/yellow"));
    buttonCentering->setToolTip(tr("Центрирование по самолету"));

    buttonCursor=new QToolButton;
    buttonCursor->setCheckable(true);
    buttonCursor->setIcon(QIcon(":/png/cursor"));

    buttonRouteDynObject=new QToolButton;
    buttonRouteDynObject->setCheckable(true);
    buttonRouteDynObject->setToolTip(tr("Отображение текущих координат объектов"));
    buttonRouteDynObject->setIcon(QIcon(":/png/routeObjects"));

    buttonAllInfoObject=new QToolButton;
    buttonAllInfoObject->setCheckable(true);
    buttonAllInfoObject->setToolTip(tr("Отображение всей информации об объекте"));
    buttonAllInfoObject->setIcon(QIcon(":/png/info_obj"));

//    varExchange=new VarExchange;
//    varExchange->SetTarget      (settingVV->hostAddress);
//    varExchange->SetFileName    (settingVV->fileName);
//    varExchange->SetLogin       (settingVV->userName,settingVV->password);
//    varExchange->SetForCommandCD(settingVV->pathCD);
//    varExchange->SetMaxCountTimer(settingVV->FTPWait);

    parser=new Parser(statusBar,&scenes,typeObjectsVis);
    connect(ui->actionCreateVar,        SIGNAL(triggered()),    this,SLOT(slotCreateMenu()));
    connect(ui->actionCreateVarCircle,  SIGNAL(triggered()),    this,SLOT(slotCreateMenuCircleVariant()));
    connect(ui->actionOpenXML,          SIGNAL(triggered()),    this,SLOT(slotOpenXML()));
    connect(ui->actionExit,             SIGNAL(triggered()),    this,SLOT(slotExitProgram()));
    connect(ui->actionSaveAs,           SIGNAL(triggered()),    this,SLOT(slotSaveAs()));
    connect(ui->actionSaveAll,          SIGNAL(triggered()),    this,SLOT(slotSaveAll()));
    connect(ui->actionCloseAll,         SIGNAL(triggered()),    this,SLOT(closeAllVariant()));
    connect(ui->actionExport,           SIGNAL(triggered()),    this,SLOT(slotExport()));

    connect(ui->actionAddLabel,         SIGNAL(triggered()),    this,SLOT(slotAddLabel()));
    connect(ui->actionGoto,             SIGNAL(triggered()),    this,SLOT(slotGotoLabel()));
    connect(formAddLabel,
            SIGNAL(gotoLatLon(double,double)),
            this,
            SLOT(slotGotoLatLon(double,double)));


    connect(ui->menuMapGoogle,          SIGNAL(triggered(QAction*)),  this,SLOT(slotMapGoogle(QAction*)));
    connect(ui->menuMapYandex,          SIGNAL(triggered(QAction*)),  this,SLOT(slotMapYandex(QAction*)));
    connect(ui->menuNokia,              SIGNAL(triggered(QAction*)),  this,SLOT(slotMapNokia(QAction*)));
    connect(ui->menuMapBing,            SIGNAL(triggered(QAction*)),  this,SLOT(slotMapBing(QAction*)));
    connect(ui->menuGoogle_Earth,       SIGNAL(triggered(QAction*)),  this,SLOT(slotMapGoogle_EA(QAction*)));
    connect(ui->menuLayerGoogle,        SIGNAL(triggered(QAction*)),  this,SLOT(slotLayerGoogle(QAction*)));
    connect(ui->menuLayerYandex,        SIGNAL(triggered(QAction*)),  this,SLOT(slotLayerYandex(QAction*)));

    connect(formManualModify,           SIGNAL(createNewScene(bool)),this,SLOT(createVariant(bool)));
    connect(formManualModify,           SIGNAL(cloneScene(cl_Scene*)),this,SLOT(slotCloningScene(cl_Scene*)));

    connect(formManualModify,           SIGNAL(currentActiveWindow(QString)),this,SLOT(setCurrentActiveWindow(QString)));

    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(slotRunMenuScene(QAction*)));
    connect(ui->menuWindow,SIGNAL(triggered(QAction*)),this,SLOT(slotMenuWindow(QAction*)));

//    connect(varExchange,SIGNAL(transfer_started()),this,SLOT(slotTransferStarted()));
//    connect(varExchange,SIGNAL(transfer_finished(int, QString)),this,SLOT(slotTransferFinished(int,QString)));

    formZoomPanel=new FormZoomPanel;
    ui->statusBar->addWidget(statusBar);
    ui->toolBarOperation->addWidget(formZoomPanel);
    connect(formZoomPanel,SIGNAL(signalZoom(int)),this,SLOT(slotZoomLevel(int)));

    ui->toolBarOperation->addWidget(buttonHandMoveMap);
    ui->toolBarOperation->addWidget(buttonCursor);
    ui->toolBarOperation->addWidget(buttonRuler);
    ui->toolBarOperation->addWidget(buttonCentering);
    ui->toolBarOperation->addWidget(buttonRouteDynObject);
    ui->toolBarOperation->addWidget(buttonAllInfoObject);

    ui->toolBarCreate->addWidget(buttonSaveAll);
    ui->toolBarCreate->addWidget(buttonVar);
    ui->toolBarCreate->addWidget(buttonCir);
    ui->toolBarCreate->addWidget(checkBoxGeography);

    ui->toolBarMain->addWidget(buttonSend);

    connect(buttonSend,SIGNAL(clicked()),this,SLOT(slotButtonSend(void)));
    connect(buttonVar, SIGNAL(clicked()),this,SLOT(slotCreateMenu()));
    connect(buttonCir, SIGNAL(clicked()),this,SLOT(slotCreateMenuCircleVariant()));
    connect(buttonSaveAll, SIGNAL(clicked()),this,SLOT(slotSaveAll()));

    connect(buttonHandMoveMap,      SIGNAL(clicked(bool)),this,SLOT(slotButtonHandMoveMap(bool)));
    connect(buttonRuler,            SIGNAL(clicked(bool)),this,SLOT(slotButtonRuler(bool)));
    connect(buttonCentering,        SIGNAL(clicked(bool)),this,SLOT(slotButtonCentering()));
    connect(buttonCursor,           SIGNAL(clicked(bool)),this,SLOT(slotButtonCursor(bool)));
    connect(buttonRouteDynObject,   SIGNAL(clicked(bool)),this,SLOT(slotButtonRouteObject(bool)));
    connect(buttonAllInfoObject,    SIGNAL(clicked(bool)),this,SLOT(slotButtonAllInfoObject(bool)));
    //! получение обратного сигнала
    connect(engine,                 SIGNAL(reciveEventsRequest(TRequestEvent)),this,SLOT(slotIds(TRequestEvent)));

    fileName="";

    dialog=new QFileDialog(this);
    dialog->setNameFilter(tr("XML-variants(*.xml)"));
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setDirectory(settingVV->dirSaveVar);
    mdiArea=new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setCentralWidget(mdiArea);
    connect(mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(slotMdiSubWindowIsActivated(QMdiSubWindow*)));

    formManualModify->setLastNameFile(parser->readLastNameFile("./xml/LastSend.xml"));
    bool isOpen=openFile(parser->readLastNameFile("./xml/LastSend.xml"));
    if(isOpen==false)
    {
        openFile("./xml/LastSend.xml");
    }
    slotButtonHandMoveMap(buttonHandMoveMap->isChecked());
    slotButtonCentering();
    connect(formManualModify,SIGNAL(signalAttach()),this,SLOT(slotAttach()));
}
void MainWindowVisVar::slotCheckBoxGeography(int value)
{
    useMap=value;
    if(useMap==true)
    {
        //! поменять точку привязки
        //setGeoParamOfAircraft();
    }
}

void MainWindowVisVar::slotButtonAllInfoObject(bool value)
{
    if(currentScenes!=0)
        currentScenes->setAllInfo(value);
}
void MainWindowVisVar::slotButtonCentering()
{
    if(currentScenes!=0)
        currentScenes->centerByAircarft();
}
//! копирование сцены
void MainWindowVisVar::slotCloningScene(cl_Scene* s)
{
    cl_Scene *scene=new cl_Scene(s,this);

    formManualModify->resetModelData();
    scene->setLabelObjects(&(formAddLabel->labelObjects));

    scenes.push_back(scene);
    scene->view->installEventFilter(new cl_MouseFilterVariant(scene->view,this));
    QString strName=tr("Вариант #")+QString::number(scenes.size());

    if(s->circleVariant==true) strName+=tr(" - движение по кругам");
    else strName+=tr(" - начальные условия");
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

    slotButtonHandMoveMap(buttonHandMoveMap->isChecked());
}
void MainWindowVisVar::slotRestart()
{
    if(currentScenes!=0)
        currentScenes->slotFlushState();
}

void MainWindowVisVar::slotMdiSubWindowIsActivated(QMdiSubWindow *window)
{
    if(window==0) return ;

    GView* view=static_cast<GView* >(window->widget());

    if(view==0)
        return;
    for(int i=0;i<scenes.size();i++)
    {
         if((scenes[i]->view)==view)
         {
             if(currentScenes!=0)
                 currentScenes->delLabelMap();

             currentScenes=findScene(view);
             currentScenes->showLabelMap();
             setMapAndLayerContextMenu();

             formProjH->setCurrentScene(currentScenes);

             if(currentScenes->firstConnectingSlots==true)
             {
                 disconnect(currentScenes,SIGNAL(signalChangeZoom(int)),formZoomPanel,SLOT(setBeginValue(int)));
                 disconnect(formAddLabel, SIGNAL(deleteLabel(LabelObject*)),currentScenes,SLOT(deleteLabelMap(LabelObject*)));
                 disconnect(currentScenes,SIGNAL(signalUpdateValueObj(int)),formProjH,SLOT(updateScene()));

                 disconnect(engineVariant,SIGNAL(sigAircraftLat(double)),currentScenes,SLOT(slotAircraftLat(double)));
                 disconnect(engineVariant,SIGNAL(sigAircraftLon(double)),currentScenes,SLOT(slotAircraftLon(double)));
                 disconnect(engineVariant,SIGNAL(sigAircraftPsi(double)),currentScenes,SLOT(slotAircraftPsi(double)));

                 disconnect(engineVariant,SIGNAL(sigTargetPsi(int,double)),currentScenes,SLOT(slotTargetPsi(int,double)));
                 disconnect(engineVariant,SIGNAL(sigTargetLat(int,double)),currentScenes,SLOT(slotTargetLat(int,double)));
                 disconnect(engineVariant,SIGNAL(sigTargetLon(int,double)),currentScenes,SLOT(slotTargetLon(int,double)));
                 //! передача текущего времени
                 connect(engineVariant,SIGNAL(sigTime(double)),currentScenes,SLOT(slotTime(double)));
             }else
                 currentScenes->firstConnectingSlots=false;

             connect(currentScenes,SIGNAL(signalChangeZoom(int)),formZoomPanel,SLOT(setBeginValue(int)));
             connect(currentScenes,SIGNAL(signalUpdateValueObj()),formProjH,SLOT(updateScene()));
             connect(formAddLabel, SIGNAL(deleteLabel(LabelObject*)),currentScenes,SLOT(deleteLabelMap(LabelObject*)));

             connect(engineVariant,SIGNAL(sigAircraftLat(double)),currentScenes,SLOT(slotAircraftLat(double)));
             connect(engineVariant,SIGNAL(sigAircraftLon(double)),currentScenes,SLOT(slotAircraftLon(double)));
             connect(engineVariant,SIGNAL(sigAircraftPsi(double)),currentScenes,SLOT(slotAircraftPsi(double)));

             connect(engineVariant,SIGNAL(sigTargetPsi(int,double)),currentScenes,SLOT(slotTargetPsi(int,double)));
             connect(engineVariant,SIGNAL(sigTargetLat(int,double)),currentScenes,SLOT(slotTargetLat(int,double)));
             connect(engineVariant,SIGNAL(sigTargetLon(int,double)),currentScenes,SLOT(slotTargetLon(int,double)));
             //! передача текущего времени
             connect(engineVariant,SIGNAL(sigTime(double)),currentScenes,SLOT(slotTime(double)));

             formZoomPanel->setBeginValue(currentScenes->currentZoom);
         }
    }
}

void MainWindowVisVar::slotButtonSend()
{
    if(scenes.size()!=0)
    {
        slotTransferStarted();

        if(useMap==1)
            setGeoParamOfAircraft();

        byteArray.clear();
        //! cохраним последнее отправленное имя файла
        formManualModify->setLastNameFile(fileName);
        parser->saveVariants("./xml/LastSend.xml",  formManualModify->comment(),useMap,id,fileName);

        //!
//        if(settingVV->noFTP==0)
//        {
//            parser->createXMLForModel(&byteArray,   formManualModify->comment(),useMap,id);
//            //varExchange->send_file(byteArray);
//        }else
//        {
            //! отправление единым куском памяти
            TCommonRequest listReq;
            listReq.setReciver("MPPM");
            listReq.setSender("VAR");
            QString prefixVar       ="InitialState.Init.";
            //QString prefixVarCircle ="VARC.Setup.";
            //QString prefix          ="";

            //! кол-во вариантов по кругам
            //int numsVarCircle=0;
            //! кол-во вариантов НУ
            //int numsVar=0;

//            for(int i = 0;i<scenes.size();i++)//кол-во вариантов
//            {
//                if(scenes[i]->use == false)
//                    continue;

//                int num=0;
//                if(scenes[i]->circleVariant==true)
//                {
//                    numsVarCircle++;
//                    prefix = prefixVarCircle;
//                    num    = numsVarCircle;
//                }
//                else
//                {
//                    numsVar++;
//                    prefix = prefixVar;
//                    num    = numsVar;
//                }
//                scenes[i]->getRequest(&listReq,prefix,num);
//            }

            currentScenes->getRequest(&listReq,prefixVar,0);

            //listReq.append(prefixVarCircle+"numberOf_Variant", QString::number(numsVarCircle));
            //listReq.append(prefixVar+      "numberOfVariant",  QString::number(numsVar));

            requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);
//        }

    }
}

void MainWindowVisVar::slotButtonHandMoveMap(bool flag)
{
    if(flag==true)
    {
        buttonRuler->setChecked(false);
        if(currentScenes!=0)
            currentScenes->deleteRoutes();

        buttonCursor->setChecked(false);
    }
    buttonHandMoveMap->setChecked(true);
    if(currentScenes!=0) currentScenes->activeAddLabel=false;

    for(int i=0;i<scenes.size();i++)
    {
        scenes[i]->setScrollViewHand(flag);
    }
}
void MainWindowVisVar::slotButtonCursor(bool flag)
{
    if(flag==true)
    {
        slotButtonHandMoveMap(false);
        buttonRuler->setChecked(false);

        if(currentScenes!=0)
            currentScenes->deleteRoutes();

        buttonHandMoveMap->setChecked(false);
    }
    buttonCursor->setChecked(true);
}
void MainWindowVisVar::slotButtonRuler(bool flag)
{
    if(flag==true)
    {
        slotButtonHandMoveMap(false);

        buttonCursor->setChecked(false);
        buttonHandMoveMap->setChecked(false);
        buttonRuler->setChecked(true);

    }else
    {
         if(currentScenes!=0)
             currentScenes->deleteRoutes();

         slotButtonHandMoveMap(true);
    }
    if(currentScenes!=0)
        currentScenes->setActiveRoute(flag);
}
void MainWindowVisVar::checkTypeMapAndLayer(cl_Scene *scene)
{
    if(scene!=0)
    {
        QString mapStr="";
        if(ui->actionLandGoogle->isChecked()==true)
        {
            mapStr=tr("Google(Ладншафт)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_LAND);
        }
        if(ui->actionMapsGoogle->isChecked()==true)
        {
            mapStr=tr("Google(Карта)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_MAP);
        }
        if(ui->actionSatGoogle->isChecked()==true)
        {
            mapStr=tr("Google(Спутник)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_SAT);
        }
        if(ui->actionSatYandex->isChecked()==true)
        {
            mapStr=tr("Яндекс(Спутник)");
            scene->setTypeMAP(GeographySysCoord::YANDEX_SAT);
        }
        if(ui->actionSatBing->isChecked()==true)
        {
            mapStr=tr("Bing(Спутник)");
            scene->setTypeMAP(GeographySysCoord::BING_SAT);
        }
        if(ui->actionSatGoogleEa->isChecked()==true)
        {
            mapStr=tr("Google Earth(Спутник)");
            scene->setTypeMAP(GeographySysCoord::GOOGLE_SAT_EARTH);
        }
        if(ui->actionNokiaSatHyb->isChecked()==true)
        {
            mapStr=tr("Nokia(Спутник+гибрид)");
            scene->setTypeMAP(GeographySysCoord::NOKIA_HYB);
        }
        if(ui->actionNokiaMaps->isChecked()==true)
        {
            mapStr=tr("Nokia(Карты)");
            scene->setTypeMAP(GeographySysCoord::NOKIA_MAP);
        }

        QString layerStr=0;
        if(ui->actionHybYandex->isChecked()==true)
        {
            layerStr=tr("гибрид(Яндекс)");
            scene->setTypeLayer(GeographySysCoord::YANDEX_HYB);
        }else if(ui->actionHybGoogle->isChecked()==true)
        {
            layerStr=tr("гибрид(Google)");
            scene->setTypeLayer(GeographySysCoord::GOOGLE_HYB);
        }else
        {
            layerStr=tr("");
            scene->setTypeLayer(GeographySysCoord::NO_LAYER);
        }

        if(layerStr.isEmpty()==true)
            statusBar->setSource(mapStr);
        else
            statusBar->setSource(mapStr+"+"+layerStr);

        scene->refreshZoomLevel();
    }
}

void MainWindowVisVar::checkTypeMapAndLayer()
{
    for(int i=0;i<scenes.size();i++)
    {

        checkTypeMapAndLayer(scenes[i]);
    }
}
void MainWindowVisVar::slotMapGoogle(QAction* act)
{
    if(act->isChecked()==true)
    {
        if(act==ui->actionSatGoogle)
        {
            ui->actionLandGoogle->setChecked(false);
            ui->actionMapsGoogle->setChecked(false);
        }else if(act==ui->actionLandGoogle)
        {
            ui->actionSatGoogle->setChecked(false);
            ui->actionMapsGoogle->setChecked(false);
        }else if(act==ui->actionMapsGoogle)
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
    if(act->isChecked()==true)
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
    if(act->isChecked()==true)
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
    if(act->isChecked()==true)
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
    if(act->isChecked()==true)
    {
        if(act==ui->actionNokiaMaps)
        {
            ui->actionNokiaSatHyb->setChecked(false);
        }else if(act==ui->actionNokiaSatHyb)
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
    if(act->isChecked()==true)
    {
        ui->actionHybYandex->setChecked(false);
    }
    checkTypeMapAndLayer();
}
void MainWindowVisVar::slotLayerYandex(QAction* act)
{
    if(act->isChecked()==true)
    {
        ui->actionHybGoogle->setChecked(false);
    }
    checkTypeMapAndLayer();
}

void MainWindowVisVar::slotExport()
{
    if(scenes.size()!=0)
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
    dialog->show();

    QStringList fileNames=dialog->selectedFiles();
    for(int i=0;i<fileNames.size();i++)
    {
        bool f=fileNames[i].contains(".xml");
        if(f==false)
        {
            if(fileNames[0]=="") fileName="";
            else fileNames[0]+=".xml";
        }else fileName=fileNames[0];

    }

    if(fileNames.size()!=0 && fileName!="")
    {

        setWindowTitle(tr("Редактор вариантов")+" ["+fileName+"]");
        parser->saveVariants(fileName,formManualModify->comment(),useMap,id);

    }
}

void MainWindowVisVar::slotSaveAll()
{
    if(fileName!="")  parser->saveVariants(fileName,formManualModify->comment(),useMap,id);
    else slotSaveAs();
}
void MainWindowVisVar::slotAddLabel()
{
    if(currentScenes!=0)
    {
        slotButtonCursor(true);
        currentScenes->activeAddLabel=true;
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
    if(currentScenes!=0)
    {
        currentScenes->setCenterWindowView(lat,lon,currentScenes->currentZoom);
    }
}
void MainWindowVisVar::slotRunFormAddLabel(double lat,double lon)
{
    formAddLabel->addNewLabel(lat,lon);
    formAddLabel->show();

    if(currentScenes!=0)
        currentScenes->calcItemPosScene();
}
//! создание варианта(обычный или по кругам)
void MainWindowVisVar::createVariant(bool circleVariant)
{
    if(scenes.isEmpty()==true)
    {
        fileName="";
        id=qrand();
        this->setWindowTitle(tr("Редактор вариантов"));
    }
    cl_Scene *scene=new cl_Scene(statusBar,typeObjectsVis,&infoObjects,circleVariant,settingVV,this);
    //!
    formManualModify->resetModelData();
    //!
    scene->setLabelObjects(&(formAddLabel->labelObjects));
    //scene->view->setWindowFlags(Qt::WindowMinMaxButtonsHint);

    int index_prev = 1;
    scene->numberNameVariant=index_prev;
    if(scenes.size()!=0)
    {
        index_prev=(scenes.back())->numberNameVariant;
        scene->numberNameVariant=index_prev+1;
    }
    scenes.push_back(scene);
    scene->view->installEventFilter(new cl_MouseFilterVariant(scene->view,this));
    QString strName=tr("Вариант #")+QString::number(scene->numberNameVariant);

    if(circleVariant==true)
        strName+=tr(" - движение по кругам");
    else
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

    if(scene->circleVariant==true)
    {
        QPointF posScene(0.0,0.0);
        scene->createNewAirTarget(posScene);
    }
    slotButtonHandMoveMap(buttonHandMoveMap->isChecked());
}
void MainWindowVisVar::slotCreateMenu()
{
    createVariant(false);
}
void MainWindowVisVar::slotCreateMenuCircleVariant()
{
    createVariant(true);
}
void MainWindowVisVar::setCurrentActiveWindow(QString name)
{
    QList<QMdiSubWindow*> list=mdiArea->subWindowList();

    for(int i=0;i<list.size();i++)
    {
        GView* view=(GView*) list[i]->widget();
        if(view->windowTitle()==name)
        {
            mdiArea->setActiveSubWindow(list[i]);
            //list[i]->setFocus();//showNormal();
        }
    }
}
void MainWindowVisVar::slotMenuWindow(QAction * act)
{
    QList<QMdiSubWindow*> list=mdiArea->subWindowList();

    for(int i=0;i<actList.size();i++)
    {
        if(actList[i]!=act)
            actList[i]->setChecked(false);
        else
            actList[i]->setChecked(true);
    }
    ui->menuWindow->update();

    for(int i=0;i<list.size();i++)
    {
        GView* view=(GView*) list[i]->widget();
        if(view->windowTitle()==act->text())
        {
            mdiArea->setActiveSubWindow(list[i]);
            list[i]->showNormal();
        }
    }
}
void MainWindowVisVar::slotZoomLevel(int zoom)
{
    QMdiSubWindow* subWindow=mdiArea->currentSubWindow();
    if(subWindow==0) return;

    GView* view=(GView*) subWindow->widget();
    cl_Scene *scene=findScene(view);
    if(scene!=0)
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

    QList<QMdiSubWindow*> list      =mdiArea->subWindowList();
    QMdiSubWindow* currentSubWindow =mdiArea->currentSubWindow();

    for(int i=0;i<list.size();i++)
    {
        QMdiSubWindow* subWindow=list[i];

        GView* view=(GView*) subWindow->widget();
        QAction* act=ui->menuWindow->addAction(view->windowTitle());
        actList.push_back(act);
        act->setCheckable(true);

        if(subWindow==currentSubWindow) act->setChecked(true);
        else act->setChecked(false);
    }
}

void MainWindowVisVar::rightButtonMouseClicked()
{
    if(currentScenes->activeRoute==true || currentScenes->activeAddLabel==true)
        return;

    QAction *act=0;
    menu->clear();
    menu->addAction(tr("Добавить воздушную цель"));
    menu->addAction(tr("Добавить наземную цель"));
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
    if(subWindow==0) return;

    GView* view=(GView*) subWindow->widget();

    cl_Scene *scene=findScene(view);

    if(scene==0) return;

    QPointF posScene=view->mapToScene(view->mapFromGlobal(posMouseMenu));

    if(act->text()==tr("Добавить воздушную цель"))
    {
        if(scene->circleVariant==false)
            scene->createNewAirTarget(posScene);
    }else if(act->text()==tr("Добавить наземную цель"))
    {
        if(scene->circleVariant==false)
            scene->createNewGroundTarget(posScene);
    }else if(act->text()==tr("Добавить аэродром"))
    {
        scene->createNewAerodrom(posScene);
    }else if(act->text()==tr("Добавить информационную точку"))
    {
        scene->createNewInfoObject(posScene);
        for(int i=0;i<scenes.size();i++)
        {
            scenes[i]->refreshInfo();
        }
    }else if(act->text()==tr("Добавить маяк РСБН"))
        scene->createNewBeaconObject(posScene);

    else if(act->text()==tr("Добавить маяк VOR"))
        scene->createNewBeaconObject(posScene);

    formManualModify->resetModelData();
}

void MainWindowVisVar::slotExitProgram()
{
    qApp->exit();
}
void MainWindowVisVar::showAllVariant()
{
    for(int i=0;i<scenes.size();i++)
    {
        scenes[i]->view->installEventFilter(new cl_MouseFilterVariant(scenes[i]->view,this));
        scenes[i]->view->setWindowTitle(scenes[i]->returnNameVariant());
        scenes[i]->setLabelObjects(&(formAddLabel->labelObjects));
        scenes[i]->setSubWindowMDI(mdiArea->addSubWindow(scenes[i]->view));
        scenes[i]->view->show();
    }
    createWindowMenu();
    setMapAndLayerContextMenu();
}
void MainWindowVisVar::setMapAndLayerContextMenu()
{
    ui->actionSatGoogle->setChecked(false);
    ui->actionMapsGoogle->setChecked(false);
    ui->actionLandGoogle->setChecked(false);
    ui->actionSatBing->setChecked(false);
    ui->actionSatGoogleEa->setChecked(false);
    ui->actionNokiaMaps->setChecked(false);
    ui->actionNokiaSatHyb->setChecked(false);

    ui->actionHybYandex->setChecked(false);
    ui->actionHybGoogle->setChecked(false);

    if(currentScenes!=0)
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
    this->setWindowTitle(tr("Редактор вариантов"));
    currentScenes=0;
    QList<QMdiSubWindow*> list=mdiArea->subWindowList();
    for(int i=0;i<list.size();i++)
    {
        list[i]->close();
    }

    for(int i=0;i<scenes.size();i++)
    {
        scenes[i]->view->close();
        cl_Scene *tempScene=scenes[i];
        delete tempScene;
    }
    scenes.clear();
}
//! обработка событий
bool MainWindowVisVar::event(QEvent *event)
{
    return QMainWindow::event(event);
}
//! открыть файл с вариантами
void MainWindowVisVar::slotOpenXML()
{
    //! здесь нужна проверка того были ли созданы уже варинаты, если да, то нужно предложить сохранить перед загрузкой
    if(scenes.size()!=0)
    {
        dialogSave->exec();
        if(dialogSave->ok==true)
        {
            slotSaveAs();
        }
    }

    closeAllVariant();

    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilter(tr("XML-variants(*.xml)"));

    dialog->selectFile("");
    dialog->exec();

    QStringList fileNames=dialog->selectedFiles();
    if(fileNames.size()!=0)
    {
        if(fileNames[0]!="") fileName=fileNames[0];
        this->setWindowTitle(tr("Редактор вариантов")+" ["+fileName+"]");

        if(fileNames.size()!=0)
        {
            openFile(fileNames[0]);
        }
    }
}
//! открыть файл с вариантами
bool MainWindowVisVar::openFile(QString fileName)
{
    bool openFile=parser->openFileVariants(fileName,
                                           &comment,
                                           useMap,id);
    //! использование цифровой карты
    checkBoxGeography->setChecked(useMap);
    //! комментарий
    formManualModify->setComment(comment);

    if(openFile==true)
    {
        //! показать все варианты
        showAllVariant();
        //! читаем параметры географии
        //readSetupGeography();
        //! пересчет положения объектов, если нет привязки к цифровой карте
        //reCalcVar();
        //! сбросить модельные данные
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
    QString time=QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
    if(code==0)
        statusBar->setStatusTransmit(tr("Успешно: дата ") +  time,
                                     FormStatusBar::ST_GOOD);
    else
        statusBar->setStatusTransmit(/*tr("Ошибка: ")+*/errString,
                                     FormStatusBar::ST_BAD);
}
cl_Scene* MainWindowVisVar::findScene(GView *view)
{
    for(int i=0;i<scenes.size();i++)
    {
        if(scenes[i]->view==view) return scenes[i];
    }
    return 0;
}
//! запись в структуру "Установка географии"
void MainWindowVisVar::setGeoParamOfAircraft()
{
    if(scenes.size()!=0)
    {

//        TCommonRequest listReq;
//                listReq.setReciver("MPPM");
//                listReq.setSender("VAR");
//                QString prefixSetupGeo       ="InitialState.Init.";

//                listReq.append(prefixSetupGeo + "Psi",     QString::number(0.0));
//                listReq.append(prefixSetupGeo + "Lat",    QString::number(currentScenes->aircraft->lat));
//                listReq.append(prefixSetupGeo + "Lon", QString::number(currentScenes->aircraft->lon));
//                listReq.append(prefixSetupGeo + "Vist",               QString::number(0.0));
//                listReq.append(prefixSetupGeo + "Y",               QString::number(0.0));

//        TCommonRequest listReq;
//        listReq.setReciver("MPPM");
//        listReq.setSender("VAR");
//        QString prefixSetupGeo       ="InitGEO.Setup.";

//        listReq.append(prefixSetupGeo + "Aust_0_geography",     QString::number(0.0));
//        listReq.append(prefixSetupGeo + "fiust_0_geography",    QString::number(currentScenes->aircraft->lat));
//        listReq.append(prefixSetupGeo + "limdaust_0_geography", QString::number(currentScenes->aircraft->lon));
//        listReq.append(prefixSetupGeo + "x0_ust",               QString::number(0.0));
//        listReq.append(prefixSetupGeo + "z0_ust",               QString::number(0.0));


        //requestEvent=engine->setValue(listReq,IEngineData::ASYNCH_ENGINE);

        /*engine->setValue("MPPM.Au_geography",           "0.0",IEngineData::ASYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.fiust_geography",        QString::number(currentScenes->aircraft->lat),IEngineData::ASYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.limdaust_geography",     QString::number(currentScenes->aircraft->lon),IEngineData::ASYNCH_ENGINE);*/

        /*engine->setValue("MPPM.InitGEO.Setup.Aust_0_geography",       "0.0",IEngineData::SYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.fiust_0_geography",      QString::number(currentScenes->aircraft->lat),IEngineData::SYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.limdaust_0_geography",   QString::number(currentScenes->aircraft->lon),IEngineData::SYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.x0_ust",                 "0.0",  IEngineData::SYNCH_ENGINE);
        engine->setValue("MPPM.InitGEO.Setup.z0_ust",                 "0.0",  IEngineData::ASYNCH_ENGINE);*/
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
    bool isOpen=parser->openFileVariants(nameFile,&comment,useMap,id);
    if(isOpen==true)
    {
        showAllVariant();
        formManualModify->resetModelData();
        slotButtonSend();
    }
}
void MainWindowVisVar::slotButtonRouteObject(bool value)
{
    for(int i=0;i<scenes.size();i++)
    {
        scenes[i]->slotUseMoveObj(value);
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
void MainWindowVisVar::slotAttach()
{
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilter(tr("XML-metadata(*.xml)"));

    dialog->selectFile("");
    dialog->exec();

    QStringList fileNames=dialog->selectedFiles();
    if(fileNames.size()!=0)
    {
        if(fileNames.size()!=0)
        {
            parser->openFileMetaData(fileNames[0]);
        }
    }
}
//! пересчет положения объектов, если нет привязки к цифровой карте
void MainWindowVisVar::reCalcVar()
{
    qDebug("start reCalcVar\n");
    if(useMap==false)//! если не используется цифровая карта
    {
        for(int i=0;i<scenes.size();i++)//! для всех сцен расчитываем новое положение объектов
        {
            qDebug("reCalcVar_lat=%f\n",lat_model.toDouble());
            qDebug("reCalcVar_lon=%f\n",lon_model.toDouble());
            qDebug("reCalcVar_psi=%f\n",psi_model.toDouble());
            //! персчет объектов относительно модельной СК
            scenes[i]->reCalcObject(lat_model.toDouble(),
                                    lon_model.toDouble(),
                                    psi_model.toDouble());
        }
    }
    qDebug("end reCalcVar\n");
}
}
