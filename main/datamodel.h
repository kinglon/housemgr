#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QString>
#include <QVector>
#include <QDateTime>

// 买家/卖家
class Person
{
public:
    // 姓名
    QString m_name;

    // 电话号码
    QString m_phoneNumber;

    // 身份证号码
    QString m_id;

    // 户口册图片
    QVector<QString> m_hukouImages;

    // 身份证图片
    QVector<QString> m_idImages;

    // 结婚证图片
    QVector<QString> m_marriageImages;
};

// 房子
class House
{
public:
    // 房子ID
    int m_houseId = -1;

    // 名称
    QString m_name;

    // 缴税人
    QString m_jiaoShuiName;

    // 费用
    QString m_fee;

    // 中介人名字
    QString m_zhongjieName;

    // 交易日期
    QDate m_jiaoYiDate;

    // 房产证图片
    QVector<QString> m_fangChanZhengImages;

    // 税票图片
    QVector<QString> m_shuiPiaoImages;

    // 受理通知书图片
    QVector<QString> m_tongZhiShuImages;

    // 买方
    Person m_buyer;

    // 卖方
    Person m_seller;

public:
    bool isValid()
    {
        if (m_houseId < 0)
        {
            return false;
        }

        return true;
    }
};

#endif // DATAMODEL_H
