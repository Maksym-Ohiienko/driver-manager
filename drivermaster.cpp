#include "drivermaster.h"

DriverMaster :: DriverMaster()
{
    loadDriversJson(":/Json/json/drivers.json");    
}

void DriverMaster :: loadDriversJson(QString path) {

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to parse JSON.";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    m_jsonArray = jsonObject["drivers"].toArray();
}

QAxObject* DriverMaster :: initializeCom() {

    QAxObject* wmiService = new QAxObject("WbemScripting.SWbemLocator");
    QAxObject* wmi = wmiService->querySubObject("ConnectServer(QString, QString)", ".", "root\\CIMV2");

    if (!wmi) {
        qWarning() << "Failed to connect to WMI.";
        delete wmiService;

        return nullptr;
    }

    return wmi;
}

QVector<QSharedPointer<Driver>> DriverMaster :: listDrivers() {

    QVector<QSharedPointer<Driver>> driverList;
    QAxObject* wmi = initializeCom(); // Ініціалізація вказівника для доступу до COM та WMI

    m_driversInfo.clear();

    if (!wmi) {
        qWarning() << "Failed to initialize COM!";
        return driverList;
    }

    QAxObject* drivers = wmi->querySubObject( // Отримання списку драйверів
        "ExecQuery(QString)",
        "SELECT DriverName, DriverProviderName, InfName, DriverVersion, Manufacturer, HardWareID, CompatID FROM Win32_PnPSignedDriver"
        );

    QAxObject* devices = wmi->querySubObject( // Отримання списку пристроїв
        "ExecQuery(QString)",
        "SELECT PNPClass, HardwareID, CompatibleID, Name FROM Win32_PnPEntity");


    int driverCount = drivers->dynamicCall("Count").toInt();
    int deviceCount = devices->dynamicCall("Count").toInt();

    if (!driverCount && !deviceCount) {
        qDebug() << "Query is empty!";

        delete wmi;
        return driverList;
    }

    QAxObject* enumDrivers = drivers->querySubObject("_NewEnum");
    QAxObject* enumDevices = devices->querySubObject("_NewEnum");

    if (!enumDrivers && !enumDevices) {
        qDebug() << "Query _NewEnum failed";

        delete wmi;
        return driverList;
    }

    IEnumVARIANT* enumDriverInterface = nullptr;
    IEnumVARIANT* enumDeviceInterface = nullptr;

    enumDrivers->queryInterface(IID_IEnumVARIANT, (void**)&enumDriverInterface);
    enumDevices->queryInterface(IID_IEnumVARIANT, (void**)&enumDeviceInterface);

    if (!enumDriverInterface && !enumDeviceInterface) {
        qDebug() << "Query interface failed";

        delete wmi;
        return driverList;
    }

    QHash<QString, QSharedPointer<Driver>> driverMap; // Для збереження id сумісного пристрою(HardWareID або CompatID) - Драйвер

    VARIANT driverItem;
    VariantInit(&driverItem);
    while (enumDriverInterface->Next(1, &driverItem, NULL) == S_OK) {

        auto driver = QSharedPointer<QAxObject>::create((IUnknown*)driverItem.punkVal);

        if (driver) {

            QString driverHardwareId    = driver->dynamicCall("HardWareID").toString();
            QString driverCompatibleId  = driver->dynamicCall("CompatID").toString();

            QString driverName          = driver->dynamicCall("DriverName").toString();
            QString driverProviderName  = driver->dynamicCall("DriverProviderName").toString();
            QString infName             = driver->dynamicCall("InfName").toString();
            QString name                = driverName.isEmpty() ? infName : driverName;

            QString manufacturer    = driver->dynamicCall("Manufacturer").toString();
            QString version         = driver->dynamicCall("DriverVersion").toString();

            QRegularExpression regex("^Microsoft.*");
            bool isOutsiderProvider = driverProviderName.isEmpty() || !regex.match(driverProviderName).hasMatch();

            auto currDriver = QSharedPointer<Driver>::create(
                "Empty",
                "Empty",
                name,
                manufacturer,
                version,
                version,
                Driver::State::Actual,
                isOutsiderProvider);

            if (findInJson(manufacturer)) { // Якщо є інформація про цей драйвер

                auto driverInfo = QPair<QSharedPointer<Driver>, QJsonObject>(currDriver, m_currDriverInfo);

                auto it = std::find_if(m_driversInfo.begin(), m_driversInfo.end(),

                    [&driverInfo](const QPair<QSharedPointer<Driver>, QJsonObject>& pair) {
                        return pair.first->manufacturer() == driverInfo.first->manufacturer();
                    });

                if (it != m_driversInfo.end()) {
                    m_driversInfo.erase(it);
                }
                m_driversInfo.append(driverInfo);
            }

            if (!driverHardwareId.isEmpty()) { // Драйвер має HardWareID, тобто підтримує взаємодію з апаратним забезпеченням
                driverMap[driverHardwareId] = currDriver;
            }
            else if (!driverCompatibleId.isEmpty()) { // Драйвер має CompatibleID, тобто підтримує взаємодію з апаратним забезпеченням
                driverMap[driverCompatibleId] = currDriver;
            }
            else { // Якщо драйвер не взаємодіє з АЗ
                driverList.append(currDriver);
            }
        }
        VariantClear(&driverItem);
    }

    VARIANT deviceItem;
    VariantInit(&deviceItem);
    while (enumDeviceInterface->Next(1, &deviceItem, NULL) == S_OK) {

        auto device = QSharedPointer<QAxObject>::create((IUnknown*)deviceItem.punkVal);

        if (device) {

            QStringList hardwareId = device->dynamicCall("HardwareID").toStringList();
            QStringList deviceCompatibleId = device->dynamicCall("CompatibleID").toStringList();
            QString deviceName = device->dynamicCall("Name").toString();
            QString deviceClass = device->dynamicCall("PNPClass").toString();

            bool found = false;
            int i = 0, j = 0;

            // Перевіряємо обидва варіанти(HardwareID та CompatibleID)
            while (i < hardwareId.count() || j < deviceCompatibleId.count()) {

                if (i < hardwareId.count() && driverMap.contains(hardwareId[i])) { // Перевіряємо hardwareID
                    QSharedPointer<Driver> driver = driverMap[hardwareId[i]];

                    driver->setDeviceClass(deviceClass);
                    driver->setDeviceName(deviceName);
                    driverList.append(driver); // Додаємо як актуальний
                    found = true;
                    break;
                }

                if (j < deviceCompatibleId.count() && driverMap.contains(deviceCompatibleId[j])) { // Перевіряємо compatibleID
                    QSharedPointer<Driver> driver = driverMap[deviceCompatibleId[j]];

                    driver->setDeviceClass(deviceClass);
                    driver->setDeviceName(deviceName);
                    driverList.append(driver); // Додаємо як актуальний
                    found = true;
                    break;
                }

                ++i;
                ++j;
            }

            if (!found) {
                driverList.append(QSharedPointer<Driver>::create(deviceClass.isEmpty() ? "Empty" : deviceClass, deviceName.isEmpty() ? "Empty" : deviceName,
                                                                 "Empty", "Empty", "Empty", "Empty", Driver::State::Download)); // Пристрій потребує драйверу
            }
        }
        VariantClear(&deviceItem);
    }

    driverMap.clear();
    enumDriverInterface->Release();
    enumDeviceInterface->Release();

    delete enumDrivers;
    delete enumDevices;
    delete drivers;
    delete devices;
    delete wmi;

    return driverList;
}

