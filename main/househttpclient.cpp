#include "househttpclient.h"
#include "settingmanager.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHttpMultiPart>
#include <QFile>
#include <QFileInfo>

#define INTERFACE_UNKNOWN       0
#define INTERFACE_CREATE_HOUSE  1
#define INTERFACE_DELETE_HOUSE  2
#define INTERFACE_UPDATE_HOUSE  3
#define INTERFACE_QUERY_HOUSE   4
#define INTERFACE_CREATE_IMAGE  5
#define INTERFACE_GET_IMAGE     6


HouseHttpClient::HouseHttpClient(QObject *parent)
    : HttpClientBase{parent}
{

}

void HouseHttpClient::queryHouse(const SearchCondition& condition)
{
    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/house/list?" + getQueryHouseParam(condition);
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);    
    QNetworkReply* reply = m_networkAccessManager->get(request);
    if (reply)
    {
        qInfo("send the request of querying house");
        m_replys[reply] = INTERFACE_QUERY_HOUSE;
    }
}

void HouseHttpClient::deleteHouse(int houseId)
{
    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/house/" + QString::number(houseId);
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);
    QNetworkReply* reply = m_networkAccessManager->post(request, QByteArray());
    if (reply)
    {
        qInfo("send the request of deleting house: %d", houseId);
        m_replys[reply] = INTERFACE_DELETE_HOUSE;
    }
}

void HouseHttpClient::addHouse(const House& house)
{
    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/house";
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);
    QJsonObject body = getAddHouseBody(house);
    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument(body).toJson());
    if (reply)
    {
        qInfo("send the request of adding house");
        m_replys[reply] = INTERFACE_CREATE_HOUSE;
    }
}

void HouseHttpClient::updateHouse(const House& house)
{
    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/house/" + house.m_houseId;
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);
    QJsonObject body = getAddHouseBody(house);
    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument(body).toJson());
    if (reply)
    {
        qInfo("send the request of updating house");
        m_replys[reply] = INTERFACE_UPDATE_HOUSE;
    }
}

void HouseHttpClient::addImage(const QString& imageFilePath)
{
    QFile *file = new QFile(imageFilePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        qCritical("failed to open image: %s", imageFilePath.toStdString().c_str());
        emit addImageCompletely(false, "");
        return;
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(imageFilePath).fileName() + "\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setBodyDevice(file);
    multiPart->append(filePart);
    file->setParent(multiPart); // 设置父对象为 multiPart，以便在 multiPart 销毁时自动释放 file

    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/app/image";
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);

    QNetworkReply* reply = m_networkAccessManager->post(request, multiPart);
    if (reply)
    {
        qInfo("send the request of adding image");
        m_replys[reply] = INTERFACE_CREATE_IMAGE;
        multiPart->setParent(reply); // 设置父对象为 reply，以便在 reply 销毁时自动释放 multiPart
    }
    else
    {
        multiPart->deleteLater();
    }
}

void HouseHttpClient::getImage(const QString& imageId, const QString& imageSavePath)
{
    if (!m_getImageSavePath.isEmpty())
    {
        qCritical("it is getting image");
        emit getImageCompletely(false);
        return;
    }
    m_getImageSavePath = imageSavePath;

    QNetworkRequest request;
    QString urlString = SettingManager::getInstance()->m_host + "/app/image/" + imageId;
    QUrl url(urlString);
    request.setUrl(url);
    addCommonHeader(request);

    QNetworkReply* reply = m_networkAccessManager->get(request);
    if (reply)
    {
        qInfo("send the request of getting image: %s", imageId.toStdString().c_str());
        m_replys[reply] = INTERFACE_GET_IMAGE;
    }
}

