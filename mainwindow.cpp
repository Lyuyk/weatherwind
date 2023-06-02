#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QErrorMessage>
#include <QJsonParseError>
#include <QJsonArray>
#include <QtNetwork/QNetworkProxy>
#include <QUrlQuery>
#include <QMessageBox>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::setFixedSize(1600,900);
    m_PrivateKey="34fcb36bcc8a42d2b0fe9b549cce8f8c";//设置APIkey
    m_Manager=new QNetworkAccessManager(this);//设置网络
    m_TypeInterfaceMap.clear();//清除

    initData();//初始化数据
    on_pushButton_clicked();//更新天气信息
    //qDebug()<<QDir::currentPath();
    disposeCityList();//解析城市列表xml
    QList<QString> Province=getProvinceName();//获取省份名称
    //ui->comboBox_province->clear();
    ui->comboBox_province->addItems(Province);//将获取的省份名称设置在combobox显示

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*MainWindow *MainWindow::getInstance(void)
{
    static MainWindow instance;
    return &instance;
}*/

void MainWindow::initData(void)
{
    m_WeatherGetType = Weather_Forecast_Type;
    //m_LocationCity = "auto_ip";
    m_LocationCity = "新会";
    m_AirQualityCity = "江门";
    //插入各种天气信息类型
    m_TypeInterfaceMap.insert(Weather_Forecast_Type, new weatherData);
    m_TypeInterfaceMap.insert(Weather_Now_Type, new weatherInfo);
    m_TypeInterfaceMap.insert(Weather_LifeStyle_Type, new weatherLife);
    m_TypeInterfaceMap.insert(Weather_AirQuality_Type, new weatherAirNow);
}

QString MainWindow::getURLAttrStringByType(WeatherGetType type)//返回何种天气类型拼接的url
{
    if (type == Weather_Forecast_Type)
        return "weather/forecast";
    else if (type == Weather_Now_Type)
        return "weather/now";
    else if (type == Weather_LifeStyle_Type)
        return "weather/lifestyle";
    else if (type == Weather_AirQuality_Type)
        return "air/now";

    return "";
}

void MainWindow::disposeWeatherJsonData(QString jsonData)
{
    QJsonParseError jsonError;
    QByteArray byteArray = jsonData.toUtf8();
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
    if (doucment.isNull() || jsonError.error != QJsonParseError::NoError)
        return;

    // 获取JsonObject
    QJsonArray mainDataJsonArray;
    if (doucment.isObject())
    {
        QJsonObject jsonObject = doucment.object();
        if (jsonObject.contains("HeWeather6"))
        {
            QJsonValue jsonValue = jsonObject.value("HeWeather6");
            if (jsonValue.isArray())
                mainDataJsonArray = jsonValue.toArray();
            else
                return;
        }
    }

    QJsonObject mainDataJsonObject = mainDataJsonArray.at(0).toObject();

    // 解释Basic
    if (mainDataJsonObject.contains("basic"))
    {
        QJsonObject basicJsonObject = mainDataJsonObject.value("basic").toObject();
        m_WeatherBaseData.cid = basicJsonObject.value("cid").toString();
        m_WeatherBaseData.location = basicJsonObject.value("location").toString();
        m_WeatherBaseData.parent_city = basicJsonObject.value("parent_city").toString();
        m_WeatherBaseData.admin_area = basicJsonObject.value("admin_area").toString();
        m_WeatherBaseData.cnty = basicJsonObject.value("cnty").toString();
        m_WeatherBaseData.lat = basicJsonObject.value("lat").toString().toDouble();
        m_WeatherBaseData.lon = basicJsonObject.value("lon").toString().toDouble();
        m_WeatherBaseData.tz = basicJsonObject.value("tz").toString().toDouble();
    }

    // 解析更新时间
    if (mainDataJsonObject.contains("update"))
    {
        QJsonObject updateJsonObject = mainDataJsonObject.value("update").toObject();
        m_WeatherTime.localTime = QDateTime::fromString(updateJsonObject.value("loc").toString(), "yyyy-MM-dd hh:mm");
        m_WeatherTime.utcTime = QDateTime::fromString(updateJsonObject.value("utc").toString(), "yyyy-MM-dd hh:mm");
    }

    // 解析状态码
    if (mainDataJsonObject.contains("status"))
    {
        m_WeatherStatus.statusCode = mainDataJsonObject.value("status").toString();
        m_WeatherStatus.note = getStatusCodeNote(m_WeatherStatus.statusCode);
    }

    // 解析类型的信息
    m_TypeInterfaceMap[m_WeatherGetType]->disposeWeatherInfo(mainDataJsonObject);
}