bool DriverMaster :: findInJson(const QString& driverManufacturer) {

    for (const QJsonValue& el : m_jsonArray) {

        QJsonObject driverInfo  = el.toObject();
        QString manufacturer    = driverInfo["manufacturer"].toString();

        if (manufacturer == driverManufacturer) {

            m_currDriverInfo = driverInfo;
            return true;
        }
    }

    return false;
}

bool DriverMaster :: isDriverNeedUpdate(const QString& currVersion, const QString& newVersion) {

    QStringList currParts = currVersion.split('.');
    QStringList newParts = newVersion.split('.');

    int maxLength = qMax(currParts.size(), newParts.size());

    for (int i = 0; i < maxLength; ++i) {

        int currPart = (i < currParts.size()) ? currParts[i].toInt() : 0;
        int newPart = (i < newParts.size()) ? newParts[i].toInt() : 0;

        if (newPart < currPart) return false;
        if (newPart > currPart) return true;
    }

    return false;
}

QString DriverMaster :: checkForIntelUpdate(QString regPath, QString keyToSearch) {

    QSettings reg(regPath, QSettings::NativeFormat);

    QStringList groups = reg.childGroups();

    for (const QString& group : groups) {

        QSettings subKey(regPath + "\\" + group, QSettings::NativeFormat);

        QString displayName = subKey.value("DisplayName").toString();

        if (displayName == "Intel Driver && Support Assistant") {

            return subKey.value(keyToSearch).toString();
        }
    }

    return "Empty";
}

void DriverMaster :: checkForUpdates() {

    for (auto& element : m_driversInfo) {

        QEventLoop* loop = new QEventLoop(this);

        QString regPath = element.second["versionRegeditPath"].toString();
        QString regKey  = element.second["versionRegeditKey"].toString();
        qDebug() << "regPath " << regPath << " regKey " << regKey;

        if (regPath.isEmpty() || regKey.isEmpty()) {
            continue;
        }

        QSettings reg(regPath, QSettings::NativeFormat);
        QString currVersion = reg.value(regKey, "Empty").toString();

        if (element.first->manufacturer() == "Intel") {
            currVersion = checkForIntelUpdate(regPath, regKey);
        }

        connect(this, &DriverMaster::checkEnded, this, [&](QString newVersion) {

            qDebug() << "disconnect\ncurrVersion = " << currVersion << "\nnewVersion = " << newVersion << '\n';

            if (isDriverNeedUpdate(currVersion, newVersion)) {

                element.first->setCurrVersion(currVersion);
                element.first->setNewVersion(newVersion);
                element.first->setState(Driver::State::Update);
            }

            disconnect(this, &DriverMaster::checkEnded, this, nullptr);

            loop->quit();
            loop->deleteLater();
        });

        QUrl url = QUrl(element.second["versionUrl"].toString());
        m_page = new QWebEnginePage(QWebEngineProfile::defaultProfile());

        disconnect(m_page, SIGNAL(loadFinished(bool)), this, nullptr);
        connect(m_page, SIGNAL(loadFinished(bool)), this, SLOT(onVersionPageLoadFinished(bool)));

        m_page->load(url);

        loop->exec();
    }
}