void HouseHttpClient::onHttpResponse(QNetworkReply *reply)
{
    // 获取接口类型
    int type = INTERFACE_UNKNOWN;
    for (auto it=m_replys.begin(); it!=m_replys.end(); it++)
    {
        if (it.key() == reply)
        {
            type = it.value();
            m_replys.erase(it);
            break;
        }
    }

    if (type == INTERFACE_UNKNOWN)
    {
        qCritical("invalid request");
        return;
    }

    // 处理响应
    if (type == INTERFACE_QUERY_HOUSE)
    {
        processQueryHouseResponse(reply);
    }
    else if (type == INTERFACE_CREATE_HOUSE)
    {
        processAddHouseResponse(reply);
    }
    else if (type == INTERFACE_DELETE_HOUSE)
    {
        processDeleteHouseResponse(reply);
    }
    else if (type == INTERFACE_UPDATE_HOUSE)
    {
        processUpdateHouseResponse(reply);
    }
    else if (type == INTERFACE_CREATE_IMAGE)
    {
        processAddImageResponse(reply);
    }
    else if (type == INTERFACE_GET_IMAGE)
    {
        processGetImageResponse(reply);
    }
}

void HouseHttpClient::processQueryHouseResponse(QNetworkReply *reply)
{
    QJsonObject data;
    if (!getDataObject(reply, data))
    {
        qCritical("failed to send the request of querying house");
        emit queryHouseCompletely(false, SearchResult());
        return;
    }

    SearchResult searchResult;
    if (!parseQueryHouseData(data, searchResult))
    {
        emit queryHouseCompletely(false, SearchResult());
        return;
    }
    else
    {
        qInfo("query house completely, total is %d", searchResult.m_total);
        emit queryHouseCompletely(true, searchResult);
        return;
    }
}

void HouseHttpClient::processDeleteHouseResponse(QNetworkReply *reply)
{
    QJsonObject data;
    if (!getDataObject(reply, data))
    {
        qCritical("failed to send the request of deleting house");
        emit deleteHouseCompletely(false);
        return;
    }
    else
    {
        qInfo("delete house completely");
        emit deleteHouseCompletely(true);
        return;
    }
}

void HouseHttpClient::processAddHouseResponse(QNetworkReply *reply)
{
    QJsonObject data;
    if (!getDataObject(reply, data))
    {
        qCritical("failed to send the request of adding house");
        emit addHouseCompletely(false, -1);
        return;
    }
    else
    {
        qInfo("add house completely");
        emit addHouseCompletely(true, data["house_id"].toInt());
        return;
    }
}

void HouseHttpClient::processUpdateHouseResponse(QNetworkReply *reply)
{
    QJsonObject data;
    if (!getDataObject(reply, data))
    {
        qCritical("failed to send the request of updating house");
        emit updateHouseCompletely(false);
        return;
    }
    else
    {
        qInfo("update house completely");
        emit updateHouseCompletely(true);
        return;
    }
}

void HouseHttpClient::processAddImageResponse(QNetworkReply *reply)
{
    QJsonObject data;
    if (!getDataObject(reply, data))
    {
        qCritical("failed to send the request of adding image");
        emit addImageCompletely(false, "");
        return;
    }
    else
    {
        qInfo("add image completely");
        emit addImageCompletely(true, data["image_id"].toString());
        return;
    }
}

void HouseHttpClient::processGetImageResponse(QNetworkReply *reply)
{
    QString imageFilePath = m_getImageSavePath;
    m_getImageSavePath = "";

    if (reply->error() != QNetworkReply::NoError)
    {
        qCritical("failed to get image, error: %d", reply->error());
        emit getImageCompletely(false);
        return;
    }

    QByteArray data = reply->readAll();
    if (data.isEmpty())
    {
        qCritical("failed to get image, error: not have data");
        emit getImageCompletely(false);
        return;
    }

    QFile file(imageFilePath);
    if (!file.open(QFile::ReadOnly))
    {
        qCritical("failed to open the image save path: %s", imageFilePath.toStdString().c_str());
        emit getImageCompletely(false);
        return;
    }

    file.write(data);
    file.close();

    qInfo("get image completely");
    emit getImageCompletely(true);
}

