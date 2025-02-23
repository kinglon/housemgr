#include "housedialog.h"
#include "ui_housedialog.h"
#include "househttpclient.h"
#include <QPixmap>
#include <Windows.h>
#include "uiutil.h"
#include "pathutil.h"
#include <QFile>
#include <QContextMenuEvent>
#include <QMenu>
#include <QFileDialog>
#include "uploadimagecontroller.h"
#include "downloadimagecontroller.h"
#include "imagedialog.h"

HouseDialog::HouseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HouseDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::WindowModal);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    m_imageListWidgets.append(ui->fangChanZhengListWidget);
    m_imageListWidgets.append(ui->shuiPiaoListWidget);
    m_imageListWidgets.append(ui->tongZhiShuListWidget);
    m_imageListWidgets.append(ui->buyerHuKouListWidget);
    m_imageListWidgets.append(ui->buyerIdListWidget);
    m_imageListWidgets.append(ui->buyerMarriageListWidget);
    m_imageListWidgets.append(ui->sellerHuKouListWidget);
    m_imageListWidgets.append(ui->sellerIdListWidget);
    m_imageListWidgets.append(ui->sellerMarriageListWidget);

    initCtrls();
    initActions();
    downloadImages();
}

HouseDialog::~HouseDialog()
{
    delete ui;
}

void HouseDialog::initCtrls()
{
    ui->houseNameEdit->setText("");
    ui->jiaoShuiEdit->setText("");
    ui->zhongJieEdit->setText("");
    ui->feeEdit->setText("");
    ui->buyerNameEdit->setText("");
    ui->buyerIdEdit->setText("");
    ui->buyerPhoneEdit->setText("");
    ui->sellerNameEdit->setText("");
    ui->sellerPhoneEdit->setText("");
    ui->sellerIdEdit->setText("");
    ui->jiaoYiDate->setDate(QDate::currentDate());
    for (auto& listWidget : m_imageListWidgets)
    {
        listWidget->clear();
    }

    if (m_house.isValid())
    {
        ui->houseNameEdit->setText(m_house.m_name);
        ui->jiaoShuiEdit->setText(m_house.m_jiaoShuiName);
        ui->zhongJieEdit->setText(m_house.m_zhongjieName);
        ui->feeEdit->setText(m_house.m_fee);
        ui->buyerNameEdit->setText(m_house.m_buyer.m_name);
        ui->buyerIdEdit->setText(m_house.m_buyer.m_id);
        ui->buyerPhoneEdit->setText(m_house.m_buyer.m_phoneNumber);
        ui->sellerNameEdit->setText(m_house.m_seller.m_name);
        ui->sellerPhoneEdit->setText(m_house.m_seller.m_phoneNumber);
        ui->sellerIdEdit->setText(m_house.m_seller.m_id);
        ui->jiaoYiDate->setDate(m_house.m_jiaoYiDate);

        for (const auto& imageId : m_house.m_fangChanZhengImages)
        {
            addImageCtrlItem(ui->fangChanZhengListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_shuiPiaoImages)
        {
            addImageCtrlItem(ui->shuiPiaoListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_tongZhiShuImages)
        {
            addImageCtrlItem(ui->tongZhiShuListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_buyer.m_hukouImages)
        {
            addImageCtrlItem(ui->buyerHuKouListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_buyer.m_idImages)
        {
            addImageCtrlItem(ui->buyerIdListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_buyer.m_marriageImages)
        {
            addImageCtrlItem(ui->buyerMarriageListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_seller.m_hukouImages)
        {
            addImageCtrlItem(ui->sellerHuKouListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_seller.m_idImages)
        {
            addImageCtrlItem(ui->sellerIdListWidget, imageId, -1);
        }

        for (const auto& imageId : m_house.m_seller.m_marriageImages)
        {
            addImageCtrlItem(ui->sellerMarriageListWidget, imageId, -1);
        }
    }

    if (m_type == ADD)
    {
        setWindowTitle(QString::fromWCharArray(L"新增房子"));
    }
    else if (m_type == EDIT)
    {
        setWindowTitle(QString::fromWCharArray(L"编辑房子"));
    }
    else
    {
        setWindowTitle(QString::fromWCharArray(L"查看房子"));
    }

    bool canEdit = (m_type == ADD || m_type == EDIT);
    ui->addFangChanZhengButton->setEnabled(canEdit);
    ui->addShuiPiaoButton->setEnabled(canEdit);
    ui->addTongZhiShuButton->setEnabled(canEdit);
    ui->addBuyerHuKouButton->setEnabled(canEdit);
    ui->addBuyerIdButton->setEnabled(canEdit);
    ui->addBuyerMarriageButton->setEnabled(canEdit);
    ui->addSellerHuKouButton->setEnabled(canEdit);
    ui->addSellerIdButton->setEnabled(canEdit);
    ui->addSellerMarriageButton->setEnabled(canEdit);
}

void HouseDialog::initActions()
{
    for (auto& listWidget : m_imageListWidgets)
    {
        listWidget->installEventFilter(this);
        connect(listWidget, &QListWidget::itemClicked, this, &HouseDialog::onImageItemClick);
    }

    connect(ui->addFangChanZhengButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->fangChanZhengListWidget);
    });

    connect(ui->addShuiPiaoButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->shuiPiaoListWidget);
    });

    connect(ui->addTongZhiShuButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->tongZhiShuListWidget);
    });

    connect(ui->addBuyerHuKouButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->buyerHuKouListWidget);
    });

    connect(ui->addBuyerIdButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->buyerIdListWidget);
    });

    connect(ui->addBuyerMarriageButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->buyerMarriageListWidget);
    });

    connect(ui->addSellerHuKouButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->sellerHuKouListWidget);
    });

    connect(ui->addSellerIdButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->sellerIdListWidget);
    });

    connect(ui->addSellerMarriageButton, &QPushButton::click, [this]() {
        onAddImageButton(ui->sellerMarriageListWidget);
    });

    connect(ui->cancelButton, &QPushButton::click, [this]() {
        close();
    });

    connect(ui->okButton, &QPushButton::click, [this]() {
        onOkButton();
    });
}

