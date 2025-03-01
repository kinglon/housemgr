#ifndef UPLOADIMAGECONTROLLER_H
#define UPLOADIMAGECONTROLLER_H

#include <QObject>
#include <QVector>
#include "myprogressdialog.h"

class UploadImageController : public QObject
{
    Q_OBJECT
public:
    explicit UploadImageController(QObject *parent = nullptr);

public:
    void run(QWidget* parent, const QVector<QString>& imageFilePaths);

signals:
    void uploadImageCompletely(QString imageFilePath, QString imageId);

    void runFinish();

private:
    void doUploadImage();

public:
    bool m_success = false;

private:
    QVector<QString> m_imageFilePaths;

    int m_currentUploadIndex = 0;

    MyProgressDialog* m_progressDlg = nullptr;

    int m_tickCount = 0;
};

#endif // UPLOADIMAGECONTROLLER_H
