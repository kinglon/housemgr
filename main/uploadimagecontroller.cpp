#include "uploadimagecontroller.h"
#include <QTimer>
#include "househttpclient.h"

// 每张图片上传最大秒数
#define MAX_SECONDS_PER_IMAGE  20000

UploadImageController::UploadImageController(QObject *parent)
    : QObject{parent}
{

}

void UploadImageController::run(QWidget* parent, const QVector<QString>& imageFilePaths)
{
    m_imageFilePaths = imageFilePaths;
    if (m_imageFilePaths.empty())
    {
        emit runFinish();
        return;
    }

    // 显示进度条
    m_progressDlg = new MyProgressDialog(QString(), QString(), 0, m_imageFilePaths.size(), parent);
    m_progressDlg->setAutoReset(false);
    m_progressDlg->setAttribute(Qt::WA_DeleteOnClose);
    m_progressDlg->setWindowTitle(QString::fromWCharArray(L"提示"));
    m_progressDlg->setLabelText(QString::fromWCharArray(L"正在上传图片"));
    m_progressDlg->setValue(0);
    m_progressDlg->show();

    QTimer* progressTimer = new QTimer(m_progressDlg);
    connect(progressTimer, &QTimer::timeout, [this]() {
        if (m_progressDlg == nullptr)
        {
            return;
        }

        m_tickCount++;
        if (m_progressDlg->isFinish())
        {
            // 至少显示1秒再关闭
            if (m_tickCount >= 1)
            {
                m_progressDlg->setCanClose();
                m_progressDlg->close();
                m_progressDlg = nullptr;

                emit runFinish();
                return;
            }
        }
    });
    progressTimer->start(1000);

    // 上传图片
    doUploadImage();
}

void UploadImageController::doUploadImage()
{
    if (m_currentUploadIndex >= m_imageFilePaths.size())
    {
        if (m_progressDlg)
        {
            m_progressDlg->setFinish();
        }
        m_success = true;
        return;
    }

    HouseHttpClient* client = new HouseHttpClient(this);
    client->setTransferTimeout(MAX_SECONDS_PER_IMAGE);
    connect(client, &HouseHttpClient::addImageCompletely,
            [this, client](bool success, QString imageId) {
        if (success)
        {
            if (m_progressDlg)
            {
                m_progressDlg->setValue(m_currentUploadIndex+1);
            }
            emit uploadImageCompletely(m_imageFilePaths[m_currentUploadIndex], imageId);

            m_currentUploadIndex++;
            doUploadImage();
        }
        else
        {
            if (m_progressDlg)
            {
                m_progressDlg->setFinish();
            }
            m_success = false;
        }

        client->deleteLater();
    });
    client->addImage(m_imageFilePaths[m_currentUploadIndex]);
}
