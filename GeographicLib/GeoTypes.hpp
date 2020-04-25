#ifndef _GEOTYPES_H_
#define _GEOTYPES_H_

#include <math.h>

class GeoCoords
{
public:
    double latitude; //Широта, гр
    double longitude; //Долгота, гр
    double height; //Высота, м

    GeoCoords():latitude(0.0),longitude(0.0),height(0.0)
    {}
    GeoCoords(double a, double b, double c = 0):latitude(a),longitude(b),height(c)
    {}
    GeoCoords(const GeoCoords& c):latitude(c.latitude),longitude(c.longitude),height(c.height)
    {}

    bool operator ==(const GeoCoords& p) const
    {
        return latitude == p.latitude && longitude == p.longitude && height == p.height;
    }
};

class GeoVector
{
public:
    union
    {
        struct
        {
            double x, y, z;
        };
        struct
        {
            double r, g, b;
        };
        double v[3];
    };
    GeoVector() :
            x(0), y(0), z(0)
    {
    }
    GeoVector(double _x, double _y = 0, double _z = 0) :
            x(_x), y(_y), z(_z)
    {
    }
    GeoVector(const GeoVector &vec) :
            x(vec.x), y(vec.y), z(vec.z)
    {
    }
    explicit GeoVector(const double vec[3]) :
            x(vec[0]), y(vec[1]), z(vec[2])
    {
    }

    inline GeoVector normalized() const;
    inline void normalize();

    double directDistance(const GeoVector& vec) const
    {
        return sqrt((vec.x - x) * (vec.x - x) + (vec.y - y) * (vec.y - y) + (vec.z - z) * (vec.z - z));
    }
    double length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    double squaredLength() const
    {
        return x * x + y * y + z * z;
    }
    double &operator[](int a)
    {
        return v[a];
    }
    double operator[](int a) const
    {
        return v[a];
    }

    static double dotProduct(const GeoVector& v1, const GeoVector& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    static GeoVector crossProduct(const GeoVector& v1, const GeoVector& v2)
    {
        return GeoVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    inline GeoVector &operator+=(const GeoVector &vector);
    inline GeoVector &operator-=(const GeoVector &vector);
    inline GeoVector &operator*=(double factor);
    inline GeoVector &operator*=(const GeoVector& vector);
    inline GeoVector &operator/=(double divisor);

    friend inline bool operator==(const GeoVector &v1, const GeoVector &v2);
    friend inline bool operator!=(const GeoVector &v1, const GeoVector &v2);
    friend inline const GeoVector operator+(const GeoVector &v1, const GeoVector &v2);
    friend inline const GeoVector operator-(const GeoVector &v1, const GeoVector &v2);
    friend inline const GeoVector operator*(double factor, const GeoVector &vector);
    friend inline const GeoVector operator*(const GeoVector &vector, double factor);
    friend const GeoVector operator*(const GeoVector &v1, const GeoVector& v2);
    friend inline const GeoVector operator-(const GeoVector &vector);
    friend inline const GeoVector operator/(const GeoVector &vector, double divisor);

    friend inline bool qFuzzyCompare(const GeoVector& v1, const GeoVector& v2);
};

inline GeoVector &GeoVector::operator+=(const GeoVector &vector)
{
    x += vector.x;
    y += vector.y;
    z += vector.z;
    return *this;
}

inline GeoVector &GeoVector::operator-=(const GeoVector &vector)
{
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    return *this;
}

inline GeoVector &GeoVector::operator*=(double factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

inline GeoVector &GeoVector::operator*=(const GeoVector& vector)
{
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;
    return *this;
}

inline GeoVector &GeoVector::operator/=(double divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
    return *this;
}

inline bool operator==(const GeoVector &v1, const GeoVector &v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline bool operator!=(const GeoVector &v1, const GeoVector &v2)
{
    return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

inline const GeoVector operator+(const GeoVector &v1, const GeoVector &v2)
{
    return GeoVector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline const GeoVector operator-(const GeoVector &v1, const GeoVector &v2)
{
    return GeoVector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline const GeoVector operator*(double factor, const GeoVector &vector)
{
    return GeoVector(vector.x * factor, vector.y * factor, vector.z * factor);
}

inline const GeoVector operator*(const GeoVector &vector, double factor)
{
    return GeoVector(vector.x * factor, vector.y * factor, vector.z * factor);
}

inline const GeoVector operator*(const GeoVector &v1, const GeoVector& v2)
{
    return GeoVector(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline const GeoVector operator-(const GeoVector &vector)
{
    return GeoVector(-vector.x, -vector.y, -vector.z);
}

inline const GeoVector operator/(const GeoVector &vector, double divisor)
{
    return GeoVector(vector.x / divisor, vector.y / divisor, vector.z / divisor);
}

inline bool qFuzzyIsNull(const GeoVector& v1)
{
    return qFuzzyIsNull(v1.x) && qFuzzyIsNull(v1.y) && qFuzzyIsNull(v1.z);
}

inline bool qFuzzyCompare(const GeoVector& v1, const GeoVector& v2)
{
    return qFuzzyCompare(v1.x, v2.x) && qFuzzyCompare(v1.y, v2.y) && qFuzzyCompare(v1.z, v2.z);
}
/*
inline GeoVector qMin(const GeoVector& v1, const GeoVector& v2)
{
    return GeoVector(qMin(v1.x, v2.x), qMin(v1.y, v2.y), qMin(v1.z, v2.z));
}
inline GeoVector qMax(const GeoVector& v1, const GeoVector& v2)
{
    return GeoVector(qMax(v1.x, v2.x), qMax(v1.y, v2.y), qMax(v1.z, v2.z));
}
*/
inline GeoVector GeoVector::normalized() const
{
    if(qFuzzyIsNull(*this))
        return *this;
    double len = squaredLength();

    if(qFuzzyIsNull(len - 1.0))
        return *this;

    return *this / sqrt(len);
}

void GeoVector::normalize()
{
    if(qFuzzyIsNull(*this))
        return;

    double len = squaredLength();
    if(qFuzzyIsNull(len - 1.0))
        return;

    len = 1.0 / sqrt(len);

    x *= len;
    y *= len;
    z *= len;
}

#endif //_GEOTYPES_H_
