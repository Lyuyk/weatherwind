#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QColor>
#include <QDate>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include "weatherbaseinfo.h"
#include "weatherdata.h"
#include "weatherinfo.h"
#include "weatherlife.h"
#include "weatherairnow.h"

#define WEATHER_INTERFACE_BASEURL "https://free-api.qweather.com/s6/"  //API的url
//#define g_weatherInterface MainWindow::getInstance()

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum WeatherGetType
    {
        Weather_Forecast_Type,          // 天气预报
        Weather_Now_Type,               // 实时天气
        Weather_LifeStyle_Type,         // 生活指数
        Weather_AirQuality_Type         // 空气质量
    };

    struct WeatherBaseData
    {
        QString location;       // 地区／城市名称  (海淀)
        QString cid;            // 地区／城市ID    (CN101080402)
        double lat;              // 地区／城市纬度  (39.956074)
        double lon;              // 地区／城市经度  (116.310316)
        QString parent_city;    // 该地区／城市的上级城市 (北京)
        QString admin_area;     // 该地区／城市所属行政区域 (北京)
        QString cnty;           // 该地区／城市所属国家名称 (中国)
        double tz;             // 该地区／城市所在时区 (+8.0)
    };

    // 更新时间
    struct WeatherUpdateTime
    {
        QDateTime localTime;
        QDateTime utcTime;
    };
    // 状态码
    struct WeatherStatus
    {
        QString statusCode;     // 状态码
        QString note;           // 提示说明
    };

    struct CityInfo
    {
        QString cityName;                   // 城市名字
        QList<QString> regionName;          // 城市下的区的名字
    };

    //static MainWindow *getInstance(void);

    // 设置是否设置代理
    void setConfigNetWorkProxy(bool);
    // 更新当前信息
    void updateData(void);
    // 设置当前信息类型
    void setCurrentWeatherInfoType(WeatherGetType type);
    // 设置当前城市
    void setCurrentCityName(QString);
    // 获取当前城市
    QString getCurrentCityName(void);
    // 设置空气质量检测城市
    void setAirQualityCityName(QString);

    // 获取天气的基础信息
    WeatherBaseData getBaseData(void);
    // 获取更新时间信息
    WeatherUpdateTime getUpdateTime(void);
    // 获取天气预报信息
    QList<weatherData::DailyForecast> getDailyForecast(void);
    // 获取当时天气信息
    weatherInfo::WeatherNowInfo getNowWeatherInfo(void);
    // 获取当前的生活指数
    QList<weatherLife::LifeStyleData> getLifeStyleInfoList(void);
    // 获取当前的空气质量
    weatherAirNow::AirQualityCity getAirQualityCity(void);
    // 获取当前控制质量的颜色
    QColor getAirQualityCityColor(void);
    // 刷新天气信息
    void updateWeatherInfo(void);

    // 获取天气图标
    QString getWeatherIcon(int code);
    QString getWeatherIconSmall(int code);

    // 获取所有的省份
    QList<QString> getProvinceName(void);
    // 获取某个省下的所有的城市
    QList<QString> getCitysName(QString);
    // 获取某个省下的某个市下的所有的县
    QList<QString> getRegionsName(QString, QString);


private slots:
    void on_pushButton_clicked();

    void on_commandLinkButton_aboutUs_clicked();

    //void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_province_currentIndexChanged(const QString &arg1);

    void on_comboBox_city_currentIndexChanged(const QString &arg1);

    void on_comboBox_province_currentTextChanged(const QString &arg1);

    void on_pushButton_changeCity_clicked();

private:
    Ui::MainWindow *ui;

    QString m_PrivateKey;   // Key

    QNetworkAccessManager *m_Manager;


    // 是否设置代理
    bool m_IsConfigNetWorkProxy = false;
    // 要获取的天气类型
    WeatherGetType m_WeatherGetType;
    // 天气类型-类列表
    QMap<WeatherGetType, weatherBaseInfo*> m_TypeInterfaceMap;
    // 初始化数据
    void initData(void);
    // 根据类型获取URL中的缺省值
    QString getURLAttrStringByType(WeatherGetType type);
    // Json解析
    void disposeWeatherJsonData(QString);

    // 当前的城市信息
    QString  m_LocationCity;
    // 当前空气质量检测城市
    QString m_AirQualityCity;
    // 天气的基础信息
    WeatherBaseData m_WeatherBaseData;
    // 更新时间信息
    WeatherUpdateTime m_WeatherTime;
    // 状态码信息
    WeatherStatus m_WeatherStatus;

    QString getReplyData(QString attrValue,QMap<QString,QString>map);
    QString getReplyData(void);

    // 获取状态码的说明字符串
    QString getStatusCodeNote(QString statusCode);

    void disposeCityList(void);//解析城市列表

    QMap<QString, QList<CityInfo>> m_ChinaCityInfo;

    void clear();//清除呈现数据
    void display();//读取并呈现数据
};
#endif // MAINWINDOW_H