bool HouseHttpClient::getDataObject(QNetworkReply *reply, QJsonObject& dataObj)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qCritical("failed to send request, error: %d", reply->error());
        return false;
    }

    QByteArray data = reply->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if (jsonDocument.isNull() || jsonDocument.isEmpty())
    {
        qCritical("response data is not valid json");
        return false;
    }

    QJsonObject root = jsonDocument.object();
    if (root.contains("code") && root["code"].toInt() == 200)
    {
        if (root.contains("data"))
        {
            dataObj = root["data"].toObject();
        }
        return true;
    }
    else
    {
        QString message = root["message"].toString();
        qCritical("response error: %s", message.toStdString().c_str());
        return false;
    }
}

QString HouseHttpClient::getQueryHouseParam(const SearchCondition& condition)
{
    const House& house = condition.m_searchCondition;
    QMap<QString, QString> queryParams;
    if (house.m_houseId >= 0)
    {
        queryParams["house_id"] = QString::number(house.m_houseId);
    }
    queryParams["house_name"] = house.m_name;
    queryParams["jiaoshui_name"] = house.m_jiaoShuiName;
    queryParams["zhongjie_name"] = house.m_zhongjieName;
    queryParams["buyer_name"] = house.m_buyer.m_name;
    queryParams["buyer_phone_number"] = house.m_buyer.m_phoneNumber;
    queryParams["buyer_id"] = house.m_buyer.m_id;
    queryParams["seller_name"] = house.m_seller.m_name;
    queryParams["seller_phone_number"] = house.m_seller.m_phoneNumber;
    queryParams["seller_id"] = house.m_seller.m_id;

    if (condition.m_enableJiaoYiDate)
    {
        QDateTime begin;
        begin.setDate(condition.m_searchJiaoYiBeginDate);
        queryParams["jiaoyi_at_begin"] = QString::number(begin.toSecsSinceEpoch());

        QDateTime end;
        end.setDate(condition.m_searchJiaoYiEndDate);
        queryParams["jiaoyi_at_end"] = QString::number(end.toSecsSinceEpoch());
    }

    queryParams["page"] = QString::number(condition.m_page);
    queryParams["page_size"] = QString::number(condition.m_pageSize);

    QString query;
    for (auto it = queryParams.begin(); it != queryParams.end(); it++)
    {
        if (!it.value().isEmpty())
        {
            if (!query.isEmpty())
            {
                query += "&";
            }
            query += (it.key() + "=" + it.value());
        }
    }

    qInfo("make the query house param: %s", query.toStdString().c_str());
    return query;
}

bool HouseHttpClient::parseQueryHouseData(const QJsonObject& data, SearchResult& result)
{
    if (!data.contains("total") || !data.contains("list"))
    {
        qCritical("the query house data miss total or list field");
        return false;
    }

    result.m_total = data["total"].toInt();
    QJsonArray houses =  data["list"].toArray();
    for (int i=0; i<houses.size(); i++)
    {
        QJsonObject houseObj = houses[i].toObject();
        House house;
        house.m_houseId = houseObj["house_id"].toInt();
        house.m_name = houseObj["house_name"].toString();
        house.m_jiaoShuiName = houseObj["jiaoshui_name"].toString();
        house.m_fee = houseObj["fee"].toString();
        house.m_zhongjieName = houseObj["zhongjie_name"].toString();
        house.m_jiaoYiDate = QDateTime::fromSecsSinceEpoch(houseObj["jiaoyi_at"].toInt()).date();

        QJsonArray images = houseObj["fangchanzheng_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_fangChanZhengImages.append(images[i].toString());
        }

        images = houseObj["shuipiao_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_shuiPiaoImages.append(images[i].toString());
        }

        images = houseObj["tongzhishu_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_tongZhiShuImages.append(images[i].toString());
        }

        house.m_buyer.m_name = houseObj["buyer_name"].toString();
        house.m_buyer.m_phoneNumber = houseObj["buyer_phone_number"].toString();
        house.m_buyer.m_id = houseObj["buyer_id"].toString();

        images = houseObj["buyer_hukou_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_buyer.m_hukouImages.append(images[i].toString());
        }

        images = houseObj["buyer_id_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_buyer.m_idImages.append(images[i].toString());
        }

        images = houseObj["buyer_marriage_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_buyer.m_marriageImages.append(images[i].toString());
        }

        house.m_seller.m_name = houseObj["seller_name"].toString();
        house.m_seller.m_phoneNumber = houseObj["seller_phone_number"].toString();
        house.m_seller.m_id = houseObj["seller_id"].toString();

        images = houseObj["seller_hukou_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_seller.m_hukouImages.append(images[i].toString());
        }

        images = houseObj["seller_id_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_seller.m_idImages.append(images[i].toString());
        }

        images = houseObj["seller_marriage_images"].toArray();
        for (int i=0; i<images.size(); i++)
        {
            house.m_seller.m_marriageImages.append(images[i].toString());
        }
    }

    return true;
}

