#ifndef WEATHERAIRNOW_H
#define WEATHERAIRNOW_H

#include "weatherbaseinfo.h"
#include <QDateTime>

class weatherAirNow : public weatherBaseInfo
{
    Q_OBJECT
public:
    weatherAirNow();
    ~weatherAirNow();

    struct AirQualityCity
    {
        int aqi;                //	空气质量指数，AQI和PM25的关系	74
        QString main;           //	主要污染物	pm25
        QString qlty;           //	空气质量，取值范围:优，良，轻度污染，中度污染，重度污染，严重污染，查看计算方式	良
        int pm10;               //	pm10	78
        int pm25;               //	pm25	66
        float no2;                //	二氧化氮	40
        float so2;                //	二氧化硫	30
        float co;                 //	一氧化碳	33
        float o3;                 //	臭氧	20
    };

    void disposeWeatherInfo(QJsonObject &jsonObject) override;//解析天气预报json

    AirQualityCity getAirQualityCity(void);//获取空气质量

private:
    AirQualityCity m_airQualityCity;

};

#endif // WEATHERAIRNOW_H
