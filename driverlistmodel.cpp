#include "driverlistmodel.h"

DriverListModel :: DriverListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_driverMaster = new DriverMaster;
    m_isActionRunning = false;

    connect(m_driverMaster, &DriverMaster::downloadFinished, this, &DriverListModel::onDownloadFinished);
    connect(m_driverMaster, &DriverMaster::deleteFinished, this, &DriverListModel::onDeleteFinished);
}

DriverListModel :: ~DriverListModel()
{
    delete m_driverMaster;
}

void DriverListModel :: setDrivers(const QVector<QSharedPointer<Driver>>& drivers) {

    beginResetModel();
    m_drivers = drivers;
    endResetModel();

    qDebug() << "Count of drivers:" << m_drivers.count();
}

void DriverListModel :: setDriverActual(const QString& manufacturer) {

    for (int row = 0; row < rowCount(); ++row) {

        QSharedPointer<Driver> driver = m_drivers[row];

        if (driver->manufacturer() == manufacturer) {
            driver->setState(Driver::State::Actual);

            QModelIndex index = createIndex(row, 0);
            emit dataChanged(index, index);
        }
    }
}

void DriverListModel :: updateScanResults() {

    m_scanResults = calculateScanResults();
    emit scanResultsChanged();
}

void DriverListModel :: onDownloadFinished(bool success) {

    setIsActionRunning(false);

    if (success) {

        setDriverActual(m_currentDriver->manufacturer());
        m_currentDriver->setCurrVersion(m_currentDriver->newVersion());

        updateScanResults();

    } else {
        emit downloadFailed();
    }
}

void DriverListModel :: downloadDriver() {

    setIsActionRunning(true);

    m_driverMaster->downloadDriver(m_currentDriver->manufacturer());
}

void DriverListModel :: onDeleteFinished(bool success) {

    setIsActionRunning(false);

    if (success) {

        QString driverName = m_currentDriver->name();
        removeItemsByName(driverName);

        updateScanResults();

    } else {
        emit deleteFailed();
    }
}

void DriverListModel :: deleteDriver() {

    setIsActionRunning(true);

    m_driverMaster->deleteDriver(m_currentDriver->name());
}

void DriverListModel :: startDeleteDriver(int row) {

    m_currentDriver = m_drivers[row];

    deleteDriver();
}

void DriverListModel :: setIsActionRunning(bool isRunning) {

    m_isActionRunning = isRunning;
    emit isActionRunningChanged();
}

void DriverListModel :: removeItem(int row) {

    beginRemoveRows(QModelIndex(), row, row);
    m_drivers.removeAt(row);
    endRemoveRows();
}

void DriverListModel :: removeItemsByName(const QString& name) {

    for (int row = rowCount() - 1; row >= 0; --row) {

        QSharedPointer<Driver> driver = m_drivers[row];

        if (driver->name() == name) {

            removeItem(row);
        }
    }
}

void DriverListModel :: actionDriver(int row) {

    m_currentDriver = m_drivers[row];

    switch (m_currentDriver->state()) {

        case Driver::State::Update:
        case Driver::State::Download:
        {
            downloadDriver();
        }
        break;

        default: break;
    }
}

void DriverListModel :: resetScanResults() {
    m_scanResults = "";
}

void DriverListModel :: scanDrivers() {

    resetScanResults();
    setIsActionRunning(true);

    QFutureWatcher<QVector<QSharedPointer<Driver>>>* watcher = new QFutureWatcher<QVector<QSharedPointer<Driver>>>(this);

    QFuture<QVector<QSharedPointer<Driver>>> future = QtConcurrent::run([this]() {
        return m_driverMaster->listDrivers();
    });

    connect(watcher, &QFutureWatcher<QVector<QSharedPointer<Driver>>>::finished, this, [this, watcher]() {
        QVector<QSharedPointer<Driver>> drivers = watcher->result();
        setDrivers(drivers);
        m_driverMaster->checkForUpdates();
        updateScanResults();
        setIsActionRunning(false);

        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

QModelIndex DriverListModel :: index(int row, int column, const QModelIndex& parent) const {
    Q_UNUSED(parent)

    if (row < 0 || row >= rowCount()) {
        return QModelIndex();
    }

    return createIndex(row, column, nullptr);
}

QVector<QSharedPointer<Driver>> DriverListModel :: drivers() const {
    return m_drivers;
}

QString DriverListModel :: scanResults() const {
    return m_scanResults;
}

QString DriverListModel :: calculateScanResults() const {

    int actual      = 0; // Кількість актуальних пакетів
    int needInstall = 0; // Кількість пристроїв без драйверу
    int needUpdate  = 0; // Кількість застарілих пакетів
    int outsiders   = 0; // Кількість сторонніх пакетів

    for (const QSharedPointer<Driver> driver : m_drivers) {

        switch (driver->state()) {

            case Driver::State::Actual:
                actual++;
                break;
            case Driver::State::Download:
                needInstall++;
                break;
            case Driver::State::Update:
                needUpdate++;
                break;

            default:
                break;
        }

        if (driver->isOutsider()) {
            outsiders++;
        }
    }

    return QString("%1 devices without driver, %2 are obsolete, %3 are outsiders, and %4 actual")
        .arg(needInstall)
        .arg(needUpdate)
        .arg(outsiders)
        .arg(actual);
}

int DriverListModel :: columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return 0;
}

int DriverListModel :: rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return m_drivers.count();
}

bool DriverListModel :: isActionRunning() const {
    return m_isActionRunning;
}

QHash<int, QByteArray> DriverListModel :: roleNames() const {

    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();

    roles.insert(static_cast<int>(Roles::DeviceClassRole),    "deviceClass");
    roles.insert(static_cast<int>(Roles::DeviceNameRole),     "deviceName");
    roles.insert(static_cast<int>(Roles::NameRole),           "name");
    roles.insert(static_cast<int>(Roles::ManufacturerRole),   "manufacturer");
    roles.insert(static_cast<int>(Roles::CurrVersionRole),    "currVersion");
    roles.insert(static_cast<int>(Roles::NewVersionRole),     "newVersion");
    roles.insert(static_cast<int>(Roles::StateRole),          "state");
    roles.insert(static_cast<int>(Roles::IsOutsiderRole),     "isOutsider");

    return roles;
}

QVariant DriverListModel :: data(const QModelIndex& index, int role) const {

    if (!index.isValid() || index.row() < 0 || index.row() >= m_drivers.size()) {
        return QVariant();
    }

    QSharedPointer<Driver> driver = m_drivers[index.row()];

    if (role == Qt::DisplayRole) {
        return driver->name();
    } else if (role == static_cast<int>(Roles::DeviceClassRole)) {
        return driver->deviceClass();
    } else if (role == static_cast<int>(Roles::DeviceNameRole)) {
        return driver->deviceName();
    } else if (role == static_cast<int>(Roles::NameRole)) {
        return driver->name();
    } else if (role == static_cast<int>(Roles::ManufacturerRole)) {
        return driver->manufacturer();
    } else if (role == static_cast<int>(Roles::CurrVersionRole)) {
        return driver->currVersion();
    } else if (role == static_cast<int>(Roles::NewVersionRole)) {
        return driver->newVersion();
    } else if (role == static_cast<int>(Roles::StateRole)) {
        return driver->stateStr();
    } else if (role == static_cast<int>(Roles::IsOutsiderRole)) {
        return driver->isOutsider();
    }

    return QVariant();
}

void DriverListModel :: updateModel() {

    QModelIndex first = index(0, 0);
    QModelIndex last = index(rowCount() - 1, 0);
    emit dataChanged(first, last);
}
