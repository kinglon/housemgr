#ifndef UPLOADIMAGECONTROLLER_H
#define UPLOADIMAGECONTROLLER_H

#include <QObject>
#include "myprogressdialog.h"

class UploadImageController : public QObject
{
    Q_OBJECT
public:
    explicit UploadImageController(QObject *parent = nullptr);

public:
    void run(QWidget* parent, const QString& imageFilePath);

signals:
    void runFinish();

public:
    QString m_imageId;

    MyProgressDialog* m_progressDlg = nullptr;
};

#endif // UPLOADIMAGECONTROLLER_H
