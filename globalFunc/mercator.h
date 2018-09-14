#ifndef MERCATOR_H
#define MERCATOR_H






//double fmin(double x, double y);
//double fmax(double x, double y);


double deg_rad (double ang);


double merc_x (double lon);

double merc_y (double lat);

double rad_deg (double ang);

double merc_lon (double x);

double merc_lat (double y);
void mercXYtoLonLat(double x,double y,double &lon,double &lat);

void mercLonLattoXY(double lon,double lat, double &x, double &y);

#endif // MERCATOR_H
