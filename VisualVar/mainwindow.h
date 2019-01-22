#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QFileDialog>
#include <QVector>
#include <QGraphicsView>
#include <QToolButton>
#include <QMenu>
#include <QLineEdit>
#include <QCheckBox>

#include "Parser.h"
#include "formzoompanel.h"

#include "TypeObjectsVis.h"
#include "dialogsavecurvar.h"
#include "./manualModify/FormManualModify.h"
#include "../CommonEngineData/CommonEngineData.h"
#include "VariantEngineData.h"
#include "SettingVV.h"
#include "../globalFunc/UnitsMeasure/ConvertUnitsMeasure.h"
#include "formaddlabelmap.h"
#include "formprojecth.h"


namespace Ui {
    class MainWindowVariant;
}
namespace VisualVariant
{
class cl_MouseFilterVariant:public QObject
{
    Q_OBJECT
protected:
    virtual bool eventFilter(QObject*,QEvent*);
public:
    cl_MouseFilterVariant(QObject* pobj=0,QWidget *slotForm_=0);
    QWidget *slotForm;
};

class MainWindowVisVar : public QMainWindow {
    Q_OBJECT
public:
    MainWindowVisVar(QWidget *parent = 0);
    ~MainWindowVisVar();
    //! область для многодокументого окна
    QMdiArea *mdiArea;
    //!
    QMenu *menu;
    //! найти сцену по окну
    cl_Scene* findScene(GView *);
    //! текущее выбранное окно
    void setCurrentActiveWindow(QGraphicsView*);
    cl_MouseFilterVariant *mouseFilter;
    QPoint posMouseMenu;
    QVector<QAction* > actList;
    //! оздать менб при нажатии на правую кнопку мыши
    void createWindowMenu();
    //! проверить тип карты и слои(гибриды)
    void checkTypeMapAndLayer();
    void checkTypeMapAndLayer(cl_Scene *scene);
    //! форма с проекцией высоты
    FormProjectH *formProjH;
    //! ручной ввод данных
    FormManualModify *formManualModify;
    //! строка состояния
    FormStatusBar *statusBar;
    //! панель увеличения/уменьшения
    FormZoomPanel *formZoomPanel;
    //! движок для вариантов
    VariantEngineData *engineVariant;
    //! указатель на движок с данными
    IEngineData *engine;
    //! настройки
    SettingVV *settingVV;
    //! идентификатор загруженного варианта
    uint id;
    //! открыть все варианты
    void showAllVariant();
    //! установить меню какие в данный момент используются слои и карты
    void setMapAndLayerContextMenu();
    //! открыть файл
    bool openFile(QString);
    //! признак того, что нужно использовать координаты из карты
    bool useMap;
    //! варианты
    QVector<cl_Scene* > scenes;
    //! пересчет положения объектов, если нет привязки к цифровой карте
    void reCalcVar();
public slots:
    //! слот при сигнале "Исходное"
    void slotRestart();
    //! слот получение времени из модели
    //void slotTime(double){}
    //! клонирование сцены
    void slotCloningScene(cl_Scene* s);
    //! открыть файл с метаданными
    void slotAttach();
    //! задать географию земли в случае наличия привязки к карте
    void setGeoParamOfAircraft();
    //! чтение параметров структуры географии
    void readSetupGeography();
    //! закрыть текущие варианты(данные будут потеряны)
    void closeAllVariant();
    //! реакция на выбор пункта экспорт
    void slotExport();
    //! создание меню
    void slotCreateMenu();
    void slotCreateMenuCircleVariant();
    void slotOpenXML();
    void slotExitProgram();
    void slotSaveAs();
    void slotSaveAll();
    //! добавить новую метку
    void slotAddLabel();
    //! переход на метку
    void slotGotoLabel();
    void slotGotoLatLon(double lat,double lon);
    //! создание варианта(либо стандартного/полет по кругам)
    void createVariant(bool circleVariant);
    //! сохранить данные в XML
    void slotSaveXML();
    //! действия по нажатию правой кнопки на карте
    void slotRunMenuScene(QAction*);
    //! по нажатию правой кнопки мыши
    void rightButtonMouseClicked();
    //! слот на обработку subWindow
    void slotMenuWindow(QAction * act);
    //! слот на изменение масштаба карты
    void slotZoomLevel(int);
    //! слот на нажатие на кнопку buttonSend;
    void slotButtonSend();
    //! отоброжать ли текущее положение объектов на карте
    void slotButtonRouteObject(bool);
    //! отображение всей информации об объектах
    void slotButtonAllInfoObject(bool);
    //! слот на отправку файла
    void slotButtonSendFile();
    //! перемешение карты рукой
    void slotButtonHandMoveMap(bool);
    //! слот при выборе линейки
    void slotButtonRuler(bool);
    //! центрирование
    void slotButtonCentering();
    //! выбор курсора
    void slotButtonCursor(bool);
    //! слот обработки идентфикаторов
    void slotIds(TRequestEvent eventRequest);
    //! задать текущим окно с именем <name>
    void setCurrentActiveWindow(QString name);
    //! слот на сигнал от FTP класса
    void slotTransferStarted();
    //! слот обмен закончен
    void slotTransferFinished(int, QString);
    //!
    void slotMdiSubWindowIsActivated(QMdiSubWindow *window);
    //! реакция на запрос движка открыть файл и загрузить его в целевую машину
    void slotOpenAndSendVariant(QString);
    //! реакция на установку флажка "брать координаты из карты"
    void slotCheckBoxGeography(int);
    //! окно для добавления метки
    void slotRunFormAddLabel(double lat,double lon);
    //! слоты для обработки меню с выбором карт
    void slotMapGoogle(QAction*);
    void slotMapNokia(QAction*);
    void slotMapYandex(QAction*);
    void slotMapBing(QAction*);
    //!
    void slotMapGoogle_EA(QAction*);
    void slotLayerYandex(QAction*);
    void slotLayerGoogle(QAction*);

