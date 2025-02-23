#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>
#include "housedialog.h"
#include "uiutil.h"
#include "househttpclient.h"
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    initCtrls();

    m_ctrlDShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(m_ctrlDShortcut, &QShortcut::activated, this, &MainWindow::onCtrlDShortcut);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initCtrls()
{
    ui->jiaoYiBeginDate->setDate(QDate::currentDate().addYears(-1));
    ui->jiaoYiEndDate->setDate(QDate::currentDate());

    initHouseTableView();

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddButton);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButton);
    connect(ui->lastPageButton, &QPushButton::clicked, this, &MainWindow::onLastPageButton);
    connect(ui->nextPageButton, &QPushButton::clicked, this, &MainWindow::onNextPageButton);
}

void MainWindow::initHouseTableView()
{
    ui->houseTableView->installEventFilter(this);
    ui->houseTableView->setModel(&m_houseViewModel);

    m_houseViewModel.clear();
    m_houseViewModel.setColumnCount(11);
    m_houseViewModel.setHeaderData(0, Qt::Horizontal, QString::fromWCharArray(L"房子名称"));
    ui->houseTableView->setColumnWidth(0, 200);
    m_houseViewModel.setHeaderData(1, Qt::Horizontal, QString::fromWCharArray(L"缴税人"));
    m_houseViewModel.setHeaderData(2, Qt::Horizontal, QString::fromWCharArray(L"中介人"));
    m_houseViewModel.setHeaderData(3, Qt::Horizontal, QString::fromWCharArray(L"交易日期"));
    m_houseViewModel.setHeaderData(4, Qt::Horizontal, QString::fromWCharArray(L"费用"));
    m_houseViewModel.setHeaderData(5, Qt::Horizontal, QString::fromWCharArray(L"买家姓名"));
    m_houseViewModel.setHeaderData(6, Qt::Horizontal, QString::fromWCharArray(L"买家电话"));
    m_houseViewModel.setHeaderData(7, Qt::Horizontal, QString::fromWCharArray(L"买家身份证"));
    ui->houseTableView->setColumnWidth(7, 200);
    m_houseViewModel.setHeaderData(8, Qt::Horizontal, QString::fromWCharArray(L"卖家姓名"));
    m_houseViewModel.setHeaderData(9, Qt::Horizontal, QString::fromWCharArray(L"卖家电话"));
    m_houseViewModel.setHeaderData(10, Qt::Horizontal, QString::fromWCharArray(L"卖家身份证"));
    ui->houseTableView->setColumnWidth(10, 200);

    updateHouseTableView();
}

void MainWindow::updateHouseTableView()
{
    m_houseViewModel.removeRows(0, m_houseViewModel.rowCount());
    for (int row=0; row<m_searchResult.m_houses.size(); row++)
    {
        const House& house = m_searchResult.m_houses[row];
        QStandardItem* item = new QStandardItem(house.m_name);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 0, item);

        item = new QStandardItem(house.m_jiaoShuiName);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 1, item);

        item = new QStandardItem(house.m_zhongjieName);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 2, item);

        item = new QStandardItem(house.m_jiaoYiDate.toString("yyyy-M-d"));
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 3, item);

        item = new QStandardItem(house.m_fee);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 4, item);

        item = new QStandardItem(house.m_buyer.m_name);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 5, item);

        item = new QStandardItem(house.m_buyer.m_phoneNumber);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 6, item);

        item = new QStandardItem(house.m_buyer.m_id);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 7, item);

        item = new QStandardItem(house.m_seller.m_name);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 8, item);

        item = new QStandardItem(house.m_seller.m_phoneNumber);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 9, item);

        item = new QStandardItem(house.m_seller.m_id);
        item->setTextAlignment(Qt::AlignCenter);
        m_houseViewModel.setItem(row, 10, item);

        m_houseViewModel.setData(m_houseViewModel.index(row, 0), house.m_houseId, Qt::UserRole);
    }

    ui->lastPageButton->setEnabled(m_searchCondition.m_page>1);
    ui->nextPageButton->setEnabled(m_searchCondition.m_page < m_searchResult.m_total);
    if (m_searchResult.m_houses.size() > 0)
    {
        int totalPage = m_searchResult.getTotalPage(m_searchCondition.m_pageSize);
        QString pageNumber = QString("%1/%2").arg(QString::number(m_searchCondition.m_page),
                                                  QString::number(totalPage));
        ui->pageNumberLabel->setText(pageNumber);
        ui->pageNumberLabel->setVisible(true);
    }
    else
    {
        ui->pageNumberLabel->setVisible(false);
    }
}

