﻿#ifndef MYPROGRESSDIALOG_H
#define MYPROGRESSDIALOG_H

#include <QProgressDialog>
#include <QCloseEvent>

class MyProgressDialog : public QProgressDialog
{
    Q_OBJECT
public:
    MyProgressDialog(const QString &labelText, const QString &cancelButtonText,
                     int minimum, int maximum, QWidget *parent = nullptr,
                     Qt::WindowFlags flags = Qt::WindowFlags());

public:
    void setCanClose() { m_canClose = true; }

    void setFinish();

    bool isFinish() { return m_finish; }

private:
    void showTip(QString tip);

private:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    bool m_canClose = false;

    // 标志进度条的任务是否完成
    bool m_finish = false;
};

#endif // MYPROGRESSDIALOG_H
