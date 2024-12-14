#ifndef DOWNLOADHELPER_H
#define DOWNLOADHELPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QFile>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

class DownloadHelper : public QObject
{
    Q_OBJECT

public:
    explicit DownloadHelper(QObject* parent = nullptr);
    void downloadAndInstall(const QUrl& url, const QString& downloadDestPath, const QString& installSourcePath, const QString& silentInstallParam);

private:
    void install(const QString& archivePath, const QString& installDir, const QString& silentInstallParam);

signals:
    void downloadFinished();
};

#endif // DOWNLOADHELPER_H
