#ifndef GEOGRAPHYMAPPING_H
#define GEOGRAPHYMAPPING_H

#include <QPixmap>

typedef struct
{
    QPointF xz[4];	//! прямоугольник по коор. xz
    QPointF latLon[4];	//! прямоугольник со значниями LatLon x-lon(долгота), y-lat(широта)
}TPairRect;

//! географическая система координат
class GeographyMapping
{
public:
    GeographyMapping(QString fileNamePixmap="",
                     QString fileNameMetaInfo="");

    //! прочитать файл с изображением карты
    bool openImageFile(QString fileNamePixmap);
    //! прочитать файл с метаинформацией о карте
    bool openMetaInfoFile(QString fileNameMetaInfo);
    //! проверить загружена ли метаинформация о карте
    bool isHaveMetaInfo();
    //! проверка загружена ли карта
    bool isHaveImageMap();
    //! отображение географических СК в линейные коордианты
    double mappingLonToMeter(double value);
    double mappingLatToMeter(double value);
    //! функции преобразования географических в прямоугольные координаты(пиксели) и обратно
    void mappingToXZ(double inLon,double inLat,double &outX,double &outZ);
    void mappingToLatLon(double inX,double inZ,double &outLon,double &outLat);
    //! линейная интерполяция двух переменных в прямоугольной области
    double linearInterpolation2Arg(double dp,double dq,double f11,double f12,double f21,double f22);
    //! проверка попадает ли заданная точка в указанный прямоугольник
    bool isPointInRect(TPairRect *rect,QPointF *point);
    //! расстояние на сфере, по двум точкам заданных в географической СК
    double distanceSphere(double Lon1,double Lat1,double Lon2,double Lat2);
private:
    //! масштаб географ. СК в линейную СК
    double scaleLonToMeter;
    double scaleLatToMeter;

    //! изображение карты
    QPixmap imageMap;
    //! метаинформация карты
    QString metaInfo;
    //! набор прямоугольников с прямоугольными и географическими координатами
    QVector<TPairRect> pairRect;
    //! признаки наличия метаинформации
    bool isHaveMetaInfo_;
    bool isHaveImageMap_;
};

#endif // GEOGRAPHYMAPPING_H
