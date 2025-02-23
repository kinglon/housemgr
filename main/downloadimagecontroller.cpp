#include "downloadimagecontroller.h"
#include "househttpclient.h"
#include "pathutil.h"

DownloadImageController::DownloadImageController(QObject *parent)
    : QObject{parent}
{

}

void DownloadImageController::run(const QVector<QString>& imageIds)
{
    m_imageIds = imageIds;
    m_currentDownloadIndex = 0;
    m_retryCount = 0;
    if (m_imageIds.size() == 0)
    {
        emit runFinish();
        return;
    }

    doDownloadImage();
}

void DownloadImageController::doDownloadImage()
{
    if (m_currentDownloadIndex >= m_imageIds.size())
    {
        emit runFinish();
        return;
    }

    HouseHttpClient* client = new HouseHttpClient(this);
    QString imageId = m_imageIds[m_currentDownloadIndex];
    connect(client, &HouseHttpClient::getImageCompletely, [this, client, imageId](bool success) {
        bool downloadNextImage = true;
        if (!success)
        {
            if (m_retryCount < 1)
            {
                // 重试1次
                m_retryCount++;
                doDownloadImage();
                downloadNextImage = false;
            }
        }
        else
        {
            emit downloadImageCompletely(imageId);
        }

        if (downloadNextImage)
        {
            m_currentDownloadIndex++;
            m_retryCount = 0;
            doDownloadImage();
        }

        client->deleteLater();
    });
    client->getImage(imageId, PathUtil::getImageFilePath(imageId));
}
