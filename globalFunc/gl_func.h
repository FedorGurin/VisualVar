#ifndef _GL_FUNC_H
#define _GL_FUNC_H
#include <QString>
#include <QStringList>
#include <QtMath>

#include "mercator.h"
#include <QDomElement>
#include <QPointF>

const double EarthRadius = 6378137;//метры
//const double EarthRadius = 6356752;//метры

const double MinLatitude = -85.05112878;
const double MaxLatitude = 85.05112878;
//const double MinLatitude = -85.0;
//const double MaxLatitude = 85.0;
const double MinLongitude = -180;
const double MaxLongitude = 180;

typedef struct _Tcordd
{
        int grad;
        int min;
        double sec;
} Tcordd;
typedef struct _TGeoPoint
{
    _TGeoPoint(double lat_=0,double lon_=0)
    {
        lat=lat_;
        lon=lon_;
    }

    double lat;//широта
    double lon;//долгота

}TGeoPoint;

//! углы относительно строительной оси
typedef struct SAngle
{
    SAngle(double tau_=0.0,double sigma_=0.0):tau(tau_),sigma(sigma_){}
    void set(double tau_,double sigma_){tau=tau_;sigma=sigma_;}

    //! смещение по вертикали (положительное направление - вверх)
    double tau;
    //! смещение по горизонтали (положительное направление - влево)
    double sigma;

    /*TAngle operator + (TAngle &angle)const
    {
        TAngle a;
        a.sigma=this->sigma+angle.sigma;
        a.tau=this->tau+angle.tau;
        return a;
    }
    TAngle operator = (TAngle angle)
    {
        this->sigma=angle.sigma;
        this->tau=angle.tau;
    }*/

    void clear()
    {
        tau=0.0;
        sigma=0.0;
    }
}TAngle;
QString readParamFromXMLFile(QString fileName,QString nameProg,QString nameAttrib);
bool writeAttrParamToXMLFile(QString fileName,QString nameProg,QString nameAttrib,QString value);
QDomElement paramFromXMLFileWithAttrib(QString fileName,QString nameProg,QString nameAttrib);
/*
Преобразование градусов/минут/секунд в дробное число.
Вызов: <Результат в double> = coord_to_float(<градусы>, <минуты>, <секунды>)
*/
double coord_to_float(int grad, int min, double sec);

/*
Преобразование дробного числа в градусы/минуты/секунды
Вызов: <Результат в int>[3] = coord_to_float(<Угол дробным числом>)
Результат является ссылкой на структуру из двух int и одного double (градусы, минуты, секунды)
*/
Tcordd float_to_coord(double flt);



double GradToRadian(double x);
double RadianToGrad(double x);
//! Преобразование [180.0;-180] в [0;360.0]
double PsiToKurs(double);
//! Преобразование [0;360.0] в [180.0;-180]
double KursToPsi(double);
//! проверка выхода за [180;-180]
double checkPI(double);

double check360(double);

//! проверка выхода за [180;-180]
//double checkPI(double);
double check180(double);

//! Преобразование [0;360.0] в [180.0;-180]
//double KursToPsi(double);
double KursToPsiGrad(double kurs);
//extern VariantNode* glRootVariant;
QString TranslateFromCP1251(const QByteArray& ArrayString);
QString TranslateFromKOI8(const QByteArray& ArrayString);

/*
Преобразование градусов/минут/секунд в дробное число.
Вызов: <Результат в double> = coord_to_float(<градусы>, <минуты>, <секунды>)
*/
double coord_to_float(int grad, int min, double sec);
double geo_string_to_double(QString);

/*
Преобразование дробного числа в градусы/минуты/секунды
Вызов: <Результат в int>[3] = coord_to_float(<Угол дробным числом>)
Результат является ссылкой на структуру из двух int и одного double (градусы, минуты, секунды)
*/
Tcordd float_to_coord(double flt);
QString geo_double_to_string(double flt);

void inverseMercator(double x, double y, double &lon, double &lat);
void forwardMercator(double lon, double lat, double &x, double &y);

///Microsoft
template <typename T> double clip(T n, T minValue, T maxValue)
{
    return qMin(qMax(n, minValue), maxValue);
}

unsigned mapSize(int levelOfDetail);

//! проекция Меркатора на сфере
void latLongToPixelXY(double latitude, double longitude, int levelOfDetail, int &pixelX, int &pixelY);
void pixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double &latitude, double &longitude);
void pixelXYToLatLong(QPointF pixel, int levelOfDetail, double &latitude, double &longitude);
void pixelXYToTileXY(int pixelX, int pixelY, int &tileX, int &tileY);
void tileXYToPixelXY(int tileX, int tileY, int &pixelX, int &pixelY);

double groundResolution(double latitude, int levelOfDetail);

double mapScale(double latitude, int levelOfDetail, int screenDpi);

//! вычисление расстояния на сфере
double distanceSphere(double Lon1,double Lat1,double Lon2,double Lat2);


double cutValues(double x);
template<typename T> T limitMinMax(T value,T min,T max)
{
    if(value<min) value=min;
    else if(value>max) value=max;

    return value;
}

//! ограничение сверху
double limitMax(double value,double max);
//! ограничение снизу
double limitMin(double value,double min);
//! апериодический фильтр
class FilterAperiodic
{
public:
    FilterAperiodic(double valueStart_,double k_,double step_,double maxValue_, double minValue_);
    //! сброс фильтра
    void resetFilter(double valueStart_);
    //! установка текущего значения фильтра
    double filterValue(double valueCurr);
    //! обновление фильтра(с заданной частотой)
    void refresh();
    //! возврат текущего значения фильтра
    double setFilterValue(double currentValue);
    //! текущее значение
    double curValue(){return value;}
    //! разность между текущим и предыдущим значением
    double delta(){return delta_;}
    //! задать коэффициент при звене
    void setK(double k_);
private:
    double valueStart;
    double valueCurrent;//значение на входе в фильтр
    double maxValue;
    double minValue;

    double value;//текущее знчачение фильтра
    double step;
    double k;

    double delta_;
};
class FiltersAperiodicAngle
{
public:
    FiltersAperiodicAngle(SAngle valueStart_,
                          double k_,
                          double step_,
                          double maxValue_,
                          double minValue_);
    //! сброс фильтра
    void resetFilter(SAngle valueStart_);
    //! установка текущего значения фильтра
    SAngle filterValue(SAngle valueCurr);
    //! обновление фильтра(с заданной частотой)
    void refresh();
    //! возврат текущего значения фильтра
    SAngle setFilterValue(SAngle currentValue);
    //! задать коэффициент при звене
    void setK(double k_);

private:

    FilterAperiodic *filterSigma;
    FilterAperiodic *filterTau;
};

//! общие переменные
extern QStringList listForCompleter;
//! имя файла с настройками
extern QString nameMainFile;
#endif
