#include "weatherairnow.h"

weatherAirNow::weatherAirNow()
{

}

weatherAirNow::~weatherAirNow()
{

}

void weatherAirNow::disposeWeatherInfo(QJsonObject &jsonObject)
{
    if (jsonObject.contains("air_now_city"))
    {
        QJsonObject airQuality = jsonObject.value("air_now_city").toObject();

        m_airQualityCity.aqi = airQuality.value("aqi").toString().toInt();                //	空气质量指数，AQI和PM25的关系	74
        m_airQualityCity.main = airQuality.value("main").toString();           //	主要污染物	pm25
        m_airQualityCity.qlty = airQuality.value("qlty").toString();           //	空气质量，取值范围:优，良，轻度污染，中度污染，重度污染，严重污染，查看计算方式	良
        m_airQualityCity.pm10 = airQuality.value("pm10").toString().toInt();               //	pm10	78
        m_airQualityCity.pm25 = airQuality.value("pm25").toString().toInt();               //	pm25	66
        m_airQualityCity.no2 = airQuality.value("no2").toString().toFloat();                //	二氧化氮	40
        m_airQualityCity.so2 = airQuality.value("so2").toString().toFloat();                //	二氧化硫	30
        m_airQualityCity.co = airQuality.value("co").toString().toFloat();                 //	一氧化碳	33
        m_airQualityCity.o3 = airQuality.value("o3").toString().toFloat();                 //	臭氧	20
    }
}

weatherAirNow::AirQualityCity weatherAirNow::getAirQualityCity(void)
{
    return m_airQualityCity;
}
