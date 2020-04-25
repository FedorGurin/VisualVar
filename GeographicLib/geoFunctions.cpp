/*
 * geoFunctions.cpp
 *
 *  Created on: 29 мая 2017 г.
 *      Author: mornelon
 */

#include "GeographicLib/geoFunctions.h"
#include "global.h"

namespace nGeoFunc
{

TIntersectionData line_fi_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double fiY, double fiZ)
{
    double x, y, z;
    y = 1000*sin(fiZ);
    x = 1000*cos(fiZ)*cos(fiY);
    z = 1000*cos(fiZ)*sin(fiY);

    return(line_lalo_intersect_geoid(_earth, _lat, _lon, _h, x, y, z));
}

/*
 * На вход берёт геойд. Координаты первой точки и координаты второй точки
 * (либо ед.вектор) в локальных декартовых координатах (LocalCartesian)
 * относительно первой точки.
 */
TIntersectionData line_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, Tpoint _point2)
{
    return(line_lalo_intersect_geoid(_earth, _lat, _lon, _h, _point2.x, _point2.y, _point2.z));
}

TIntersectionData line_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double _x, double _y, double _z)
{
    GeographicLib::LocalCartesian locals(_lat*RAD2GRAD, _lon*RAD2GRAD, _h, *_earth);

    // Получаем координаты второй точки.
    double lat2, lon2, h2;

    locals.Reverse(_z, _x, _y, lat2, lon2, h2);

    // получаем координаты двух точек прямой.
    Tpoint aPoint1;
    Tpoint aPoint2;
    _earth->Forward(_lat, _lon, _h, aPoint1.x, aPoint1.y, aPoint1.z);
    _earth->Forward(lat2, lon2, h2, aPoint2.x, aPoint2.y, aPoint2.z);

    return(line_intersect_geoid(_earth, aPoint1.x, aPoint1.y, aPoint1.z, aPoint2.x, aPoint2.y, aPoint2.z));
}



/*
 * Функция получения точек пересечения прямой, заданной двумя точками,
 * геойда.
 * x, y, z передаются в модельной манере:
 * y - нормаль к элепсойду.
 * x - направление на север по касательной к поверхности элепсойда.
 * z - дополняет (восток).
 * Если логика не понятна см. черновик.
 *
 * Порядок рассчета:
 * Уравнение элипсойда вида: x^2/a^2 + y^2/b^2 + z^2/a^2 = 1.
 * Уравнение прямой: (x - x1)/(x2 - x1) = (y - y1)/(y2 - y1) = (z - z1)/(z2 - z1)
 * Записываем прямую в виде:
 *     x = alfa*y + c1;
 *     x = beta*z + c2;
 *     где:
 *         alfa = (x2 - x1)/(y2 - y1);
 *         beta = (x2 - x1)/(z2 - z1);
 *         c1 = _x1 - y1*alfa;
 *         c2 = _x1 - z1*beta;
 *
 * Подставляем Y и Z в уравнение элепса и приводим к квадратному уравнению вида:
 *     ac*x^2 + bc*x + cc = 0
 *
 * Находим четверть-дискриминант:
 *     b2 = bc/2;
 *     d4 = b2^2 - ac;
 *
 * d4 должен быть больше либо равен 0.
 *
 * Находим корни уравнения:
 * x = (b2 -+ sqrt(d4))/ac;
 * y = (x - c1)/alfa;
 * z = (x - c2)/beta;
 *
 * Точки найдены ;)
 *
 *  Также рассматриваются частные случаи прямых, когда x = const или y = const или z = const.
 */

TIntersectionData line_intersect_geoid(GeographicLib::Geocentric* _earth, Tpoint _point1, Tpoint _point2)
{
    return(line_intersect_geoid(_earth, _point1.x, _point1.y, _point1.z, _point2.x, _point2.y, _point2.z));
}

