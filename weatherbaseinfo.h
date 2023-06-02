#ifndef WEATHERBASEINFO_H
#define WEATHERBASEINFO_H

#include <QObject>
#include <QJsonObject>

class weatherBaseInfo: public QObject
{
public:
    weatherBaseInfo(QObject *parent=nullptr);//构造函数
    ~weatherBaseInfo();//虚基类析构函数
    virtual void disposeWeatherInfo(QJsonObject &jsonQbject)=0;//解析天气预报虚函数
};

#endif // WEATHERBASEINFO_H
