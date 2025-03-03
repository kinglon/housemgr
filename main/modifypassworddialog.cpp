#include "modifypassworddialog.h"
#include "ui_modifypassworddialog.h"
#include "uiutil.h"
#include "settingmanager.h"
#include <QCryptographicHash>

ModifyPasswordDialog::ModifyPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyPasswordDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowModality(Qt::WindowModal);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    initAction();
}

ModifyPasswordDialog::~ModifyPasswordDialog()
{
    delete ui;
}

void ModifyPasswordDialog::initAction()
{
    connect(ui->cancelButton, &QPushButton::clicked, [this]() {
        close();
    });

    connect(ui->okButton, &QPushButton::clicked, [this]() {
        QString oldPassword = ui->oldPasswordEdit->text();
        QString newPassword = ui->newPasswordEdit->text();
        QString newPassword2 = ui->newPassword2Edit->text();
        if (oldPassword.isEmpty() || newPassword.isEmpty() || newPassword2.isEmpty())
        {
            UiUtil::showTip(QString::fromWCharArray(L"请输入密码"));
            return;
        }

        if (SettingManager::getInstance()->m_passwordMd5.isEmpty())
        {
            if (oldPassword != SettingManager::getInstance()->getDefaultPassword())
            {
                UiUtil::showTip(QString::fromWCharArray(L"旧密码不是默认密码"));
                return;
            }
        }
        else
        {
            QByteArray hashBytes = QCryptographicHash::hash(oldPassword.toUtf8(), QCryptographicHash::Md5);
            QString passwordMd5(hashBytes);
            if (passwordMd5 != SettingManager::getInstance()->m_passwordMd5)
            {
                UiUtil::showTip(QString::fromWCharArray(L"旧密码错误"));
                return;
            }
        }

        if (newPassword != newPassword2)
        {
            UiUtil::showTip(QString::fromWCharArray(L"新密码不一致"));
            return;
        }

        QByteArray hashBytes = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Md5);
        SettingManager::getInstance()->m_passwordMd5 = hashBytes;
        SettingManager::getInstance()->save();
        UiUtil::showTip(QString::fromWCharArray(L"修改密码成功"));
        close();
    });
}