void DriverMaster :: onVersionPageLoadFinished(bool success) {

    if (!success) {
        qDebug() << "Error loading page!";
        return;
    }

    QString id = "versions-items-list";

    // Виконуємо JavaScript для отримання href кнопки
    QString js = QString(
                     "document.getElementById('%1').firstElementChild.children[1].textContent;"
                     ).arg(id);

    m_page->runJavaScript(js, [this](const QVariant& result) {

        QString newVersion = result.toString();

        emit checkEnded(newVersion);

        m_page->deleteLater();
    });
}

void DriverMaster :: downloadDriver(const QString& driverManufacturer) {

    if (findInJson(driverManufacturer)) { // Якщо такий драйвер можна завантажити

        QString buttonId = m_currDriverInfo["downloadButtonId"].toString();
        QUrl url         = QUrl(m_currDriverInfo["downloadUrl"].toString());

        if (buttonId.isEmpty()) { // Якщо кнопка для завантаження не вказана, одразу завантажуємо по url
            downloadAsync(url);
        } else {

            m_page = new QWebEnginePage(QWebEngineProfile::defaultProfile());

            disconnect(m_page, SIGNAL(loadFinished(bool)), this, nullptr);
            connect(m_page, SIGNAL(loadFinished(bool)), this, SLOT(onDownloadPageLoadFinished(bool)));

            m_page->load(url);
        }
    }
    else {
        emit downloadFinished(false); // Завантажити не вдалося
    }
}

void DriverMaster :: onDownloadPageLoadFinished(bool success) {

    if (!success) {
        qDebug() << "Error loading page!";
        return;
    }

    QString buttonId = m_currDriverInfo["downloadButtonId"].toString();

    // Виконуємо JavaScript для отримання href кнопки
    QString js = QString(
    "document.getElementById('%1').href;"
     ).arg(buttonId);

    m_page->runJavaScript(js, [this](const QVariant& result) {

        QUrl href = result.toUrl();

        qDebug() << "Download link: " << href;
        downloadAsync(href);

        m_page->deleteLater();
    });
}

void DriverMaster :: downloadAsync(const QUrl& url) {

    QThread* thread = new QThread(this);

    ThreadManager::instance().registerThread(thread);

    DownloadHelper* helper = new DownloadHelper;
    helper->moveToThread(thread);

    QString shortManufacturer = m_currDriverInfo["shortManufacturer"].toString();
    QString silentParam = m_currDriverInfo["downloadSilentParam"].toString();

    QDir solutionPath = QCoreApplication::applicationDirPath();
    solutionPath.cdUp();

    QString pathToSaveSetup = QString(solutionPath.absolutePath() + "/setups/setup_%1.exe").arg(shortManufacturer);
    QString installDirSource = QString(solutionPath.absolutePath() + "/setups/%1").arg(shortManufacturer);

    qDebug() << "pathToSaveSetup " << pathToSaveSetup << " installDirSource " << installDirSource;

    connect(thread, &QThread::started, helper, [helper, url, pathToSaveSetup, installDirSource, silentParam]() {
        helper->downloadAndInstall(url, pathToSaveSetup, installDirSource, silentParam);
    });

    connect(helper, &DownloadHelper::downloadFinished, this, [this, thread, helper]() {

        thread->quit();
        thread->wait();
        thread->deleteLater();
        helper->deleteLater();

        QProcess::startDetached(qApp->arguments().at(0), qApp->arguments());
        QCoreApplication::quit();

        emit downloadFinished(true);
    });

    thread->start();
}

void DriverMaster :: deleteDriver(const QString& infPath) {

    if (infPath.endsWith(".inf")) {

        QProcess* deleteDriverProcess = new QProcess(this);
        QString deleteCommand = QString("pnputil /delete-driver %1 /uninstall /force").arg(infPath);

        qDebug() << "deleteCommand" << deleteCommand;

        connect(deleteDriverProcess, &QProcess::finished, this, [=]() {

            deleteDriverProcess->deleteLater();
            ThreadManager::instance().rebootPC();
            emit deleteFinished(true);
        });

        deleteDriverProcess->start("cmd.exe", QStringList() << "/C" << deleteCommand);
    } else {
        emit deleteFinished(false);
    }
}