    void slotEventsRequest(TRequestEvent);
protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);
private:
    TRequestEvent requestEvent;
    TRequestEvent reqEventCurPos;
    //! параметры структуры
    QString psi_model;
    QString lat_model;
    QString lon_model;
    QString x_model;
    QString z_model;

    //! текущая ссылка на объект scene
    cl_Scene *currentScenes;
    //! преобразование ед. измерения
    //ConvertUnitsMeasure *convertor;
    //! буффер с данными
    QByteArray byteArray;
    //! диалог
    QFileDialog *dialog;


    //! кнопка передачи варианта по FTP
    QToolButton *buttonSend;
    QToolButton *buttonSendFile;
    QToolButton *buttonVar;
    QToolButton *buttonCir;

    //! кнопка включающая отображение текущих координат объектов на карте варианта
    QToolButton *buttonRouteDynObject;
    //! кнопка включающая отображение всей информации об объекте
    QToolButton *buttonAllInfoObject;
    //! кнопка сохранения файл
    QToolButton *buttonSaveAll;
    //! путь в целевой машине при отправке файла
    QLineEdit   *pathFileToSend;
    //! использовать географические координаты из карты
    QCheckBox   *checkBoxGeography;
    //! комментарий
    QString comment;
    //! список идентификаторов для обработки
    uint ids;
    //! меню кнопки
    QToolButton *buttonHandMoveMap;
    QToolButton *buttonRuler;
    QToolButton *buttonCursor;
    QToolButton *buttonCentering;

    //! имя файла если файл открывался
    QString fileName;
    //! класс разбора
    Parser *parser;
    //! типы объектов для системы визуализации
    TypeObjectsVis *typeObjectsVis;
    //! список информационных точек
    QList<InfoObject* > infoObjects;
    //! диалог сохранения вариантов
    DialogSaveCurVar *dialogSave;
    //! указатель на форму
    Ui::MainWindowVariant *ui;
    //! форма добавления новой метки
    FormAddLabelMap *formAddLabel;
signals:
    void signalAddData();
    void sigAircraftLat(double);
    void sigAircraftLon(double);
    void sigAircraftPsi(double);
    void sAircraftPos2D(double,double,double);
    void sigTime(double);

};
}
#endif // MAINWINDOW_H
