#ifndef HOUSEHTTPCLIENT_H
#define HOUSEHTTPCLIENT_H

#include <QObject>
#include <QMap>
#include "datamodel.h"
#include "httpclientbase.h"

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

    // 每页显示个数
    int m_pageSize = 30;
};

class SearchResult
{
public:
    // 总数
    int m_total = 0;

    // 当前页面的数据
    QVector<House> m_houses;

public:
    int getTotalPage(int pageSize)
    {
        if (m_total <= 0)
        {
            return 0;
        }

        return (m_total-1)/pageSize + 1;
    }
};

// 不支持同时发送多个请求
class HouseHttpClient : public HttpClientBase
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

protected:
    virtual void onHttpResponse(QNetworkReply *reply) override;

private:
    void processQueryHouseResponse(QNetworkReply *reply);
    void processDeleteHouseResponse(QNetworkReply *reply);
    void processAddHouseResponse(QNetworkReply *reply);
    void processUpdateHouseResponse(QNetworkReply *reply);
    void processAddImageResponse(QNetworkReply *reply);
    void processGetImageResponse(QNetworkReply *reply);

    bool getDataObject(QNetworkReply *reply, QJsonObject& data);

    QString getQueryHouseParam(const SearchCondition& condition);
    bool parseQueryHouseData(const QJsonObject& data, SearchResult& result);
    QJsonObject getAddHouseBody(const House& house);

private:
    // reply -> interface type
    QMap<QNetworkReply*, int> m_replys;

    QString m_getImageSavePath;
};

#endif // HOUSEHTTPCLIENT_H