TIntersectionData line_intersect_geoid(GeographicLib::Geocentric* _earth, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2)
{
    // Коэффициенты уравнения прямой.
    double alfa = 0, beta = 0, c1 = 0, c2 = 0;
    // Коэффициенты квадратного уравнения.
    double ac = 0, cc = 0, b2 = 0, d4 = 0;

    // Получаем радиусы элипса.
    double a = _earth->MajorRadius();
    double b = (1 - _earth->Flattening())*a;

    // Создаем экземпляр для результатов вычислений.
    TIntersectionData result;

    // Получаем коэффициенты уравнения прямой.
    if(_x2 - _x1 == 0)
    {
        if(_z2 - _z1 == 0)
        {
            b = b*b;
            a = a*a;

            d4 = b - b*(_x1*_x1 + _z1*_z1)/a;

            if(d4 >= 0)
            {
                d4 = sqrt(d4);
                result.intersect = true;
                result.intersection[0].z = _z1;
                result.intersection[1].z = _z1;

                result.intersection[0].y = d4;
                result.intersection[1].y = -d4;

                result.intersection[0].x = _x1;
                result.intersection[1].x = _x1;
            }
            else
            {
                // Нет пересечения.
                result.intersect = false;
                result.intersection[0].x = 0;
                result.intersection[1].x = 0;

                result.intersection[0].y = 0;
                result.intersection[1].y = 0;

                result.intersection[0].z = 0;
                result.intersection[1].z = 0;
            }
        }
        else if(_y2 - _y1 == 0)
        {
            b = b*b;
            a = a*a;

            d4 = a -_x1*_x1 - a*_y1*_y1/b;

            if(d4 >= 0)
            {
                d4 = sqrt(d4);
                result.intersect = true;
                result.intersection[0].z = d4;
                result.intersection[1].z = -d4;

                result.intersection[0].y = _y1;
                result.intersection[1].y = _y1;

                result.intersection[0].x = _x1;
                result.intersection[1].x = _x1;
            }
            else
            {
                // Нет пересечения.
                result.intersect = false;
                result.intersection[0].x = 0;
                result.intersection[1].x = 0;

                result.intersection[0].y = 0;
                result.intersection[1].y = 0;

                result.intersection[0].z = 0;
                result.intersection[1].z = 0;
            }
        }
        else
        {
            alfa = (_z2 - _z1)/(_y2 - _y1);
            c1 = _z1 - _y1*alfa;
            c2 = _x1;

            // Находим коэффициенты квадратного уравнения.
            b = b*b;
            a = a*a;
            double alfa2 = alfa*alfa;
            //double bata2 = beta*beta;
            ac = (b*alfa2 + a);
            b2 = -(c1*a);
            cc = c1*c1*a + c2*c2*b*alfa2 - a*b*alfa2; //!!! Проверь !!!

            // Подучаем четверть дискриминанта.
            d4 = b2*b2 - ac*cc;

            if(d4 >= 0)
            {
                d4 = sqrt(d4);
                result.intersect = true;
                result.intersection[0].z = (b2 + d4)/ac;
                result.intersection[1].z = (b2 - d4)/ac;

                result.intersection[0].y = (result.intersection[0].x - c1)/alfa;
                result.intersection[1].y = (result.intersection[1].x - c1)/alfa;

                result.intersection[0].x = c2;
                result.intersection[1].x = c2;
            }
            else
            {
                // Нет пересечения.
                result.intersect = false;
                result.intersection[0].x = 0;
                result.intersection[1].x = 0;

                result.intersection[0].y = 0;
                result.intersection[1].y = 0;

                result.intersection[0].z = 0;
                result.intersection[1].z = 0;
            }
        }
    }
    else if(_y2 - _y1 == 0)
    {
        if(_z2 - _z1 == 0)
        {
            b = b*b;
            a = a*a;

            d4 = a -_z1*_z1 - a*_y1*_y1/b;

            if(d4 >= 0)
            {
                d4 = sqrt(d4);
                result.intersect = true;
                result.intersection[0].z = _z1;
                result.intersection[1].z = _z1;

                result.intersection[0].y = _y1;
                result.intersection[1].y = _y1;

                result.intersection[0].x = d4;
                result.intersection[1].x = -d4;
            }
            else
            {
                // Нет пересечения.
                result.intersect = false;
                result.intersection[0].x = 0;
                result.intersection[1].x = 0;

                result.intersection[0].y = 0;
                result.intersection[1].y = 0;

                result.intersection[0].z = 0;
                result.intersection[1].z = 0;
            }
        }
        else
        {
            beta = (_x2 - _x1)/(_z2 - _z1);
            c1 = _y1;
            c2 = _x1 - _z1*beta;

            // Находим коэффициенты квадратного уравнения.
            b = b*b;
            a = a*a;
            double beta2 = beta*beta;
            ac = (b*beta2 + b);
            b2 = -(c2*b);
            cc = c1*c1*a*beta + c2*c2*b - a*b*beta2;\

            // Подучаем четверть дискриминанта.
            d4 = b2*b2 - ac*cc;

            if(d4 >= 0)
            {
                d4 = sqrt(d4);
                result.intersect = true;
                result.intersection[0].x = (b2 + d4)/ac;
                result.intersection[1].x = (b2 - d4)/ac;

                result.intersection[0].y = c1;
                result.intersection[1].y = c1;

                result.intersection[0].z = (result.intersection[0].x - c2)/beta;
                result.intersection[1].z = (result.intersection[1].x - c2)/beta;
            }
            else
            {
                // Нет пересечения.
                result.intersect = false;
                result.intersection[0].x = 0;
                result.intersection[1].x = 0;

                result.intersection[0].y = 0;
                result.intersection[1].y = 0;

                result.intersection[0].z = 0;
                result.intersection[1].z = 0;
            }
        }
    }
    else if(_z2 - _z1 == 0)
    {
        alfa = (_x2 - _x1)/(_y2 - _y1);
        c1 = _x1 - _y1*alfa;
        c2 = _z1;

        // Находим коэффициенты квадратного уравнения.
        b = b*b;
        a = a*a;
        double alfa2 = alfa*alfa;
        // double bata2 = beta*beta; //!!!
        ac = (b*alfa2 + a);
        b2 = -(c1*a);
        cc = c1*c1*a + c2*c2*b*alfa2 - a*b*alfa2;

        // Подучаем четверть дискриминанта.
        d4 = b2*b2 - ac*cc;

        if(d4 >= 0)
        {
            d4 = sqrt(d4);
            result.intersect = true;
            result.intersection[0].x = (b2 + d4)/ac;
            result.intersection[1].x = (b2 - d4)/ac;

            result.intersection[0].y = (result.intersection[0].x - c1)/alfa;
            result.intersection[1].y = (result.intersection[1].x - c1)/alfa;

            result.intersection[0].z = c2;
            result.intersection[1].z = c2;
        }
        else
        {
            // Нет пересечения.
            result.intersect = false;
            result.intersection[0].x = 0;
            result.intersection[1].x = 0;

            result.intersection[0].y = 0;
            result.intersection[1].y = 0;

            result.intersection[0].z = 0;
            result.intersection[1].z = 0;
        }
    }
    else
    {
        alfa = (_x2 - _x1)/(_y2 - _y1);
        beta = (_x2 - _x1)/(_z2 - _z1);
        c1 = _x1 - _y1*alfa;
        c2 = _x1 - _z1*beta;

        // Находим коэффициенты квадратного уравнения.
        b = b*b;
        a = a*a;
        double alfa2 = alfa*alfa;
        double beta2 = beta*beta;
        ac = (b*alfa2*beta2 + a*beta2 + b*alfa2);
        b2 = -(c1*a*beta2 + c2*b*alfa2);
        cc = c1*c1*a*beta + c2*c2*b*alfa2 - a*b*alfa2*beta2;

        // Подучаем четверть дискриминанта.
        d4 = b2*b2 - ac*cc;

        if(d4 >= 0)
        {
            d4 = sqrt(d4);
            result.intersect = true;
            result.intersection[0].x = (b2 + d4)/ac;
            result.intersection[1].x = (b2 - d4)/ac;

            result.intersection[0].y = (result.intersection[0].x - c1)/alfa;
            result.intersection[1].y = (result.intersection[1].x - c1)/alfa;

            result.intersection[0].z = (result.intersection[0].x - c2)/beta;
            result.intersection[1].z = (result.intersection[1].x - c2)/beta;
        }
        else
        {
            // Нет пересечения.
            result.intersect = false;
            result.intersection[0].x = 0;
            result.intersection[1].x = 0;

            result.intersection[0].y = 0;
            result.intersection[1].y = 0;

            result.intersection[0].z = 0;
            result.intersection[1].z = 0;
        }
    }

    return(result);
}

