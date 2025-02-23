#include "imagedialog.h"
#include "ui_imagedialog.h"
#include "pathutil.h"

ImageDialog::ImageDialog(QWidget *parent, QString imageId) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::WindowModal);

    m_imageLabel = new QLabel(this);

    m_originalPixmap.load(PathUtil::getImageFilePath(imageId));
    updateImageSize(); // 初始化图片大小
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::updateImageSize()
{
    if (!m_originalPixmap.isNull())
    {
        // 获取客户区大小（不包含标题栏）
        QSize clientSize = contentsRect().size();

        // 缩放图片以适应客户区大小，同时保持比例
        m_imageLabel->setPixmap(m_originalPixmap.scaled(clientSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ImageDialog::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImageSize(); // 窗口大小变化时更新图片大小
}
