#ifndef SETTINGVV_H
#define SETTINGVV_H

#include <QString>
#include <QObject>

class SettingVV:public QObject
{
public:
    SettingVV(QObject* parent = nullptr);

    //! директория по умолчанию, куда обычно сохраняются варианты
    QString dirSaveVar;
    //! директория с картой
    QString pathWithMap;

    //! начальная точка старта на карте
    double startLat;
    double startLon;
    //! уровень детализации
    int zoom;   
    //! разрешение отрисовки двигующихся объектов
    int moveObjects;  
    //! тип используемой по умолчанию карты
    QString typeMap;
    //! тип используемого по умолчанию слоя
    QString typeLayer;
    //! путь к параметрам задания
    QString pathToObj;
};

#endif // SETTINGVV_H
