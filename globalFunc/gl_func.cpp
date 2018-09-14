#include "gl_func.h"
#include <math.h>
#include <QTextCodec>
#include <QStringList>
#include "../globalFunc/Matrix3D_D.h"
#include <QFile>
#include <QApplication>
#include <QTextStream>

QStringList listForCompleter;

QString nameMainFile;

QString readParamFromXMLFile(QString fileName,QString nameProg,QString nameAttrib)
{
    bool openFile=false;
    QString tempName("");
    QFile file(qApp->applicationDirPath()+"/"+fileName);
    QDomDocument domDoc;
    openFile=file.open(QIODevice::ReadOnly);
    if(openFile==true)
    {
        bool readXML=false;
        readXML=domDoc.setContent(&file,true);
        if(readXML==true)
        {
            QDomElement ele1=domDoc.firstChildElement("data");
            QDomElement ele=ele1.firstChildElement(nameProg);
            QString tempName=ele.attribute(nameAttrib,"");
            return tempName;
        }
    }else qDebug("ReadParamFromXMLFile: File not find=%s\n nameProg=%s\n nameAttrib=%s\n",qPrintable(fileName),qPrintable(nameProg),qPrintable(nameAttrib));
    return tempName;
}
bool writeAttrParamToXMLFile(QString fileName,QString nameProg,QString nameAttrib,QString value)
{
    bool openFile=false;
    //QString tempName("");
    QFile file(qApp->applicationDirPath()+"/"+fileName);
    QDomDocument domDoc;
    openFile=file.open(QIODevice::ReadWrite|QIODevice::Text);
    QTextStream out(&file);
    if(openFile==true)
    {
        bool readXML=false;
        readXML=domDoc.setContent(&file,true);
        if(readXML==true)
        {
            QDomElement ele1=domDoc.firstChildElement("data");
            QDomElement ele=ele1.firstChildElement(nameProg);
            ele.setAttribute("dasda","324");
            //domDoc.appendChild(ele1);
            domDoc.save(out,4);
            //return ele;
        }
    }else qDebug("ReadParamFromXMLFile: File not find=%s\n",qPrintable(fileName));

    return false;
    //return QDomElement();
}

QDomElement paramFromXMLFileWithAttrib(QString fileName,QString nameProg,QString nameAttrib)
{
    bool openFile=false;
    QString tempName("");
    QFile file(qApp->applicationDirPath()+"/"+fileName);
    QDomDocument domDoc;
    openFile=file.open(QIODevice::ReadOnly);
    if(openFile==true)
    {
        bool readXML=false;
        readXML=domDoc.setContent(&file,true);
        if(readXML==true)
        {
            QDomElement ele1=domDoc.firstChildElement("data");
            QDomElement ele=ele1.firstChildElement(nameProg);

            return ele;
        }
    }else qDebug("ReadParamFromXMLFile: File not find=%s\n",qPrintable(fileName));

    return QDomElement();
}
QString TranslateFromCP1251(const QByteArray& ArrayString)
{
    QTextCodec *codec = QTextCodec::codecForName("CP1251");
    QString unicode=codec->toUnicode(ArrayString);
    return unicode;
}

QString TranslateFromKOI8(const QByteArray& ArrayString)
{
    QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
    QString unicode=codec->toUnicode(ArrayString);
    return unicode;
}
/*
Преобразование градусов/минут/секунд в дробное число.
Вызов: <Результат в double> = coord_to_float(<градусы>, <минуты>, <секунды>)
*/
double coord_to_float(int grad, int min, double sec)
{
    double flt;
    if((min < 60.0)&&(sec < 60.0))
    {
        flt = (1.0)*grad + min/60.0 + sec/3600.0;
//        qDebug("grad=%f\n",(1.0)*grad);
//        qDebug("min=%f\n",min/60.0);
//        qDebug("sec=%f\n",sec/3600.0);
    }
    else
        flt = 0;
    return flt;
}
/*
Преобразование дробного числа в градусы/минуты/секунды
Вызов: <Результат в int>[3] = coord_to_float(<Угол дробным числом>)
Результат является ссылкой на массив из трех int (градусы, минуты, секунды)
*/
Tcordd float_to_coord(double flt)
{
    double dgrad, dmin, dsec;
    Tcordd cordd;

    flt = modf(flt, &dgrad);
    flt = flt * 60;
    flt = modf(flt, &dmin);
    dsec = flt * 60;

    cordd.grad = (int)dgrad;
    cordd.min = (int)dmin;
    cordd.sec = dsec;
    return cordd;
}
double GradToRadian(double x)
{
    return (x*M_PI)/180.0;
}
double RadianToGrad(double x)
{
     return (x*180)/M_PI;
}