void HouseDialog::setType(Type type)
{
    m_type = type;
    initCtrls();
}

void HouseDialog::setHouse(const House& house)
{
    m_house = house;
    initCtrls();
}

void HouseDialog::onImageItemClick(QListWidgetItem *item)
{
    QString imageId = item->data(Qt::UserRole).toString();
    ImageDialog dialog(this, imageId);
    dialog.show();
    dialog.exec();
}

void HouseDialog::onOkButton()
{
    if (m_type == BROWSE)
    {
        close();
        return;
    }

    House house;
    house.m_name = ui->houseNameEdit->text();
    house.m_jiaoShuiName = ui->jiaoShuiEdit->text();
    house.m_zhongjieName = ui->zhongJieEdit->text();
    house.m_fee = ui->feeEdit->text();
    house.m_jiaoYiDate = ui->jiaoYiDate->date();
    house.m_buyer.m_name = ui->buyerNameEdit->text();
    house.m_buyer.m_id = ui->buyerIdEdit->text();
    house.m_buyer.m_phoneNumber = ui->buyerPhoneEdit->text();
    house.m_seller.m_name = ui->sellerNameEdit->text();
    house.m_seller.m_id = ui->sellerIdEdit->text();
    house.m_seller.m_phoneNumber = ui->sellerPhoneEdit->text();

    for (int i=0; i<ui->fangChanZhengListWidget->count(); i++)
    {
        QString imageId = ui->fangChanZhengListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_fangChanZhengImages.append(imageId);
    }

    for (int i=0; i<ui->shuiPiaoListWidget->count(); i++)
    {
        QString imageId = ui->shuiPiaoListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_shuiPiaoImages.append(imageId);
    }

    for (int i=0; i<ui->tongZhiShuListWidget->count(); i++)
    {
        QString imageId = ui->tongZhiShuListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_tongZhiShuImages.append(imageId);
    }

    for (int i=0; i<ui->buyerHuKouListWidget->count(); i++)
    {
        QString imageId = ui->buyerHuKouListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_hukouImages.append(imageId);
    }

    for (int i=0; i<ui->buyerIdListWidget->count(); i++)
    {
        QString imageId = ui->buyerIdListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_idImages.append(imageId);
    }

    for (int i=0; i<ui->buyerMarriageListWidget->count(); i++)
    {
        QString imageId = ui->buyerMarriageListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_marriageImages.append(imageId);
    }

    for (int i=0; i<ui->sellerHuKouListWidget->count(); i++)
    {
        QString imageId = ui->sellerHuKouListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_hukouImages.append(imageId);
    }

    for (int i=0; i<ui->sellerIdListWidget->count(); i++)
    {
        QString imageId = ui->sellerIdListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_idImages.append(imageId);
    }

    for (int i=0; i<ui->sellerMarriageListWidget->count(); i++)
    {
        QString imageId = ui->sellerMarriageListWidget->item(i)->data(Qt::UserRole).toString();
        house.m_buyer.m_marriageImages.append(imageId);
    }

    if (m_type == ADD)
    {
        HouseHttpClient* client = new HouseHttpClient(this);
        connect(client, &HouseHttpClient::addHouseCompletely,
                [this, client, house](bool success, int houseId) {
            if (!success)
            {
                UiUtil::showTip(QString::fromWCharArray(L"新增房子失败"));
                return;
            }
            else
            {
                UiUtil::showTip(QString::fromWCharArray(L"新增房子成功"));
                m_house = house;
                m_house.m_houseId = houseId;
                setResult(QDialog::Accepted);
                close();
                return;
            }

            client->deleteLater();
        });
        client->addHouse(house);
    }
    else if (m_type == EDIT)
    {
        HouseHttpClient* client = new HouseHttpClient(this);
        connect(client, &HouseHttpClient::updateHouseCompletely,
                [this, client, house](bool success) {
            if (!success)
            {
                UiUtil::showTip(QString::fromWCharArray(L"更新房子失败"));
                return;
            }
            else
            {
                UiUtil::showTip(QString::fromWCharArray(L"更新房子成功"));
                m_house = house;
                setResult(QDialog::Accepted);
                close();
                return;
            }

            client->deleteLater();
        });
        client->updateHouse(house);
    }
}

