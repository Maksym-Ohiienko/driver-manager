#include "downloadhelper.h"

DownloadHelper :: DownloadHelper(QObject *parent)
    : QObject{parent}
{

}

void DownloadHelper :: downloadAndInstall(const QUrl& url, const QString& downloadDestPath, const QString& installSourcePath, const QString& silentInstallParam) {

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    QNetworkRequest request(url);
    request.setRawHeader("Referer", url.toString().toUtf8());

    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QFile file(downloadDestPath);

            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                qDebug() << "File downloaded successfully!";
                install(downloadDestPath, installSourcePath, silentInstallParam);
            } else {
                qDebug() << "Error opening file for writing!\n" << "\nDownload Dest Path: " << downloadDestPath << "\nInstall Source Path: "  << installSourcePath;
            }
        } else {
            qDebug() << "Error downloading file:" << reply->errorString();
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void DownloadHelper :: install(const QString& archivePath, const QString& installDir, const QString& silentInstallParam) {

    qDebug() << "Path to setup:" << archivePath;
    qDebug() << "Extracting to:" << installDir;

    if (!QFile::exists(archivePath)) {
        qDebug() << "File does not exist:" << archivePath;
        return;
    }

    QProcess* unpackProcess = new QProcess(this);

    QDir solutionPath = QCoreApplication::applicationDirPath();
    solutionPath.cdUp();

    QString full7zPath = QString(solutionPath.absolutePath() + "/7-Zip/7z.exe");

    qDebug() << "7z Path " << full7zPath;

    QString unpackCommand = QString(full7zPath + " x %1 -o%2 -y")
                                .arg(archivePath)
                                .arg(installDir);

    qDebug() << "Unpack command:" << unpackCommand;

    unpackProcess->start("cmd.exe", QStringList() << "/C" << unpackCommand);

    // Перехоплення виводу процесу
    connect(unpackProcess, &QProcess::readyReadStandardOutput, unpackProcess, [unpackProcess]() {
        qDebug() << "Standard Output:" << unpackProcess->readAllStandardOutput();
    });
    connect(unpackProcess, &QProcess::readyReadStandardError, unpackProcess, [unpackProcess]() {
        qDebug() << "Standard Error:" << unpackProcess->readAllStandardError();
    });
    connect(unpackProcess, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "QProcess error:" << error;
    });

    // Перехоплення завершення розпаковуання
    connect(unpackProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus status) {

        unpackProcess->deleteLater();

        if (status == QProcess::NormalExit && exitCode == 0) {
            qDebug() << "Unpacked successfully!";

            // Далі запуск setup.exe у тихому режимі
            QProcess* installProcess = new QProcess(this);
            QString setupPath = QString("%1/setup.exe").arg(installDir); // Шлях до setup.exe після розпакування

            if (archivePath.endsWith("Intel.exe")) {
                setupPath = archivePath;
            }

            QString installCommand = QString(setupPath + " " + silentInstallParam);

            connect(installProcess, &QProcess::finished, this, [this, installProcess]() {

                installProcess->deleteLater();

                qDebug() << "Silent installation completed successfully!";
                emit downloadFinished();

            });

            installProcess->start("cmd.exe", QStringList() << "/C" << installCommand); // Запуск встановлення

        } else {
            qDebug() << "Failed to unpack. Exit code:" << exitCode;
        }
    });
}
