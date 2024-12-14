import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: mainWindow

    property string appFontFamily: "Bowler"
    visible: true
    minimumWidth: 900
    minimumHeight: 600

    flags: !(driverListModel && driverListModel.isActionRunning) ? Qt.Window : Qt.FramelessWindowHint | Qt.Window

    Rectangle {
        id: mainWindowContent

        anchors.fill: parent
        color: mainPage.backgroundColor

        Text {
            id: caption
            text: "Driver Manager 1.0"
            font.pixelSize: 14
            font.family: mainWindow.appFontFamily
            color: "white"

            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Main View
        StackView {
            id: mainView
            initialItem: mainPage

            anchors.top: caption.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    MainPage {
        id: mainPage
    }

    DriverListPage {
        id: driverListPage
    }

    onClosing: {
        threadManager.stopAllThreads();
    }
}


