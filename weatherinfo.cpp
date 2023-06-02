#include "weatherinfo.h"

weatherInfo::weatherInfo()
{

}

weatherInfo::~weatherInfo()
{

}

weatherInfo::WeatherNowInfo weatherInfo::getNowWeatherInfo(void)
{
    return m_weatherInfo;
}

void weatherInfo::disposeWeatherInfo(QJsonObject &jsonObjects)
{
    // 解析天气预报
    if (jsonObjects.contains("now"))
    {
        QJsonObject jsonObject = jsonObjects.value("now").toObject();
        m_weatherInfo.fl = jsonObject.value("fl").toString().toInt();                     //	体感温度，默认单位：摄氏度	23
        m_weatherInfo.tmp = jsonObject.value("tmp").toString().toInt();                    //	温度，默认单位：摄氏度	21
        m_weatherInfo.cond_code = jsonObject.value("cond_code").toString().toInt();              //	实况天气状况代码	100
        m_weatherInfo.cond_txt = jsonObject.value("cond_txt").toString();           //	实况天气状况代码	晴
        m_weatherInfo.wind_deg = jsonObject.value("wind_deg").toString().toInt();               //	风向360角度	305
        m_weatherInfo.wind_dir = jsonObject.value("wind_dir").toString();           //	风向	西北
        m_weatherInfo.wind_sc = jsonObject.value("wind_sc").toString().toInt();                //	风力	3
        m_weatherInfo.wind_spd = jsonObject.value("wind_spd").toString().toInt();               //	风速，公里/小时	15
        m_weatherInfo.hum = jsonObject.value("hum").toString().toInt();                    //	相对湿度	40
        m_weatherInfo.pcpn = jsonObject.value("pcpn").toString().toInt();                   //	降水量	0
        m_weatherInfo.pres = jsonObject.value("pres").toString().toInt();                   //	大气压强	1020
        m_weatherInfo.vis = jsonObject.value("vis").toString().toInt();                    //	能见度，默认单位：公里	10
        m_weatherInfo.cloud = jsonObject.value("cloud").toString().toInt();                  //	云量	23
    }
}
