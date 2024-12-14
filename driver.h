#ifndef DRIVER_H
#define DRIVER_H

#include <QString>
#include <QVariant>
#include <QSharedPointer>

class Driver
{

public:
    enum class State
    {
        Unknown,
        Download,
        Update,
        Actual
    };

    QString deviceName() const;

    QString name() const;

    QString manufacturer() const;

    QString currVersion() const;

    QString newVersion() const;

    QString stateStr() const;

    State state() const;

    bool isOutsider() const;

    QString deviceClass() const;

    Driver(const QString& deviceClass = "Empty", const QString& deviceName = "Empty", const QString& name = "Empty", const QString& manufacturer = "Empty",
           const QString& currVersion = "Empty", const QString& newVersion = "Empty", State state = State::Unknown, bool isOutsider = false);

    Driver(const Driver& other);
    Driver& operator= (const Driver& other);

    void setDeviceName(const QString& newDeviceName);
    void setState(State newState);
    void setNewVersion(const QString& newVersion);
    void setCurrVersion(const QString& newVersion);
    void setDeviceClass(const QString& newDeviceClass);

private:
    QString m_deviceClass;
    QString m_deviceName;
    QString m_name;
    QString m_manufacturer;
    QString m_currVersion;
    QString m_newVersion;
    State   m_state;
    bool    m_isOutsider;
};

#endif // DRIVER_H
