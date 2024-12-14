#include "driver.h"

Driver :: Driver(const QString& deviceClass, const QString& deviceName, const QString& name, const QString& manufacturer, const QString& currVersion,
                const QString& newVersion, State state, bool isOutsider)
    : m_deviceClass(deviceClass), m_deviceName(deviceName), m_name(name), m_manufacturer(manufacturer), m_currVersion(currVersion),
    m_newVersion(newVersion), m_state(state), m_isOutsider(isOutsider)
{

}

Driver :: Driver(const Driver& other) : m_deviceClass(other.deviceClass()), m_deviceName(other.deviceName()), m_name(other.name()), m_manufacturer(other.manufacturer()), m_currVersion(other.currVersion()),
    m_newVersion(other.newVersion()), m_state(other.state()), m_isOutsider(other.isOutsider())
{ }

Driver& Driver :: operator=(const Driver& other) {

    if (this == &other)
        return *this;

    m_deviceClass   = other.deviceClass();
    m_deviceName    = other.deviceName();
    m_name          = other.name();
    m_manufacturer  = other.manufacturer();
    m_currVersion   = other.currVersion();
    m_newVersion    = other.newVersion();
    m_state         = other.state();
    m_isOutsider    = other.isOutsider();

    return *this;
}

QString Driver :: stateStr() const {

    switch (m_state) {

        case State::Download:  return "Download";
        case State::Update:    return "Update";
        case State::Actual:    return "Actual";

        default:               return "Unknown";
    }
}

QString Driver :: deviceName() const {
    return m_deviceName;
}

QString Driver :: name() const {
    return m_name;
}

QString Driver :: manufacturer() const {
    return m_manufacturer;
}

QString Driver :: currVersion() const {
    return m_currVersion;
}

QString Driver :: newVersion() const {
    return m_newVersion;
}

Driver::State Driver :: state() const {
    return m_state;
}

bool Driver :: isOutsider() const {
    return m_isOutsider;
}

QString Driver :: deviceClass() const {
    return m_deviceClass;
}

void Driver :: setDeviceClass(const QString& newDeviceClass) {
    m_deviceClass = newDeviceClass;
}

void Driver :: setDeviceName(const QString& newDeviceName) {
    m_deviceName = newDeviceName;
}

void Driver :: setState(State newState) {
    m_state = newState;
}

void Driver :: setNewVersion(const QString& newVersion) {
    m_newVersion = newVersion;
}

void Driver :: setCurrVersion(const QString& newVersion) {
    m_currVersion = newVersion;
}