void MainWindow::onCtrlDShortcut()
{
    //
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->houseTableView && event->type() == QEvent::ContextMenu)
    {
        QModelIndex index = ui->houseTableView->currentIndex();
        if (index.isValid())
        {
            QMenu menu;
            menu.addAction(QString::fromWCharArray(L"查看"));
            menu.addAction(QString::fromWCharArray(L"编辑"));
            menu.addAction(QString::fromWCharArray(L"删除"));
            QAction * action = menu.exec(cursor().pos());
            if (action && action->text() == QString::fromWCharArray(L"编辑"))
            {
                onEditHouseTableView(index.row());
            }
            else if (action && action->text() == QString::fromWCharArray(L"删除"))
            {
                onDeleteHouseTableView(index.row());
            }
            else if (action && action->text() == QString::fromWCharArray(L"查看"))
            {
                onBrowseTableView(index.row());
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onAddButton()
{
    HouseDialog dialog(this);
    dialog.setType(HouseDialog::ADD);
    dialog.show();
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    doSearch(m_searchCondition);
}

void MainWindow::doSearch(const SearchCondition& searchCondition)
{
    HouseHttpClient* client = new HouseHttpClient(this);
    connect(client, &HouseHttpClient::queryHouseCompletely,
            [this, searchCondition, client](bool success, const SearchResult& result) {
        if (!success)
        {
            return;
        }

        m_searchCondition = searchCondition;
        m_searchResult = result;
        updateHouseTableView();
        client->deleteLater();
    });
    client->queryHouse(searchCondition);
}

void MainWindow::onSearchButton()
{
    SearchCondition searchCondition;
    searchCondition.m_enableJiaoYiDate = ui->jiaoYiDateCheckBox->isChecked();
    if (searchCondition.m_enableJiaoYiDate)
    {
        searchCondition.m_searchJiaoYiBeginDate = ui->jiaoYiBeginDate->date();
        searchCondition.m_searchJiaoYiEndDate = ui->jiaoYiEndDate->date().addDays(1);
        if (searchCondition.m_searchJiaoYiBeginDate >= searchCondition.m_searchJiaoYiEndDate)
        {
            UiUtil::showTip(QString::fromWCharArray(L"请正确设置交易日期"));
            return;
        }
    }

    searchCondition.m_searchCondition.m_name = ui->houseNameEdit->text();
    searchCondition.m_searchCondition.m_jiaoShuiName = ui->jiaoShuiEdit->text();
    searchCondition.m_searchCondition.m_zhongjieName = ui->zhongJieEdit->text();
    searchCondition.m_searchCondition.m_buyer.m_name = ui->buyerName->text();
    searchCondition.m_searchCondition.m_buyer.m_phoneNumber = ui->buyerPhoneEdit->text();
    searchCondition.m_searchCondition.m_buyer.m_id = ui->buyerIdEdit->text();
    searchCondition.m_searchCondition.m_seller.m_name = ui->sellerNameEdit->text();
    searchCondition.m_searchCondition.m_seller.m_phoneNumber = ui->sellerPhoneEdit->text();
    searchCondition.m_searchCondition.m_seller.m_id = ui->sellerIdEdit->text();

    doSearch(searchCondition);
}

void MainWindow::onLastPageButton()
{
    SearchCondition searchCondition = m_searchCondition;
    searchCondition.m_page--;
    if (searchCondition.m_page < 1)
    {
        return;
    }

    doSearch(searchCondition);
}

void MainWindow::onNextPageButton()
{
    SearchCondition searchCondition = m_searchCondition;
    searchCondition.m_page++;
    int totalPage = m_searchResult.getTotalPage(m_searchCondition.m_pageSize);
    if (searchCondition.m_page > totalPage)
    {
        return;
    }

    doSearch(searchCondition);
}

House MainWindow::getHouse(int houseId)
{
    for (const auto& house : m_searchResult.m_houses)
    {
        if (house.m_houseId == houseId)
        {
            return house;
        }
    }

    return House();
}

void MainWindow::onEditHouseTableView(int row)
{
    int houseId = m_houseViewModel.data(m_houseViewModel.index(row, 0), Qt::UserRole).toInt();
    House house = getHouse(houseId);
    if (!house.isValid())
    {
        return;
    }

    HouseDialog dialog(this);
    dialog.setType(HouseDialog::EDIT);
    dialog.setHouse(house);
    dialog.show();
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    doSearch(m_searchCondition);
}

void MainWindow::onDeleteHouseTableView(int row)
{
    if (!UiUtil::showTipV2(QString::fromWCharArray(L"确定删除？")))
    {
        return;
    }

    int houseId = m_houseViewModel.data(m_houseViewModel.index(row, 0), Qt::UserRole).toInt();
    HouseHttpClient* client = new HouseHttpClient(this);
    connect(client, &HouseHttpClient::deleteHouseCompletely,
            [this, client](bool success) {
        if (!success)
        {
            UiUtil::showTip(QString::fromWCharArray(L"删除失败"));
            return;
        }

        doSearch(m_searchCondition);
        client->deleteLater();
    });
    client->deleteHouse(houseId);
}

void MainWindow::onBrowseTableView(int row)
{
    int houseId = m_houseViewModel.data(m_houseViewModel.index(row, 0), Qt::UserRole).toInt();
    House house = getHouse(houseId);
    if (!house.isValid())
    {
        return;
    }

    HouseDialog dialog(this);
    dialog.setType(HouseDialog::BROWSE);
    dialog.setHouse(house);
    dialog.show();
    dialog.exec();
}
