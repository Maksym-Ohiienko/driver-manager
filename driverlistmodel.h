#ifndef DRIVERLISTMODEL_H
#define DRIVERLISTMODEL_H

#include <QAbstractListModel>
#include "drivermaster.h"

class DriverListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isActionRunning READ isActionRunning NOTIFY isActionRunningChanged)
    Q_PROPERTY(QString scanResults READ scanResults NOTIFY scanResultsChanged)

public:

    enum class Roles {

        DeviceClassRole     = Qt::UserRole + 1,
        DeviceNameRole      = Qt::UserRole + 2,
        NameRole            = Qt::UserRole + 3,
        ManufacturerRole    = Qt::UserRole + 4,
        CurrVersionRole     = Qt::UserRole + 5,
        NewVersionRole      = Qt::UserRole + 6,
        StateRole           = Qt::UserRole + 7,
        IsOutsiderRole      = Qt::UserRole + 8
    };
    Q_ENUM(Roles)

    explicit DriverListModel(QObject* parent = nullptr);
    ~DriverListModel();

    void setDrivers(const QVector<QSharedPointer<Driver>>& drivers);

    Q_INVOKABLE void actionDriver(int row); // Конкретний драйвер
    Q_INVOKABLE void scanDrivers();

    Q_INVOKABLE QVector<QSharedPointer<Driver>> drivers() const;
    Q_INVOKABLE void startDeleteDriver(int row);

    void setDriverActual(const QString& manufacturer);
    void updateScanResults();
    void updateModel();
    void removeItem(int row);
    void removeItemsByName(const QString& name);

    int columnCount(const QModelIndex& parent)                                              const override;
    int rowCount(const QModelIndex& parent = QModelIndex())                                 const override;
    bool isActionRunning() const;
    QString scanResults() const;

    QHash<int, QByteArray> roleNames()                                                      const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)                     const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex())   const override;

private slots:
    void onDownloadFinished(bool success);
    void onDeleteFinished(bool success);

signals:
    void scanResultsChanged();
    void isActionRunningChanged();
    void downloadFailed();
    void deleteFailed();

private:
    QVector<QSharedPointer<Driver>>     m_drivers;
    QSharedPointer<Driver>              m_currentDriver;
    QString                             m_scanResults;
    DriverMaster*                       m_driverMaster;
    bool                                m_isActionRunning;

    QString calculateScanResults() const;
    void resetScanResults();
    void setIsActionRunning(bool isRunning);

    void downloadDriver();
    void deleteDriver  ();
};

#endif // DRIVERLISTMODEL_H