/*
 * Всё тоже самое, но для случая с лучом. Т.о. вторая точка задает направление
 * и точки пересечения по другую сторону учитываться не будут.
 */

int check_values(double control1, double control2, double point1, double point2)
{
    int x = -1;
    if(control1 != control2)
    {
        if(control1 < control2)
        {
            if(control1 <= point1)
            {
                x = 0;
            }

            if(control1 < point2 && point1 > point2)
            {
                x = 1;
            }
        }
        else
        {
            if(control1 >= point1)
            {
                x = 0;
            }

            if(control1 > point2 && point1 < point2)
            {
                x = 1;
            }
        }
    }
    return(x);
}

TIntersectionData line2ray(TIntersectionData _res, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2)
{
    if(_res.intersect)
    {
        int ind = 0;
        ind = check_values(_x1, _x2, _res.intersection[0].x, _res.intersection[1].x);
        if(ind < 0)
        {
            ind = check_values(_y1, _y2, _res.intersection[0].y, _res.intersection[1].y);
            if(ind < 0)
            {
                ind = check_values(_z1, _z2, _res.intersection[0].y, _res.intersection[1].y);
                if(ind < 0)
                {
                    ind = 0;
                }
            }
        }

        if(ind == 1)
        {
            _res.intersection[0].x = _res.intersection[1].x;
            _res.intersection[0].y = _res.intersection[1].y;
            _res.intersection[0].z = _res.intersection[1].z;
        }
        _res.intersection[1].x = 0;
        _res.intersection[1].y = 0;
        _res.intersection[1].z = 0;
    }
    return(_res);
}