QString MainWindow::getStatusCodeNote(QString statusCode)
{
    if ("ok" == statusCode)//转换状态码的意义
        return "数据正常";
    else if ("invalid key" == statusCode)
        return "错误的key，请检查你的key是否输入以及是否输入有误";
    else if ("unknown location" == statusCode)
        return "未知或错误城市/地区";
    else if ("no data for this location" == statusCode)
        return "该城市/地区没有你所请求的数据";
    else if ("no more requests" == statusCode)
        return "超过访问次数";
    else if ("param invalid" == statusCode)
        return "参数错误，请检查你传递的参数是否正确";
    else if ("too fast" == statusCode)
        return "超过限定的QPM";
    else if ("dead" == statusCode)
        return "无响应或超时";
    else if ("permission denied" == statusCode)
        return "无访问权限，你没有购买你所访问的这部分服务";
    else if ("sign error" == statusCode)
        return "签名错误";
}

void MainWindow::disposeCityList(void)//解析城市列表
{
    QFile file("./data/LocList.xml");//城市列表xml文件打开
    if (!file.open(QFile::ReadOnly))
        return;

    QDomDocument doc;
    if (!doc.setContent(&file))//用domdocument容器装入信息
    {
        file.close();
        return;
    }
    file.close();

    QDomElement rootElement = doc.documentElement();
    if (rootElement.tagName() != "Location")
        return;

    QDomElement chinaElement;
    QDomNodeList nodeLsit = rootElement.childNodes();
    for (int i=0; i<nodeLsit.count(); ++i)
    {
        QDomElement element = nodeLsit.at(i).toElement();
        if (element.attribute("Name") == "中国")
        {
            chinaElement = element;
            break;
        }
    }

    if (chinaElement.isNull())
        return;

    QDomNodeList chinaNodeList = chinaElement.childNodes();
    for (int i=0; i<chinaNodeList.count(); ++i)
    {
        // 解析省/直辖市
        QDomElement element = chinaNodeList.at(i).toElement();
        QString stateName = element.attribute("Name");
        QList<CityInfo> cityInfoList;
        cityInfoList.clear();

        // 解析城市
        QDomNodeList cityNodeList = element.childNodes();
        for (int j=0; j<cityNodeList.count(); ++j)
        {
            QDomElement cityElement = cityNodeList.at(j).toElement();
            QString cityName = cityElement.attribute("Name");

            CityInfo cityInfo;
            cityInfo.cityName = cityName;
            QList<QString> regionNameList;
            regionNameList.clear();

            // 解析区县
            QDomNodeList regionNodeList = cityElement.childNodes();
            for (int k=0; k<regionNodeList.count(); ++k)
            {
                QDomElement regionElement = regionNodeList.at(k).toElement();

                QString regionName = regionElement.attribute("Name");
                regionNameList.push_back(regionName);
            }

            cityInfo.regionName = regionNameList;
            cityInfoList.push_back(cityInfo);
        }

        m_ChinaCityInfo.insert(stateName, cityInfoList);//将信息插入容器列表
    }
}

