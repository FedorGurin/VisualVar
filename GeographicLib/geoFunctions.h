/*
 * geoFunctions.h
 *
 *  Created on: 29 мая 2017 г.
 *      Author: mornelon
 */

#ifndef GEOFUNCTIONS_H_
#define GEOFUNCTIONS_H_

#include "LocalCartesian.hpp"

namespace nGeoFunc
{

struct Tpoint
{
    double x;
    double y;
    double z;
};

struct TIntersectionData
{
    bool intersect;
    Tpoint intersection[2];
};

TIntersectionData line2ray(TIntersectionData _res, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2);
TIntersectionData ray_intersect_geoid(GeographicLib::Geocentric* _earth, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2);
TIntersectionData ray_intersect_geoid(GeographicLib::Geocentric* _earth, Tpoint _point1, Tpoint _point2);
TIntersectionData ray_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double _x, double _y, double _z);
TIntersectionData ray_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, Tpoint _point2);
TIntersectionData ray_fi_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double fiY, double fiZ);

TIntersectionData line_intersect_geoid(GeographicLib::Geocentric* _earth, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2);
TIntersectionData line_intersect_geoid(GeographicLib::Geocentric* _earth, Tpoint _point1, Tpoint _point2);
TIntersectionData line_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double _x, double _y, double _z);
TIntersectionData line_lalo_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, Tpoint _point2);
TIntersectionData line_fi_intersect_geoid(GeographicLib::Geocentric* _earth, double _lat, double _lon, double _h, double fiY, double fiZ);

int check_values(double control1, double control2, double point1, double point2);
TIntersectionData line2ray(TIntersectionData _res, double _x1, double _y1, double _z1, double _x2, double _y2, double _z2);

}

#endif /* GEOFUNCTIONS_H_ */