TIntersectionData ray_intersect_geoid(GeographicLib::Geocentric* _earth, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2)
{
    TIntersectionData res = line_intersect_geoid(_earth, _x1, _y1, _z1, _x2, _y2, _z2);

    line2ray(res, _x1, _y1, _z1, _x2, _y2, _z2);

    return(res);
}

TIntersectionData ray_intersect_geoid(GeographicLib::Geocentric* _earth, Tpoint _point1, Tpoint _point2)
{
    return(ray_intersect_geoid(_earth, _point1.x, _point1.y, _point1.z, _point2.x, _point2.y, _point2.z));
}

TIntersectionData ray_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double _x, double _y, double _z)
{
    GeographicLib::LocalCartesian locals(_lat*RAD2GRAD, _lon*RAD2GRAD, _h, *_earth);

    // Получаем координаты второй точки.
    double lat2, lon2, h2;

    locals.Reverse(_z, _x, _y, lat2, lon2, h2);

    // получаем координаты двух точек прямой.
    Tpoint aPoint1;
    Tpoint aPoint2;
    _earth->Forward(_lat, _lon, _h, aPoint1.x, aPoint1.y, aPoint1.z);
    _earth->Forward(lat2, lon2, h2, aPoint2.x, aPoint2.y, aPoint2.z);

    TIntersectionData res = ray_intersect_geoid(_earth, aPoint1.x, aPoint1.y, aPoint1.z, aPoint2.x, aPoint2.y, aPoint2.z);

    res = line2ray(res, aPoint1.x, aPoint1.y, aPoint1.z, aPoint2.x, aPoint2.y, aPoint2.z);

    return(res);
}

TIntersectionData ray_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, Tpoint _point2)
{
    return(ray_lalo_intersect_geoid(_earth, _lat, _lon, _h, _point2.x, _point2.y, _point2.z));
}

TIntersectionData ray_fi_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double fiY, double fiZ)
{
    double x, y, z;
    y = 1000*sin(fiZ);
    x = 1000*cos(fiZ)*cos(fiY);
    z = 1000*cos(fiZ)*sin(fiY);

    return(ray_lalo_intersect_geoid(_earth, _lat, _lon, _h, x, y, z));
}

}