double PsiToKurs(double psi_)
{
    psi_=-psi_;
    double kurs_=psi_;
    if(psi_<0.0) kurs_=2*M_PI+psi_;
    return kurs_;
}
//! проверка выхода за [180;-180]
double checkPI(double angle/*в рад*/)
{
    if(angle>M_PI) angle+=-2.0*M_PI;
    else if(angle<-M_PI)angle+=2.0*M_PI;
    return angle;
}

double KursToPsi(double kurs)
{
    double psi=kurs;
    if(kurs>M_PI) psi=psi-2*M_PI;
    psi=-psi;

    return psi ;
}
//! проверка выхода за [0;-360]
double check360(double angle/*в град.*/)
{
    if(angle>(2.0*180.0)) angle-=2.0*180.0;
    else if(angle<0.0)angle+=2.0*180.0;
    return angle;
}
void forwardMercator(double lon, double lat, double &x, double &y)
{
    x = lon * 20037508.34 / 180.0;
    y = log(tan((90.0 + lat) * M_PI / 360.0)) / (M_PI / 180.0);
    y = y * 20037508.34 / 180.0;
}
void inverseMercator(double x, double y, double &lon, double &lat)
{
    lon = (x / 20037508.34) * 180;
    lat = (y / 20037508.34) * 180;
    lat = 180.0/M_PI * (2.0 * atan(exp(lat * M_PI / 180.0)) - M_PI / 2.0);
}
//! Преобразование сферических в декартовы
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z)
{

    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////

    Matrix3D_D AgG;

    double cosfi=cos(fi);
    double sinfi=sin(fi);
    double cosLambda=cos(lambda);
    double sinLambda=sin(lambda);

    AgG.x[0][0]=cosfi;
    AgG.x[0][1]=-sinfi*cosLambda;
    AgG.x[0][2]=-sinfi*sinLambda;

    AgG.x[1][0]=sinfi;
    AgG.x[1][1]=cosfi*cosLambda;
    AgG.x[1][2]=cosfi*sinLambda;

    AgG.x[2][0]=0.0;
    AgG.x[2][1]=-sinLambda;
    AgG.x[2][2]=cosLambda;

    Matrix3D_D tempAsG=AsG;
    Matrix3D_D Asg=AgG*tempAsG.transpose();

    double tetta=asin(Asg.x[1][0]);
    double psi=atan2(Asg.x[1][2],Asg.x[1][1]);

    x=6356767.0*tetta; z=6356767.0*psi;

}

//! Преобразование декартовых в сферические
//! положение стартовой СК относительно земной СК
//!                                             -lambda
//!                                             -fi

