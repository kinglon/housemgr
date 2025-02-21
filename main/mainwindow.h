#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "datamodel.h"
#include "househttpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initCtrls();

    void initHouseTableView();

    // 更新房子列表和翻页
    void updateHouseTableView();

private slots:
    void onCtrlDShortcut();

    void onAddButton();

    void onSearchButton();

    void onLastPageButton();

    void onNextPageButton();

    void onEditHouseTableView(int row);

    void onDeleteHouseTableView(int row);

    void onBrowseTableView(int row);

private:
    void doSearch(const SearchCondition& searchCondition);

    House getHouse(int houseId);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;

    QShortcut* m_ctrlDShortcut = nullptr;

    QStandardItemModel m_houseViewModel;

    SearchCondition m_searchCondition;

    SearchResult m_searchResult;
};
#endif // MAINWINDOW_H
