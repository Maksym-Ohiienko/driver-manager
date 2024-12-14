#ifndef DRIVERMASTER_H
#define DRIVERMASTER_H

#include <QAxObject>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QSettings>
#include <windows.h>

#include "driver.h"
#include "threadmanager.h"
#include "downloadhelper.h"

class DriverMaster : public QObject
{
    Q_OBJECT

public:
    explicit DriverMaster();

    QVector<QSharedPointer<Driver>> listDrivers();

    void downloadDriver(const QString& driverManufacturer);
    void deleteDriver(const QString& infPath);
    void checkForUpdates();

signals:
    void downloadFinished(bool success);
    void deleteFinished(bool success);
    void checkEnded(QString newVersion);

private:
    QJsonArray                                          m_jsonArray; // Для доступу до інформації драйверів
    QJsonObject                                         m_currDriverInfo; // Інформація про поточний об'єкт драйверу
    QWebEnginePage*                                     m_page; // Змінна для завантаження сторінки
    QList<QPair<QSharedPointer<Driver>, QJsonObject>>   m_driversInfo; // Таблиця Драйвер-Інформація про нього

    void        loadDriversJson(QString path);
    void        downloadAsync(const QUrl& url);

    QAxObject*  initializeCom(); // Для доступу до WMI
    bool        findInJson(const QString& driverManufacturer); // Для пошуку по конкретному драйверу розробника
    bool        isDriverNeedUpdate(const QString& currVersion, const QString& newVersion);
    QString     checkForIntelUpdate(QString regPath, QString keyToSearch);

private slots:
    void onDownloadPageLoadFinished(bool success);
    void onVersionPageLoadFinished(bool success);
};

#endif // DRIVERMASTER_H