//! Декартовы координаты объекта относительно стартовой СК
//!                                             -x
//!                                             -z
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi)
{
    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////
    double tetta=x/6356767.0;
    double psi=z/6356767.0;

    Matrix3D_D Atetta;
    double cosTetta=cos(tetta);
    double sinTetta=sin(tetta);

    Atetta.x[0][0]=cosTetta;
    Atetta.x[0][1]=-sinTetta;
    Atetta.x[0][2]=0.0;

    Atetta.x[1][0]=sinTetta;
    Atetta.x[1][1]=cosTetta;
    Atetta.x[1][2]=0.0;

    Atetta.x[2][0]=0.0;
    Atetta.x[2][1]=0.0;
    Atetta.x[2][2]=1.0;


    Matrix3D_D Apsi;

    double cosPsi=cos(psi);
    double sinPsi=sin(psi);

    Apsi.x[0][0]=1.0;
    Apsi.x[0][1]=0.0;
    Apsi.x[0][2]=0.0;

    Apsi.x[1][0]=0.0;
    Apsi.x[1][1]=cosPsi;
    Apsi.x[1][2]=sinPsi;

    Apsi.x[2][0]=0.0;
    Apsi.x[2][1]=-sinPsi;
    Apsi.x[2][2]=cosPsi;

    Matrix3D_D tempMatrix=Atetta*AsG;
    Matrix3D_D AgG=Apsi*tempMatrix;

    fi=asin(AgG.x[1][0]);
    lambda=atan2(AgG.x[1][2],AgG.x[1][1]);

}
double KursToPsiGrad(double kurs)
{
    double psi=kurs;
    if(kurs>180.0) psi=psi-2*180.0;
    return psi ;
}
//! проверка выхода за [180;-180]
double check180(double angle/*в град*/)
{
    if(angle>180.0) angle+=-2.0*180.0;
    else if(angle<-180.0)angle+=2.0*180.0;
    return angle;
}

QString geo_double_to_string(double flt)
{
    double dgrad, dmin, dsec;
    Tcordd cordd;
    QString str;
    flt = modf(flt, &dgrad);
    flt = flt * 60;
    flt = modf(flt, &dmin);
    dsec = flt * 60;


    cordd.grad = (int)dgrad;
    cordd.min = (int)dmin;
    cordd.sec = dsec;

    str=QString::number(cordd.grad)+
        QT_TR_NOOP("°")+
        QString::number(cordd.min)+
        QT_TR_NOOP("'")+
        QString::number(cordd.sec,'g',4)+QT_TR_NOOP("\"");

    return str;
}
double geo_string_to_double(QString param)
{
    int grad=0;
    int min=0;
    double sec=0.0;

    QStringList list=param.split(QRegExp(("[°'\"]")),QString::KeepEmptyParts);

    if(list.size()>=1) grad=list[0].toInt();
    if(list.size()>=2) min=list[1].toInt();
    if(list.size()>=3) sec=list[2].toDouble();

    return coord_to_float(grad,min,sec);
}

///T clip(T n, T minValue, T maxValue)

unsigned mapSize(int levelOfDetail)
{
    return (uint) 256 << levelOfDetail;
}
void latLongToPixelXY(double latitude, double longitude, int levelOfDetail, int &pixelX, int &pixelY)
{
    latitude = clip(latitude, MinLatitude, MaxLatitude);
    longitude = clip(longitude, MinLongitude, MaxLongitude);
    double x = (longitude + 180) / 360;
    double sinLatitude = sin(latitude * M_PI / 180);
    double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * M_PI);
    int map_size = mapSize(levelOfDetail);
    pixelX = (int) clip(x * map_size + 0.5, 0.0, map_size - 1.0);
    pixelY = (int) clip(y * map_size + 0.5, 0.0, map_size - 1.0);
}

void pixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double &latitude, double &longitude)
{
    int map_size = mapSize(levelOfDetail);

    double x = (clip(pixelX, 0, map_size - 1) / map_size) - 0.5;
    double y = 0.5 - (clip(pixelY, 0, map_size-1) / map_size);

//    y=pixelY/256;
//    double n = M_PI - 2.0 * M_PI * y / pow(2.0, levelOfDetail);
//    latitude= 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));

//    n = 2 ^ zoom
//    lon_deg = xtile / n * 360.0 - 180.0
//    lat_rad = arctan(sinh(π * (1 - 2 * ytile / n)))
//    lat_deg = lat_rad * 180.0 / π

//    qDebug("tileX=%f\n",pixelX/256.0);
//    qDebug("tileY=%f\n",y);
    latitude = 90.0 - 360.0 * atan(exp(-y*2* M_PI)) / M_PI;
    longitude = 360.0 * x;
}
void pixelXYToLatLong(QPointF pixel, int levelOfDetail, double &latitude, double &longitude)
{
    int map_size = mapSize(levelOfDetail);
    double x = (clip(pixel.x(), 0.0, map_size - 1.0) / map_size) - 0.5;
    double y = 0.5 - (clip(pixel.y(), 0.0, map_size - 1.0) / map_size);
    latitude = 90.0 - 360.0 * atan(exp(-y * 2 * M_PI)) / M_PI;
    longitude = 360.0 * x;
}