void MainWindow::clear()//清除界面信息
{
    ui->label_location->clear();
    ui->label_tempi->clear();
    ui->label_lastUpdatei->clear();
    ui->label_weatheri->clear();

    ui->label_AQIi->clear();
    ui->label_AirNowi->clear();
    ui->label_windSpeedi->clear();
    ui->label_visualDistancei->clear();
    ui->label_Tempfli->clear();
    ui->label_pm25i->clear();

    ui->label_tempi_low->clear();
    ui->label_tempi_low_2->clear();
    ui->label_tempi_low_3->clear();


    ui->label_tempi_high->clear();
    ui->label_tempi_high_2->clear();
    ui->label_tempi_high_3->clear();


    ui->label_weatherIcon->clear();
    ui->label_weatherIcon01->clear();
    ui->label_weatherIcon02->clear();
    ui->label_weatherIcon03->clear();


    ui->label_respondCodei->clear();
    ui->label_respondCodeMeaning->clear();

    ui->label_sunrise->clear();
    ui->label_sunset->clear();
    ui->label_winDirection->clear();
    ui->label_winLevel->clear();
    ui->label_pcpn->clear();
    ui->label_hum->clear();
    ui->label_pop->clear();
    ui->label_co->clear();
    ui->label_no2->clear();
    ui->label_so2->clear();
    ui->label_pres->clear();


}

void MainWindow::display()//获取并显示界面信息
{

    QString location=m_WeatherBaseData.location+m_LocationCity;
    QString lastUpdateTime=m_WeatherTime.localTime.toString();
    int tempi=getNowWeatherInfo().tmp;
    QString weather=getNowWeatherInfo().cond_txt;

    int aqi=getAirQualityCity().aqi;
    //QColor aqiColor=getAirQualityCityColor();
    QString airCond=getAirQualityCity().qlty;
    int pm25=getAirQualityCity().pm25;
    int flTemp=getNowWeatherInfo().fl;
    int windSpeed=getNowWeatherInfo().wind_spd;
    int visualDistance=getNowWeatherInfo().vis;

    //==============================================dailyForecast
    QList<weatherData::DailyForecast> dailyForecast=getDailyForecast();
    int day1HighTmp = dailyForecast.at(0).tmp_max;
    int day2HighTmp = dailyForecast.at(1).tmp_max;
    int day3HighTmp = dailyForecast.at(2).tmp_max;
    int day1LowTmp = dailyForecast.at(0).tmp_min;
    int day2LowTmp = dailyForecast.at(1).tmp_min;
    int day3LowTmp = dailyForecast.at(2).tmp_min;
    int day1Code = dailyForecast.at(0).cond_code_d;
    int day2Code = dailyForecast.at(1).cond_code_d;
    int day3Code = dailyForecast.at(2).cond_code_d;


    //==============================================dailyForecast

    QString resCode=m_WeatherStatus.statusCode;
    QString resCodeMeaning=m_WeatherStatus.note;

    //====================================================icon
    int weatherCode=getNowWeatherInfo().cond_code;
    QString weatherIconpath=getWeatherIcon(weatherCode);
    QImage weatherIcon;
    weatherIcon.load(weatherIconpath);qDebug()<<weatherIconpath;
    ui->label_weatherIcon->setPixmap(QPixmap::fromImage(weatherIcon));

    QString weatherIconpath1=getWeatherIconSmall(day1Code);
    QImage weatherIcon1;
    weatherIcon1.load(weatherIconpath1);qDebug()<<weatherIconpath1;
    ui->label_weatherIcon01->setPixmap(QPixmap::fromImage(weatherIcon1));

    QString weatherIconpath2=getWeatherIconSmall(day2Code);
    QImage weatherIcon2;
    weatherIcon2.load(weatherIconpath2);qDebug()<<weatherIconpath2;
    ui->label_weatherIcon02->setPixmap(QPixmap::fromImage(weatherIcon2));

    QString weatherIconpath3=getWeatherIconSmall(day3Code);
    QImage weatherIcon3;
    weatherIcon3.load(weatherIconpath3);qDebug()<<weatherIconpath3;
    ui->label_weatherIcon03->setPixmap(QPixmap::fromImage(weatherIcon3));
    //====================================================icon

    ui->label_lastUpdatei->setText(lastUpdateTime);
    ui->label_location->setText(location);
    ui->label_tempi->setNum(tempi);
    ui->label_weatheri->setText(weather);


    ui->label_AQIi->setNum(aqi);

    ui->label_AirNowi->setText(airCond);
    ui->label_pm25i->setNum(pm25);
    ui->label_Tempfli->setNum(flTemp);
    ui->label_windSpeedi->setNum(windSpeed);
    ui->label_visualDistancei->setNum(visualDistance);

    ui->label_tempi_high->setNum(day1HighTmp);
    ui->label_tempi_high_2->setNum(day2HighTmp);
    ui->label_tempi_high_3->setNum(day3HighTmp);
    ui->label_tempi_low->setNum(day1LowTmp);
    ui->label_tempi_low_2->setNum(day2LowTmp);
    ui->label_tempi_low_3->setNum(day3LowTmp);

    ui->label_respondCodei->setText(resCode);
    ui->label_respondCodeMeaning->setText(resCodeMeaning);

    //====================================================rightside bar

    QTime sunrise=dailyForecast.at(0).sr;
    QTime sunset=dailyForecast.at(0).ss;
    QString winDir=dailyForecast.at(0).wind_dir;
    QString winSc=dailyForecast.at(0).wind_sc;
    int hum=dailyForecast.at(0).hum;
    int pcpn=dailyForecast.at(0).pcpn;
    int pop=dailyForecast.at(0).pop;
    int pres=dailyForecast.at(0).pres;
    int uvIndex=dailyForecast.at(0).uv_index;

    float no2=getAirQualityCity().no2;
    float so2=getAirQualityCity().so2;
    float co=getAirQualityCity().co;

    ui->label_sunrise->setText(sunrise.toString());
    ui->label_sunset->setText(sunset.toString());
    ui->label_winDirection->setText(winDir);
    ui->label_winLevel->setText(winSc);
    ui->label_hum->setNum(hum);
    ui->label_pcpn->setNum(pcpn);
    ui->label_uvIndex->setNum(uvIndex);
    ui->label_so2->setNum(so2);
    ui->label_co->setNum(co);
    ui->label_pres->setNum(pres);
    ui->label_pop->setNum(pop);
    ui->label_no2->setNum(no2);


}

