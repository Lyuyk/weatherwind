# Weatherwind

基于Qt开发的天气预报软件



## 设计

由于是开发一个天气预报软件，首先最麻烦的部分不在于界面的设计，而是在于如何处理API请求后发送回来的数据，如何合理地解析API发送回来的数据，决定了可以如何更好地把数据以可视化的方式呈现。软件的设计思路比较简单，打开软件界面的时候，用户向软件提交一个需要查询天气的城市，然后天气预报软件向设定好的指定API发送查询天气请求。然后接收API返回的信息文件，将接收到的返回的信息文件经过解析后，将解析得到的信息呈现在软件界面上，从而达到天气应用软件的作用。

 

## 类具体实现

### weatherBaseInfo类

该类是一个抽象类，作为weatherInfo类、weatherData类、weatherLife类、weatherAirNow类的基类，这四个类是存储API返回的数据的类。

### weatherInfo类

继承自weatherBaseInfo类，包含disposeWeatherInfo（）和getNowWeatherInfo（）公有函数，以及WeatherNowInfo结构体。

WeatherNowInfo结构体包含当日天气细节的各项参数，如体感温度、温度、风力、风速、相对湿度、降水量的参数；getNowWeatherInfo（）函数返回weatherInfo（即上面介绍到的）的数据；disposeWeatherInfo（）函数对返回存储在jsonObjects的json数据进行解析，并存放在weatherInfo结构体中。

 

### weatherData类

继承自weatherBaseInfo类，内含DailyForecast结构体，getDailyForecast（）和disposeWeatherInfo（）公有函数，私有成员QList<DailyForecast> m_dailyForecastList。

DailyForecast结构体内包含一天气温、天气、风力风向、相对湿度、日月出落、紫外线、能见度等天气数据；getDailyForecast（）函数返回天气数据（存储在结构体中）；disposeWeatherInfo（）获取jsonObjects中存放的获取到的天气数据并将其解析后存放在私有数据成员m_dailyForecastList中。

 

### weatherLife类

该类继承自weatherBaseInfo类,包含convertToString, getLifeStyleInfoList（）和disposeWeatherInfo（）三个公有函数，一个LifeStyleData结构体，LifeStyleType枚举变量，私有成员有QList容器存放的lifeStyleData的数据，私有函数有lifeStyleStringToType（）将

convertToString（）函数将指定的枚举类型转换为字符串，lifeStyleStringToType（）函数将指定的数字符串转换为枚举类型，getLifeStyleInfoList（）函数获取返回生活指数列表，存储数据在私有成员QList容器中，结构体LifeStyleData存储枚举变量中代表的各种指数的枚举类型及其名称，LiftStyleType枚举变量列举了各种生活指数数据。

 

### weatherAirNow类

继承自weatherBaseInfo类，包含disposeWeatherInfo和getAirQualityCity公有函数，以及AirQualityCity结构体，其亦作为该类的私有成员。

AirQualityCity结构体包含当日各项空气指数数据，如AQI，主要污染物，空气质量，pm10，pm2.5,二氧化氮,二氧化硫,一氧化碳,臭氧含量的数据;getAirQualityCity函数返回AirQualityCity的数据;disposeWeatherInfo函数对返回存储在jsonObjects的json数据进行解析,并存放在AirQualityCity结构体的类的的私有成员中。

 

### mainWindow类

这个类是软件中最关键的类，是主窗口的类。

包含WeatherGetType枚举变量（记录天气信息类型），WeatherBaseData（记录天气基础信息）、WeatherUpdateTime（记录天气更新时间）、WeatherStatus（记录更新状态码）、CityInfo（记录城市及城市下区的名字）结构体；函数有setConfigNetWorkProxy（），updateData（），setCurrentWeatherInfoType（），setCurrentCityName（），getCurrentCityName（），setAirQualityCityName（），getBaseData（），getUpdateTime（），getDailyForecast（），getNowWeatherInfo（），getLifeStyleInfoList（），getAirQualityCity（），getAirQualityCityColor（），updateWeatherInfo（），getWeatherIcon（），getWeatherIconSmall（），getProvinceName（），getCitysName（），getRegionsName（）；私有函数有initData（），getURLAttrStringByType（），disposeWeatherJsonData（），getReplyData（），getStatusCodeNote（），disposeCityList（），clear（），display（）；

函数的作用

| 名称                          | 作用                      |
| :---------------------------- | :------------------------ |
| setConfigNetWorkProxy（）     | 设置是否设置代理          |
| updateData（）                | 更新当前天气信息          |
| setCurrentWeatherInfoType（） | 设置当前信息类型          |
| setCurrentCityName（）        | 设置当前城市              |
| getCurrentCityName（）        | 获取当前城市              |
| setAirQualityCityName（）     | 设置空气质量检测城市      |
| getBaseData（）               | 获取天气基础信息          |
| getUpdateTime（）             | 获取更新时间信息          |
| getDailyForecast（）          | 获取天气预报信息          |
| getNowWeatherInfo（）         | 获取当时天气信息          |
| getLifeStyleInfoList（）      | 获取当前生活指数          |
| getAirQualityCity（）         | 获取当前空气质量          |
| getAirQualityCityColor（）    | 获取当前空气质量颜色      |
| updateWeatherInfo（）         | 刷新天气信息              |
| getWeatherIcon（）            | 获取天气图标              |
| getWeatherIconSmall（）       | 获取天气图标              |
| getProvinceName（）           | 获取所有的省              |
| getCitysName（）              | 获取某个省下的所有城市    |
| getRegionsName（）            | 获取某个市下的所有县      |
| initData（）                  | 初始化数据                |
| getURLAttrStringByType（）    | 根据类型获取URL中的缺省值 |
| disposeWeatherJsonData（）    | 解析json                  |
| getReplyData（）              | 获取返回信息              |
| getStatusCodeNote（）         | 获取状态码说明字符串      |
| disposeCityList（）           | 解析城市列表              |
| clear（）                     | 清空呈现的天气信息        |
| display（）                   | 读取并呈现天气信息        |



 

私有成员及作用：

| 成员名称                                                     | 作用                 |      |
| :----------------------------------------------------------- | :------------------- | :--- |
| QString m_PrivateKey（API key）QNetworkAccessManager *m_Manager | 网络配置             |      |
| bool m_IsConfigNetWorkProxy = false                          | 是否设置代理         |      |
| WeatherGetType m_WeatherGetType                              | 要获取的天气类型     |      |
| QMap<WeatherGetType, weatherBaseInfo*> m_TypeInterfaceMap    | 天气类型-类列表      |      |
| QString m_LocationCity;                                      | 当前的城市信息       |      |
| QString m_AirQualityCity;                                    | 当前空气质量检测城市 |      |
| WeatherBaseData m_WeatherBaseData                            | 天气的基础信息       |      |
| WeatherUpdateTime m_WeatherTime                              | 更新时间信息         |      |
| WeatherStatus m_WeatherStatus                                | 状态码信息           |      |
| QMap<QString, QList<CityInfo>> m_ChinaCityInfo               | 城市信息列表         |      |
|                                                              |                      |      |


