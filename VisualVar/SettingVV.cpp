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

    startLat=55.3666;
    startLon=38.2833;

    typeMap="nokia_hyb";
    typeLayer="";

    zoom=12;
    moveObjects=0;

    temp=readParamFromXMLFile(nameMainFile,"VV","FileName");
    if(temp.size()!=0) fileName=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","HostAddress");
    if(temp.size()!=0) hostAddress=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","UserName");
    if(temp.size()!=0) userName=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","Password");
    if(temp.size()!=0) password=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","PathCD");
    if(temp.size()!=0) pathCD=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","DirVariant");
    if(temp.size()!=0) dirSaveVar=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","DirWithMap");
    if(temp.size()!=0) pathWithMap=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","FTPWait");
    if(temp.size()!=0) FTPWait=temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","StartLat");
    if(temp.size()!=0) startLat=temp.toDouble();

    temp=readParamFromXMLFile(nameMainFile,"VV","StartLon");
    if(temp.size()!=0) startLon=temp.toDouble();

    temp=readParamFromXMLFile(nameMainFile,"VV","Zoom");
    if(temp.size()!=0) zoom=temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","MoveObj");
    if(temp.size()!=0) moveObjects=temp.toInt();

    temp=readParamFromXMLFile(nameMainFile,"VV","TypeMap");
    if(temp.size()!=0) typeMap=temp;

    temp=readParamFromXMLFile(nameMainFile,"VV","TypeLayer");
    if(temp.size()!=0) typeLayer=temp;

//    temp=readParamFromXMLFile(nameMainFile,"VV","NoFTP");
//    if(temp.size()!=0) noFTP=temp.toInt();
}
