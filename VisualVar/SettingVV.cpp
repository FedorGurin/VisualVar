#include "SettingVV.h"
#include "../globalFunc/gl_func.h"



SettingVV::SettingVV(QObject *parent):QObject(parent)
{
    QString temp;

    fileName=   tr("BegData.xml");
    hostAddress=tr("192.168.58.5");
    userName=   tr("anonymous");
    password=   tr("no@mail.net");
    pathCD=     tr("/ADAPTERS");
    FTPWait=20000;
    //noFTP=1;

    startLat = 55.567331494459;
    startLon = 38.121120929718;

    typeMap   = "nokia_hyb";
    typeLayer = "";

    zoom=12;
    moveObjects=0;

    temp=readParamFromXMLFile(nameMainFile,"VV","FileName");
    if(temp.isEmpty() == false) fileName=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","HostAddress");
    if(temp.isEmpty() == false) hostAddress=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","UserName");
    if(temp.isEmpty() == false) userName=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","Password");
    if(temp.isEmpty() == false) password=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","PathCD");
    if(temp.isEmpty() == false) pathCD=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","DirVariant");
    if(temp.isEmpty() == false) dirSaveVar=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","DirWithMap");
    if(temp.isEmpty() == false) pathWithMap=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","FTPWait");
    if(temp.isEmpty() == false) FTPWait=temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","StartLat");
    if(temp.isEmpty() == false) startLat=temp.toDouble();

    temp=readParamFromXMLFile(nameMainFile,"VV","StartLon");
    if(temp.isEmpty() == false) startLon=temp.toDouble();

    temp=readParamFromXMLFile(nameMainFile,"VV","Zoom");
    if(temp.isEmpty() == false) zoom = temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","MoveObj");
    if(temp.isEmpty() == false) moveObjects = temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","TypeMap");
    if(temp.isEmpty() == false) typeMap  = temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","TypeLayer");
    if(temp.isEmpty() == false) typeLayer = temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","PathToObj");
    if(temp.isEmpty() == false) pathToObj = temp;

//    temp=readParamFromXMLFile(nameMainFile,"VV","NoFTP");
//    if(temp.size()!=0) noFTP=temp.toInt();
}