void MainWindow::setConfigNetWorkProxy(bool visible)//设置网络代理
{
    m_IsConfigNetWorkProxy = visible;

    if (m_IsConfigNetWorkProxy)
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);//设置代理模式
        proxy.setHostName("127.0.0.1");//设置host
        proxy.setPort(8888);//设置端口
        m_Manager->setProxy(proxy);
    }
    else
        m_Manager->setProxy(QNetworkProxy::NoProxy);//不设置代理
}

void MainWindow::updateData(void)//获取更新json信息
{
    QString jsonData = getReplyData();
    disposeWeatherJsonData(jsonData);
    //qDebug()<<jsonData;
    qDebug()<<"=================================================";
}

void MainWindow::setCurrentWeatherInfoType(WeatherGetType type)//返回当前天气类型
{
    m_WeatherGetType = type;
}

void MainWindow::setCurrentCityName(QString cityName)//设置当前城市名称
{
    m_LocationCity = cityName;
}

void MainWindow::setAirQualityCityName(QString cityName)//设置当谬空气质量城市名称
{
    m_AirQualityCity = cityName;
}

QString MainWindow::getCurrentCityName(void)//获取当前城市名称
{
    return m_LocationCity;
}

// 获取天气的基础信息
MainWindow::WeatherBaseData MainWindow::getBaseData(void)
{
    return m_WeatherBaseData;
}

// 获取更新时间信息
MainWindow::WeatherUpdateTime MainWindow::getUpdateTime(void)
{
    return m_WeatherTime;
}