void HouseDialog::addImageCtrlItem(QListWidget* parent, const QString& imageId, int pos)
{
    QLabel* imageLabel = new QLabel(parent);
    imageLabel->setFixedSize(parent->height(), parent->height());
    QString imageFilePath = PathUtil::getImageFilePath(imageId);
    if (!QFile(imageFilePath).exists())
    {
        imageFilePath = ":/res/images/loading.png";
    }
    QPixmap pixmap(imageFilePath);
    if (pixmap.isNull())
    {
        qCritical("failed to load image: %s", imageFilePath.toStdString().c_str());
    }
    else
    {
        UiUtil::showImage(pixmap, *imageLabel);
    }

    QListWidgetItem *item = new QListWidgetItem(parent);
    item->setSizeHint(imageLabel->sizeHint());
    item->setData(Qt::UserRole, imageId);
    if (pos == -1)
    {
        parent->addItem(item);
    }
    else
    {
        parent->insertItem(pos, item);
    }
    parent->setItemWidget(item, imageLabel);
}

bool HouseDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu)
    {
        QListWidget* listWidget = nullptr;
        for (auto& item : m_imageListWidgets)
        {
            if (item == obj)
            {
                listWidget = item;
                break;
            }
        }

        if (listWidget)
        {
            QContextMenuEvent *contextMenuEvent = (QContextMenuEvent*)(event);
            QListWidgetItem *item = listWidget->itemAt(contextMenuEvent->pos());
            if (item)
            {
                QMenu menu;
                menu.addAction(QString::fromWCharArray(L"删除"));
                QAction * action = menu.exec(cursor().pos());
                if (action && action->text() == QString::fromWCharArray(L"删除"))
                {
                    listWidget->removeItemWidget(item);
                }
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

void HouseDialog::onAddImageButton(QListWidget* listWidget)
{
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QString::fromWCharArray(L"选择图片"));
    fileDialog.setNameFilters(QStringList() << "image files (*.jpg|*.jpeg|*.png|*.bmp)");
    if (fileDialog.exec() != QDialog::Accepted)
    {
        return;
    }

    QString imageFilePath = fileDialog.selectedFiles()[0];
    UploadImageController* controller = new UploadImageController(this);
    connect(controller, &UploadImageController::runFinish, [this, listWidget, controller, imageFilePath]() {
        if (controller->m_imageId.isEmpty())
        {
            UiUtil::showTip(QString::fromWCharArray(L"上传图片失败"));
            return;
        }

        QString imageDestFilePath = PathUtil::getImageFilePath(controller->m_imageId);
        ::CopyFile(imageFilePath.toStdWString().c_str(), imageDestFilePath.toStdWString().c_str(), TRUE);

        addImageCtrlItem(listWidget, controller->m_imageId, -1);
    });
    controller->run(this, imageFilePath);
}

void HouseDialog::downloadImages()
{
    QVector<QString> imageIds;
    imageIds.append(m_house.m_fangChanZhengImages);
    imageIds.append(m_house.m_shuiPiaoImages);
    imageIds.append(m_house.m_tongZhiShuImages);
    imageIds.append(m_house.m_buyer.m_hukouImages);
    imageIds.append(m_house.m_buyer.m_idImages);
    imageIds.append(m_house.m_buyer.m_marriageImages);
    imageIds.append(m_house.m_seller.m_hukouImages);
    imageIds.append(m_house.m_seller.m_idImages);
    imageIds.append(m_house.m_seller.m_marriageImages);

    QVector<QString> downloadImageIds;
    for (const auto& imageId : imageIds)
    {
        QString imageFilePath = PathUtil::getImageFilePath(imageId);
        if (!QFile(imageFilePath).exists())
        {
            downloadImageIds.append(imageId);
        }
    }

    DownloadImageController* controller = new DownloadImageController(this);
    connect(controller, &DownloadImageController::downloadImageCompletely, [this](QString imageId) {
        for (auto& listWidget : m_imageListWidgets)
        {
            bool found = false;
            for (int i=0; i<listWidget->count(); i++)
            {
                if (listWidget->item(i)->data(Qt::UserRole) == imageId)
                {
                    found = true;
                    listWidget->removeItemWidget(listWidget->item(i));
                    addImageCtrlItem(listWidget, imageId, i);
                    break;
                }
            }

            if (found)
            {
                break;
            }
        }
    });
    connect(controller, &DownloadImageController::runFinish, [controller]() {
        controller->deleteLater();
    });
    controller->run(downloadImageIds);
}
