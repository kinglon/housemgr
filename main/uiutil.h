#ifndef UIUTIL_H
#define UIUTIL_H

#include <QString>
#include <QPixmap>
#include <QLabel>

class UiUtil
{
public:
    static void showTip(QString tip);

    // 显示提示，让用户选择，继续返回true
    static bool showTipV2(QString tip);

    // 将图片等比缩放到控件大小，然后居中显示
    static void showImage(const QPixmap& image, QLabel& labelCtrl);
};

#endif // UIUTIL_H