QList<weatherData::DailyForecast> MainWindow::getDailyForecast(void)//获取当前天气预报列表
{
    weatherData *pWeatherForecast = dynamic_cast<weatherData*>(m_TypeInterfaceMap[Weather_Forecast_Type]);
    if (pWeatherForecast)
        return pWeatherForecast->getDailyForecast();
}

weatherInfo::WeatherNowInfo MainWindow::getNowWeatherInfo(void)//获取当前天气预报
{
    weatherInfo *pWeatherNow = dynamic_cast<weatherInfo*>(m_TypeInterfaceMap[Weather_Now_Type]);
    if (pWeatherNow)
        return pWeatherNow->getNowWeatherInfo();
}

QList<weatherLife::LifeStyleData> MainWindow::getLifeStyleInfoList(void)//获取当前生活指数
{
    weatherLife *pWeatherLifeStyle = dynamic_cast<weatherLife*>(m_TypeInterfaceMap[Weather_LifeStyle_Type]);
    if (pWeatherLifeStyle)
        return pWeatherLifeStyle->getLifeStyleInfoList();
}

weatherAirNow::AirQualityCity MainWindow::getAirQualityCity(void)//获取当前空气质量
{
    weatherAirNow *pWeatherAirNow = dynamic_cast<weatherAirNow*>(m_TypeInterfaceMap[Weather_AirQuality_Type]);
    if (pWeatherAirNow)
        return pWeatherAirNow->getAirQualityCity();
}

QColor MainWindow::getAirQualityCityColor(void)//获取城市空气质量代表的颜色
{
    QColor color = QColor(255, 255, 255);
    weatherAirNow *pWeatherAirNow = dynamic_cast<weatherAirNow*>(m_TypeInterfaceMap[Weather_AirQuality_Type]);
    if (pWeatherAirNow)
    {
        int nAqi = pWeatherAirNow->getAirQualityCity().aqi;
        if (nAqi <= 100)
            color = QColor(0, 255, 0);
        else if (nAqi <= 200)
            color = QColor(230, 230, 0);
        else
            color = QColor(255, 0, 0);
    }

    return color;
}

void MainWindow::updateWeatherInfo(void)
{
    // 天气预报
    setCurrentWeatherInfoType(Weather_Forecast_Type);
    updateData();
    // 实时天气
    setCurrentWeatherInfoType(Weather_Now_Type);
    updateData();
    // 生活指数
    setCurrentWeatherInfoType(Weather_LifeStyle_Type);
    updateData();
    // 空气质量
    setCurrentWeatherInfoType(Weather_AirQuality_Type);
    updateData();
}

QString MainWindow::getWeatherIcon(int code)//获取天气状态码对应的天气图标路径
{
    QString weatherIconString = "./Icons/weather-icon/color-128/%1.png";
    weatherIconString = weatherIconString.arg(code);
    qDebug()<<weatherIconString<<"(code:)"<<code;
    return weatherIconString;
}

QString MainWindow::getWeatherIconSmall(int code)//获取天气状态码对应的天气图标路径
{
    QString weatherIconString = "./Icons/weather-icon/color-64/%1.png";
    weatherIconString = weatherIconString.arg(code);
    qDebug()<<weatherIconString<<"(code:)"<<code;
    return weatherIconString;
}

QList<QString> MainWindow::getProvinceName(void)//获取省份信息
{
    QList<QString> provinceNameList;//容器列表
    provinceNameList.clear();
    for (auto itor = m_ChinaCityInfo.begin(); itor != m_ChinaCityInfo.end(); ++itor)
    {
        provinceNameList.push_back(itor.key());//获取对应城市到list
    }
    return provinceNameList;
}

QList<QString> MainWindow::getCitysName(QString name)//获取城市名称
{
    auto itor = m_ChinaCityInfo.find(name);
     QList<CityInfo> cityList = itor.value();
    QList<QString> cityNameList;
    cityNameList.clear();
    for (int i=0; i<cityList.count(); ++i)
    {
        cityNameList.push_back(cityList.at(i).cityName);
    }

    return cityNameList;
}

