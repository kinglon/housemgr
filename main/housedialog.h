#ifndef HOUSEDIALOG_H
#define HOUSEDIALOG_H

#include <QDialog>
#include "datamodel.h"
#include <QListWidgetItem>

namespace Ui {
class HouseDialog;
}

class HouseDialog : public QDialog
{
    Q_OBJECT

public:
    enum Type
    {
        ADD,
        EDIT,
        BROWSE
    };

public:
    explicit HouseDialog(QWidget *parent = nullptr);
    ~HouseDialog();

public:
    void setType(Type type);

    void setHouse(const House& house);

    House getHouse() { return m_house; }

private:
    void initCtrls();

    void initActions();

    // pos=-1 添加到尾部
    void addImageCtrlItem(QListWidget* parent, const QString& imageId, int pos);

    void downloadImages();

private:
    void onAddImageButton(QListWidget* listWidget);

    void onOkButton();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::HouseDialog *ui;

    Type m_type = BROWSE;

    House m_house;

    // 该界面上所有的图片列表控件
    QVector<QListWidget*> m_imageListWidgets;
};

#endif // HOUSEDIALOG_H
