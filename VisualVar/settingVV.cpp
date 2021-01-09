#include "settingVV.h"
#include "../globalFunc/gl_func.h"

SettingVV::SettingVV(QObject *parent):QObject(parent)
{
    QString temp;

    startLat = 55.567331494459;
    startLon = 38.121120929718;

    typeMap   = "nokia_hyb";
    typeLayer = "";

    zoom = 12;
    moveObjects = 0;

    temp = readParamFromXMLFile(nameMainFile,"VV","DirVariant");
    if(!temp.isEmpty()) dirSaveVar = temp;

    temp = readParamFromXMLFile(nameMainFile,"VV","DirWithMap");
    if(!temp.isEmpty()) pathWithMap = temp;

    temp = readParamFromXMLFile(nameMainFile,"VV","StartLat");
    if(!temp.isEmpty()) startLat = temp.toDouble();

    temp = readParamFromXMLFile(nameMainFile,"VV","StartLon");
    if(!temp.isEmpty()) startLon = temp.toDouble();

    temp = readParamFromXMLFile(nameMainFile,"VV","Zoom");
    if(!temp.isEmpty()) zoom = temp.toInt();

    temp = readParamFromXMLFile(nameMainFile,"VV","MoveObj");
    if(!temp.isEmpty()) moveObjects = temp.toInt();

    temp = readParamFromXMLFile(nameMainFile,"VV","TypeMap");
    if(!temp.isEmpty()) typeMap  = temp;

    temp = readParamFromXMLFile(nameMainFile,"VV","TypeLayer");
    if(!temp.isEmpty()) typeLayer = temp;

    temp = readParamFromXMLFile(nameMainFile,"VV","PathToObj");
    if(!temp.isEmpty()) pathToObj = temp;
}
