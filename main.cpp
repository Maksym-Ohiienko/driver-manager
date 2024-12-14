#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QFontDatabase>
#include "driverlistmodel.h"

int main(int argc, char** argv) {

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/Images/MainPageIcons/mainIcon.png"));
    QFontDatabase::addApplicationFont(":/Fonts/fonts/Bowler.ttf");

    QQmlApplicationEngine engine;

    DriverListModel driverListModel;
    engine.rootContext()->setContextProperty("driverListModel", &driverListModel);
    engine.rootContext()->setContextProperty("threadManager", &ThreadManager::instance());

    engine.load(QUrl("qrc:/QML/qml/Main.qml"));

    return app.exec();
}
