#include "uploadimagecontroller.h"
#include <QTimer>
#include "househttpclient.h"

UploadImageController::UploadImageController(QObject *parent)
    : QObject{parent}
{

}

void UploadImageController::run(QWidget* parent, const QString& imageFilePath)
{
    // 显示进度条
    m_progressDlg = new MyProgressDialog(QString(), QString(), 0, 30, parent);
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

        int value = m_progressDlg->value();
        if (m_progressDlg->isSuccess())
        {
            // 至少显示2秒再关闭
            if (value >= 1)
            {
                m_progressDlg->setCanClose();
                m_progressDlg->close();
                m_progressDlg = nullptr;

                emit runFinish();
                return;
            }
        }
        else
        {
            if (value >= m_progressDlg->maximum())
            {

                m_progressDlg->setCanClose();
                m_progressDlg->close();
                m_progressDlg = nullptr;

                emit runFinish();
                return;
            }
        }
        m_progressDlg->setValue(value+1);
    });
    progressTimer->start(1000);

    // 上传图片
    HouseHttpClient* client = new HouseHttpClient(this);
    connect(client, &HouseHttpClient::addImageCompletely,
            [this, client](bool success, QString imageId) {
        if (m_progressDlg)
        {
            m_progressDlg->setSuccess();
        }

        if (success)
        {
            m_imageId = imageId;
        }

        client->deleteLater();
    });
    client->addImage(imageFilePath);
}
