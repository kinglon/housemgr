#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class ImageDialog;
}

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent, QString imageId);
    ~ImageDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateImageSize();

private:
    Ui::ImageDialog *ui;

    QLabel *m_imageLabel = nullptr;      // 用于显示图片的 QLabel

    QPixmap m_originalPixmap;  // 原始图片
};

#endif // IMAGEDIALOG_H