QList<QString> MainWindow::getRegionsName(QString stateName, QString cityName)//获取省份名称
{
    auto itor = m_ChinaCityInfo.find(stateName);
    QList<CityInfo> cityList = itor.value();
    QList<QString> regionNameList;
    regionNameList.clear();
    for (int i=0; i<cityList.count(); ++i)
    {
        if (cityList.at(i).cityName == cityName)
            return cityList.at(i).regionName;
    }

    return regionNameList;
}

QString MainWindow::getReplyData(QString attrValue, QMap<QString, QString> map)//获取返回json信息，并返回返回状态
{
    if (map.count() <= 0)
        return "";

    QString baseUrlString = WEATHER_INTERFACE_BASEURL;
    baseUrlString += attrValue;
    //qDebug()<<attrValue<<(attrValue);
    // 设置URL
    QUrl url(baseUrlString);
    QUrlQuery query;
    query.addQueryItem("key", m_PrivateKey);
    for (auto itor = map.begin(); itor != map.end(); ++itor)
    {
        QString key = itor.key();
        QString value = itor.value();

        query.addQueryItem(key, value);
    }
    qDebug()<<url<<"(url0)";

    url.setQuery(query);qDebug()<<url<<"(url2)";
    //qDebug() <<url.toString()<<"(urlToString)";

    // 设置请求
    QNetworkRequest requset;
    requset.setUrl(url);

    // 发送get请求
    QNetworkReply *pReplay = m_Manager->get(requset);

    // 等待请求完成
    QEventLoop loop;
    QObject::connect(m_Manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString replyString = QString(pReplay->readAll().data());
    return replyString;
}

QString MainWindow::getReplyData(void)//获取返回的信息
{
    QString attrValue = getURLAttrStringByType(m_WeatherGetType);

    QMap<QString, QString> map;
    map.clear();
    if (Weather_AirQuality_Type != m_WeatherGetType)
        map.insert("location", m_LocationCity);
    else
        map.insert("location", m_AirQualityCity);

    return getReplyData(attrValue, map);
}

void MainWindow::on_pushButton_clicked()//刷新天气操作
{
    clear();//清除界面
    updateWeatherInfo();
    display();//更新显示界面
}

void MainWindow::on_commandLinkButton_aboutUs_clicked()
{
    QMessageBox::about(this,u8"关于我们",u8"本软件基于QtCreator开发\n开发者：Lui");
}



void MainWindow::on_comboBox_province_currentIndexChanged(const QString &arg1)
{
    qDebug()<<arg1<<"(currentIndex)";
}

void MainWindow::on_comboBox_city_currentIndexChanged(const QString &arg1)//当选择了城市后区县combobox列表的变更
{
    qDebug()<<arg1<<"(currentIndexCity)";
    QString Province = ui->comboBox_province->currentText();
    QList<QString> Region=getRegionsName(Province,arg1);
    qDebug()<<arg1;
    ui->comboBox_region->clear();
    ui->comboBox_region->addItems(Region);
}

void MainWindow::on_comboBox_province_currentTextChanged(const QString &arg1)//当选择了省份后城市combobox列表的变更
{
    QList<QString> City=getCitysName(arg1);
    qDebug()<<arg1<<"(currentText)";
    ui->comboBox_city->clear();
    ui->comboBox_city->addItems(City);
}

void MainWindow::on_pushButton_changeCity_clicked()//当更换城市时的操作
{
    QString Province=ui->comboBox_province->currentText();//获取省份名称
    QString City=ui->comboBox_city->currentText();//获取城市名称
    QString Region=ui->comboBox_region->currentText();//获取区县名称
    if(ui->comboBox_region->currentText()=="")
    {
        m_LocationCity=City;
        m_AirQualityCity=Province;
        on_pushButton_clicked();//更新信息
    }
    else
    {
        m_LocationCity=Region;
        m_AirQualityCity=City;
        on_pushButton_clicked();//更新信息
    }
}
