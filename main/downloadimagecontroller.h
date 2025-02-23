#ifndef DOWNLOADIMAGECONTROLLER_H
#define DOWNLOADIMAGECONTROLLER_H

#include <QObject>

class DownloadImageController : public QObject
{
    Q_OBJECT
public:
    explicit DownloadImageController(QObject *parent = nullptr);

public:
    void run(const QVector<QString>& imageIds);

signals:
    void downloadImageCompletely(QString imageId);

    void runFinish();

private:
    void doDownloadImage();

private:
    QVector<QString> m_imageIds;

    // 当前下载的索引
    int m_currentDownloadIndex = 0;

    // 重试次数
    int m_retryCount = 0;
};

#endif // DOWNLOADIMAGECONTROLLER_H