QJsonObject HouseHttpClient::getAddHouseBody(const House& house)
{
    QJsonObject body;
    body["house_name"] = house.m_name;
    body["jiaoshui_name"] = house.m_jiaoShuiName;
    body["fee"] = house.m_fee;
    body["zhongjie_name"] = house.m_zhongjieName;
    QDateTime jiaoYiDate;
    jiaoYiDate.setDate(house.m_jiaoYiDate);
    body["jiaoyi_at"] = jiaoYiDate.toSecsSinceEpoch();

    QJsonArray fangChanZhengImages;
    for(const auto& image_id : house.m_fangChanZhengImages)
    {
        fangChanZhengImages.append(image_id);
    }
    body["fangchanzheng_images"] = fangChanZhengImages;

    QJsonArray shuiPiaoImages;
    for(const auto& image_id : house.m_shuiPiaoImages)
    {
        shuiPiaoImages.append(image_id);
    }
    body["shuipiao_images"] = shuiPiaoImages;

    QJsonArray tongZhiShuImages;
    for(const auto& image_id : house.m_tongZhiShuImages)
    {
        tongZhiShuImages.append(image_id);
    }
    body["tongzhishu_images"] = tongZhiShuImages;

    body["buyer_name"] = house.m_buyer.m_name;
    body["buyer_phone_number"] = house.m_buyer.m_phoneNumber;
    body["buyer_id"] = house.m_buyer.m_id;

    QJsonArray buyerIdImages;
    for(const auto& image_id : house.m_buyer.m_idImages)
    {
        buyerIdImages.append(image_id);
    }
    body["buyer_id_images"] = buyerIdImages;

    QJsonArray buyerHuKouImages;
    for(const auto& image_id : house.m_buyer.m_hukouImages)
    {
        buyerHuKouImages.append(image_id);
    }
    body["buyer_hukou_images"] = buyerHuKouImages;

    QJsonArray buyerMarriageImages;
    for(const auto& image_id : house.m_buyer.m_marriageImages)
    {
        buyerMarriageImages.append(image_id);
    }
    body["buyer_marriage_images"] = buyerMarriageImages;

    body["seller_name"] = house.m_seller.m_name;
    body["seller_phone_number"] = house.m_seller.m_phoneNumber;
    body["seller_id"] = house.m_seller.m_id;

    QJsonArray sellerIdImages;
    for(const auto& image_id : house.m_seller.m_idImages)
    {
        sellerIdImages.append(image_id);
    }
    body["seller_id_images"] = sellerIdImages;

    QJsonArray sellerHuKouImages;
    for(const auto& image_id : house.m_seller.m_hukouImages)
    {
        sellerHuKouImages.append(image_id);
    }
    body["seller_hukou_images"] = sellerHuKouImages;

    QJsonArray sellerMarriageImages;
    for(const auto& image_id : house.m_seller.m_marriageImages)
    {
        sellerMarriageImages.append(image_id);
    }
    body["seller_marriage_images"] = sellerMarriageImages;

    return body;
}
