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
};

#endif // DOWNLOADIMAGECONTROLLER_H
