#ifndef HOUSEHTTPCLIENT_H
#define HOUSEHTTPCLIENT_H

#include <QObject>
#include "datamodel.h"

class SearchCondition
{
public:
    // 搜索条件
    House m_searchCondition;

    // 搜索交易日期范围
    bool m_enableJiaoYiDate = false;
    QDate m_searchJiaoYiBeginDate;
    QDate m_searchJiaoYiEndDate;

    // 搜索页码，1是第一页
    int m_page = 1;
};

class SearchResult
{
public:
    int totalPage = 0;

    QVector<House> m_houses;
};

class HouseHttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HouseHttpClient(QObject *parent = nullptr);

public:
    // 房子管理
    void queryHouse(const SearchCondition& condition);
    void deleteHouse(int houseId);
    void addHouse(const House& house);
    void updateHouse(const House& house);    

    // 图片管理
    void addImage(const QString& imageFilePath);
    void getImage(const QString& imageId, const QString& imageSavePath);

signals:
    // 房子管理回调
    void queryHouseCompletely(bool success, const SearchResult& result);
    void deleteHouseCompletely(bool success);
    void addHouseCompletely(bool success, int houseId);
    void updateHouseCompletely(bool success);

    // 图片管理回调
    void addImageCompletely(bool success, QString imageId);
    void getImageCompletely(bool success);
};

#endif // HOUSEHTTPCLIENT_H
