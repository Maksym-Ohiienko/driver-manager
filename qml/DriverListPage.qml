import QtQuick 2.15
import QtQuick.Dialogs
import QtQuick.Controls.Fusion

SimplePage {
    id: root

    SimpleButton {
        id: backBtn

        image.source: "qrc:/Images/DriverListPageIcons/back.png"
        image.width: 32
        image.height: 32

        width: image.width
        height: width
        color: "transparent"
        enabled: driverListModel ? !driverListModel.isActionRunning : false

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 10

        onClicked: {
            mainView.pop()
        }
    }

    SimpleButton {
        id: rebootBtn

        image.source: "qrc:/Images/DriverListPageIcons/reboot.png"
        image.width: 32
        image.height: 32

        width: image.width
        height: width
        color: "transparent"
        enabled: driverListModel ? !driverListModel.isActionRunning : false

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 10

        onClicked: {

            rebootConfirm.x = mainWindow.width / 2 - rebootConfirm.width / 2
            rebootConfirm.y = mainWindow.height / 2 - rebootConfirm.height / 2
            rebootConfirm.open()
        }
    }

    Image {
        id: scanResultsImage

        source: "qrc:/Images/DriverListPageIcons/information.png"
        width: 42
        height: 42
        smooth: true

        anchors.top: backBtn.bottom
        anchors.topMargin: 80
        anchors.left: backBtn.left
    }

    Text {
        id: scanResultsInfo

        text: driverListModel ? driverListModel.scanResults : "Empty"

        font.pixelSize: 14
        font.family: mainWindow.appFontFamily
        color: "#FF2400"
        visible: driverListModel ? !driverListModel.isActionRunning : false

        anchors.left: scanResultsImage.right
        anchors.leftMargin: 10
        anchors.verticalCenter: scanResultsImage.verticalCenter
    }

    LoadingText {
        id: loadingText

        textVisible: driverListModel ? driverListModel.isActionRunning : false
        isRunning: driverListModel ? driverListModel.isActionRunning : false

        anchors.left: scanResultsImage.right
        anchors.leftMargin: 10
        anchors.verticalCenter: scanResultsImage.verticalCenter
    }

    AnimatedImage {
        id: loadingGif
        source: "qrc:/Gifs/gifs/loading.gif"
        width: 150
        height: 150
        visible: driverListModel ? driverListModel.isActionRunning : false

        anchors.right: parent.right
        anchors.rightMargin: backBtn.anchors.leftMargin
        anchors.bottom: scanResultsImage.bottom
    }

    ListView {
        id: driverList

        width: root.width - backBtn.anchors.leftMargin * 2

        model: driverListModel
        clip: true

        anchors.left: scanResultsImage.left
        anchors.top: loadingGif.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom

        delegate: Item {

            id: listDelegate
            width: driverList.width
            height: model.state !== "Unknown" ? 80 : 0
            visible: model.state !== "Unknown"

            Rectangle {

                radius: 5
                color: "#333333"

                anchors.fill: parent
                anchors.bottomMargin: 5
                anchors.topMargin: 5

                Image {
                    id: deviceImage

                    source: "qrc:/Images/PNPDeviceIcons/" + (model.deviceClass !== null && model.deviceClass !== "" && model.deviceClass !== "Empty" ?
                                                                 model.deviceClass : "Unknown") + ".png"
                    width: 32
                    height: 32
                    smooth: true

                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: action.verticalCenter
                }

                Text {
                    id: deviceName
                    color: "white"

                    text: model.deviceName
                    font.pixelSize: 10
                    font.family: mainWindow.appFontFamily

                    anchors.top: action.top
                    anchors.left: deviceImage.right
                    anchors.leftMargin: 10
                }
                Text {
                    id: name
                    color: "#808080"

                    text: model.name
                    font.pixelSize: 10
                    font.family: mainWindow.appFontFamily

                    anchors.left: deviceName.left
                    anchors.bottom: action.bottom
                }
                Text {
                    id: manufacturer
                    color: "#808080"

                    text: model.manufacturer
                    font.pixelSize: 10
                    font.family: mainWindow.appFontFamily

                    anchors.left: name.right
                    anchors.leftMargin: 10
                    anchors.bottom: name.bottom
                }

                Text {
                    id: currVersion
                    color: "#666666"

                    text: "Current: " + model.currVersion
                    font.pixelSize: 10
                    font.family: mainWindow.appFontFamily

                    anchors.verticalCenter: model.state === "Actual" ? parent.verticalCenter : undefined
                    anchors.top: model.state === "Actual" ? undefined : action.top
                    anchors.right: action.left
                    anchors.rightMargin: 10
                }

                Text {
                    id: newVersion
                    color: "#808080"

                    visible: model.state === "Download" || model.state === "Update"
                    text: "Available: " + model.newVersion
                    font.pixelSize: 10
                    font.family: mainWindow.appFontFamily

                    anchors.right: currVersion.right
                    anchors.bottom: action.bottom
                }

                SimpleButton {
                    id: action

                    text: model.state
                    textColor: model.state !== "Actual" ? action.textColor : "#808080"
                    font.pixelSize: model.state !== "Actual" ? action.font.pixelSize : 14
                    font.family: mainWindow.appFontFamily

                    width: 100
                    height: 50
                    color: model.state !== "Actual" ? action.color : "transparent"
                    enabled: model.state !== "Actual" && !driverListModel.isActionRunning

                    anchors.right: deleteAction.visible ? deleteAction.left : parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 10

                    onClicked: {

                        Qt.callLater(function() {
                            if (driverListModel && !driverListModel.isActionRunning) {
                                driverListModel.actionDriver(index);
                            }
                        });
                    }
                }

                SimpleButton {

                    id: deleteAction

                    width: model.state === "Actual" ? 24 : action.height
                    height: model.state === "Actual" ? 24 : action.height
                    radius: width / 2
                    color: action.color
                    visible: model.isOutsider
                    enabled: !driverListModel.isActionRunning
                    image.source: "qrc:/Images/DriverListPageIcons/delete.png"
                    image.width: model.state === "Actual" ? 24 : 32
                    image.height: model.state === "Actual" ? 24 : 32

                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 10

                    onClicked: {

                        deleteConfirm.x = mainWindow.width / 2 - deleteConfirm.width / 2
                        deleteConfirm.y = mainWindow.height / 2 - deleteConfirm.height / 2
                        deleteConfirm.rowToDelete = index
                        deleteConfirm.open()
                    }
                }
            }
        }
    }

    SimpleMessageBox {

        id: errorMb
        titleText: "Unable to perform operation"
    }

    SimpleMessageBox {

        id: rebootConfirm

        titleText: "Confirm system reboot"
        messageText: "Are you sure?"
        yesNoMB: true

        onAccepted: {

            threadManager.rebootPC();
        }
    }

    SimpleMessageBox {

        id: deleteConfirm

        property var rowToDelete: undefined

        titleText: "Confirm the action"
        width: 400
        messageText: "Are you sure?\nPC will auto restart!"
        yesNoMB: true

        onAccepted: {

            if (driverListModel) {
                driverListModel.startDeleteDriver(rowToDelete)
            }
        }
    }

    Connections {

        target: driverListModel ? driverListModel : null

        function onDownloadFailed() {

            errorMb.messageText = "Unknown device...";
            errorMb.x = mainWindow.width / 2 - errorMb.width / 2;
            errorMb.y = mainWindow.height / 2 - errorMb.height / 2;
            errorMb.open();
        }

        function onDeleteFailed() {

            errorMb.messageText = "Unknown driver...";
            errorMb.x = mainWindow.width / 2 - errorMb.width / 2;
            errorMb.y = mainWindow.height / 2 - errorMb.height / 2;
            errorMb.open();
        }
    }
}
