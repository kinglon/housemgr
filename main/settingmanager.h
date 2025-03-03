#pragma once

#include <QString>
#include <QVector>

class SettingManager
{
protected:
    SettingManager();

public:
    static SettingManager* getInstance();

    QString getDefaultPassword();

    void save();

private:
    void load();

public:
    int m_logLevel = 2;  // info

    QString m_host;

    QString m_passwordMd5;
};