void pixelXYToTileXY(int pixelX, int pixelY, int &tileX, int &tileY)
{
    tileX = pixelX / 256;
    tileY = pixelY / 256;
}
void tileXYToPixelXY(int tileX, int tileY, int &pixelX, int &pixelY)
{
    pixelX = tileX * 256;
    pixelY = tileY * 256;
}
double groundResolution(double latitude, int levelOfDetail)
{
    latitude = clip(latitude, MinLatitude, MaxLatitude);
    return (cos(latitude * M_PI / 180) * 2 * M_PI * EarthRadius)/ mapSize(levelOfDetail);
}
double mapScale(double latitude, int levelOfDetail, int screenDpi)
{
    return groundResolution(latitude, levelOfDetail) * screenDpi / 0.0254;
}
double distanceSphere(double Lon1,double Lat1,double Lon2,double Lat2)
{
    Lon1=GradToRadian(Lon1);
    Lat1=GradToRadian(Lat1);

    Lon2=GradToRadian(Lon2);
    Lat2=GradToRadian(Lat2);

    double R=6356767;

    double lat=(Lat2+Lat1)/2.0;
    double dLat=(Lat2-Lat1)/2.0;
    double dLon=(Lon2-Lon1)/2.0;

    double ws=pow(cos(dLon),2)*pow(sin(dLat),2)+pow(cos(lat),2)*pow(sin(dLon),2);
    double wc=pow(cos(dLon),2)*pow(cos(dLat),2)+pow(sin(lat),2)*pow(sin(dLon),2);

    double sigma;
    if(wc>=ws) sigma=2*asin(sqrt(ws));
    else sigma=2*acos(sqrt(wc));

    return R*sigma;
}
double cutValues(double x)//отрезать дробную чпсть
{
    if(fabs(x)<1e-30) return 0.0;
    return x;
}
//! ограничение сверху
double limitMax(double value,double max)
{
    if(value>max) value=max;
    return  value;
}
//! ограничение снизу
double limitMin(double value,double min)
{
    if(value>min) value=min;
    return  value;
}
//! Апериодический фильтр
FilterAperiodic::FilterAperiodic(double valueStart_,double k_,double step_,double maxValue_, double minValue_)
{
    k=k_;
    step=step_;
    maxValue=maxValue_;
    minValue=minValue_;
    value=valueStart_;
    valueStart=valueStart_;
    valueCurrent=valueStart;
    delta_=fabs(valueCurrent-value);
}
//! сброс фильтра (задать текущее значение фильтра)
void FilterAperiodic::resetFilter(double valueStart_)
{
    value=valueStart_;
}
//! возврат текущего значения фильтра
double FilterAperiodic::filterValue(double valueCurr)
{
    value=value+(k*cutValues(valueCurr-value)*step);
    value = limitMinMax(value,minValue,maxValue);
    return value;
}
void FilterAperiodic::setK(double k_)
{
    k=k_;
}
//! обновление фильтра(с заданной частотой)
void FilterAperiodic::refresh()
{
    delta_=fabs(valueCurrent-value);
    if(delta_<10e-3)
        value=valueCurrent;
    else
    {
        value=value+(k*cutValues(valueCurrent-value)*step);
        value=limitMinMax(value,minValue,maxValue);
    }
}
//! установка текущего значения фильтра
double FilterAperiodic::setFilterValue(double v)
{
    valueCurrent=v;
    return value;
}
