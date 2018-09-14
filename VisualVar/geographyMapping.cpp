#include "geographyMapping.h"
#include "math.h"
#include "../globalFunc/gl_func.h"

GeographyMapping::GeographyMapping(QString fileNamePixmap,
                                   QString fileNameMetaInfo)
{
    isHaveMetaInfo_=false;
    isHaveImageMap_=false;

    TPairRect tempXZ;    
    ////////// 1 прямоугольник, точка 1///////////////
    tempXZ.xz[0].setX(0.0);
    tempXZ.xz[0].setY(0.0);

    tempXZ.latLon[0].setX(37.9213714599609);
    tempXZ.latLon[0].setY(55.720922807787);
    // точка 2
    tempXZ.xz[1].setX(4280);
    tempXZ.xz[1].setY(0.0);

    tempXZ.latLon[1].setX(38.2887268066406);
    tempXZ.latLon[1].setY(55.720922807787);
    // точка 3
    tempXZ.xz[2].setX(4280);
    tempXZ.xz[2].setY(3488);

    tempXZ.latLon[2].setX(38.2887268066406);
    tempXZ.latLon[2].setY(55.5519418606257);
    // точка 4
    tempXZ.xz[3].setX(0.0);
    tempXZ.xz[3].setY(3488);

    tempXZ.latLon[3].setX(37.9213714599609);
    tempXZ.latLon[3].setY(55.5519418606257);
    pairRect.push_back(tempXZ);
    //////////////////////////////////////////////////

    ////////// 2 прямоугольник, точка 1///////////////
    tempXZ.xz[0].setX(4280);
    tempXZ.xz[0].setY(0.0);

    tempXZ.latLon[0].setX(38.2887268066406);
    tempXZ.latLon[0].setY(55.720922807787);
    // точка 2
    tempXZ.xz[1].setX(8560);
    tempXZ.xz[1].setY(0.0);

    tempXZ.latLon[1].setX(38.6560821533203);
    tempXZ.latLon[1].setY(55.720922807787);
    // точка 3
    tempXZ.xz[2].setX(8560);
    tempXZ.xz[2].setY(3488);

    tempXZ.latLon[2].setX(38.6560821533203);
    tempXZ.latLon[2].setY(55.5519418606257);
    // точка 4
    tempXZ.xz[3].setX(4280);
    tempXZ.xz[3].setY(3488);

    tempXZ.latLon[3].setX(38.2887268066406);
    tempXZ.latLon[3].setY(55.5519418606257);
    pairRect.push_back(tempXZ);
    //////////////////////////////////////////////////

    ////////// 3 прямоугольник, точка 1///////////////
    tempXZ.xz[0].setX(4280);
    tempXZ.xz[0].setY(3488);

    tempXZ.latLon[0].setX(38.2887268066406);
    tempXZ.latLon[0].setY(55.5519418606257);
    // точка 2
    tempXZ.xz[1].setX(8560);
    tempXZ.xz[1].setY(3488);

    tempXZ.latLon[1].setX(38.6560821533203);
    tempXZ.latLon[1].setY(55.5519418606257);
    // точка 3
    tempXZ.xz[2].setX(8560);
    tempXZ.xz[2].setY(6976);

    tempXZ.latLon[2].setX(38.6560821533203);
    tempXZ.latLon[2].setY(55.3822312320878);
    // точка 4
    tempXZ.xz[3].setX(4280);
    tempXZ.xz[3].setY(6976);

    tempXZ.latLon[3].setX(38.2887268066406);
    tempXZ.latLon[3].setY(55.3822312320878);
    pairRect.push_back(tempXZ);
    //////////////////////////////////////////////////

    ////////// 4 прямоугольник, точка 1///////////////
    tempXZ.xz[0].setX(0);
    tempXZ.xz[0].setY(3488);

    tempXZ.latLon[0].setX(37.9213714599609);
    tempXZ.latLon[0].setY(55.5519418606257);
    // точка 2
    tempXZ.xz[1].setX(4280);
    tempXZ.xz[1].setY(3488);

    tempXZ.latLon[1].setX(38.2887268066406);
    tempXZ.latLon[1].setY(55.5519418606257);
    // точка 3
    tempXZ.xz[2].setX(4280);
    tempXZ.xz[2].setY(6976);

    tempXZ.latLon[2].setX(38.2887268066406);
    tempXZ.latLon[2].setY(55.3822312320878);
    // точка 4
    tempXZ.xz[3].setX(0.0);
    tempXZ.xz[3].setY(6976);

    tempXZ.latLon[3].setX(37.9213714599609);
    tempXZ.latLon[3].setY(55.3822312320878);
    pairRect.push_back(tempXZ);
    //////////////////////////////////////////////////

    scaleLonToMeter=distanceSphere(37.9213714599609,55.720922807787,38.6560821533203,55.720922807787)/8560;
    scaleLatToMeter=distanceSphere(37.9213714599609,55.720922807787,37.9213714599609,55.3822312320878)/6976;
}
//! загружена ли метаинформация о карте
bool GeographyMapping::isHaveMetaInfo()
{
    return isHaveMetaInfo_;
}
//! загружено ли изображение карты
bool GeographyMapping::isHaveImageMap()
{
    return isHaveImageMap_;
}
//! прочитать файл с изображением карты
bool GeographyMapping::openImageFile(QString fileNamePixmap)
{
    return false;
}
//! прочитать файл с метаинформацией о карте
bool GeographyMapping::openMetaInfoFile(QString fileNameMetaInfo)
{
    return false;
}
bool GeographyMapping::isPointInRect(TPairRect *rect,QPointF *point)
{
    Q_UNUSED(rect);
    Q_UNUSED(point);
    return false;
}
//! функция преобразования прямоугольных в географические координаты
void GeographyMapping::mappingToXZ(double inLon,double inLat,double &outX,double &outZ)
{
    double dp=0.0;
    double dq=0.0;

    double f11X=0.0;
    double f12X=0.0;
    double f21X=0.0;
    double f22X=0.0;

    double f11Z=0.0;
    double f12Z=0.0;
    double f21Z=0.0;
    double f22Z=0.0;

    TPairRect tempRect;
    //поиск прямоугольника в который попадает точка
    for(int i=0;i<pairRect.size();i++)
    {
        tempRect=pairRect[i];

        if((inLon>pairRect[i].latLon[0].x()) &&
           (inLon<pairRect[i].latLon[1].x()) &&
           (inLat<pairRect[i].latLon[1].y()) &&
           (inLat>pairRect[i].latLon[3].y()))
        {
            double x1=pairRect[i].latLon[0].x();
            double x2=pairRect[i].latLon[1].x();

            double y1=pairRect[i].latLon[0].y();
            double y2=pairRect[i].latLon[3].y();

            dp=(inLon-x1)/(x2-x1);
            dq=(inLat-y1)/(y2-y1);

            f11X=pairRect[i].xz[0].x();
            f12X=pairRect[i].xz[3].x();
            f21X=pairRect[i].xz[1].x();
            f22X=pairRect[i].xz[2].x();

            f11Z=pairRect[i].xz[0].y();
            f12Z=pairRect[i].xz[3].y();
            f21Z=pairRect[i].xz[1].y();
            f22Z=pairRect[i].xz[2].y();
        }
    }
    outX=linearInterpolation2Arg(dp,dq,f11X,f12X,f21X,f22X);
    outZ=linearInterpolation2Arg(dp,dq,f11Z,f12Z,f21Z,f22Z);
}
//! функция преобразования географических в прямоугольные координаты
void GeographyMapping::mappingToLatLon(double inX,double inZ,double &outLon,double &outLat)
{
    double dp=0.0;
    double dq=0.0;

    double f11Lat=0.0;
    double f12Lat=0.0;
    double f21Lat=0.0;
    double f22Lat=0.0;

    double f11Lon=0.0;
    double f12Lon=0.0;
    double f21Lon=0.0;
    double f22Lon=0.0;

    TPairRect tempRect;
    //поиск прямоугольника в который попадает точка
    for(int i=0;i<pairRect.size();i++)
    {
        tempRect=pairRect[i];

        if((inX>=pairRect[i].xz[0].x()) &&
           (inX<=pairRect[i].xz[1].x()) &&
           (inZ>=pairRect[i].xz[1].y()) &&
           (inZ<=pairRect[i].xz[3].y()))
        {
            double x1=pairRect[i].xz[0].x();
            double x2=pairRect[i].xz[1].x();

            double y1=pairRect[i].xz[0].y();
            double y2=pairRect[i].xz[3].y();

            dp=(inX-x1)/(x2-x1);
            dq=(inZ-y1)/(y2-y1);

            f11Lat=pairRect[i].latLon[0].y();
            f12Lat=pairRect[i].latLon[3].y();
            f21Lat=pairRect[i].latLon[1].y();
            f22Lat=pairRect[i].latLon[2].y();

            f11Lon=pairRect[i].latLon[0].x();
            f12Lon=pairRect[i].latLon[3].x();
            f21Lon=pairRect[i].latLon[1].x();
            f22Lon=pairRect[i].latLon[2].x();
        }
    }
    outLat=linearInterpolation2Arg(dp,dq,f11Lat,f12Lat,f21Lat,f22Lat);
    outLon=linearInterpolation2Arg(dp,dq,f11Lon,f12Lon,f21Lon,f22Lon);
}
double  GeographyMapping::linearInterpolation2Arg(double dp,double dq,double f11,double f12,double f21,double f22)
{
    return f11*(1.0-dp-dq+dp*dq)+f12*dq*(1.0-dp)+f21*dp*(1.0-dq)+f22*dp*dq;
}
double GeographyMapping::mappingLonToMeter(double value)
{
    return value*scaleLonToMeter;
}
double GeographyMapping::mappingLatToMeter(double value)
{
    return value*scaleLatToMeter;
}
double GeographyMapping::distanceSphere(double Lon1,double Lat1,double Lon2,double Lat2)
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

