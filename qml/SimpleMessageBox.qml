import QtQuick 2.15
import QtQuick.Dialogs
import QtQuick.Controls.Fusion

Dialog {
    id: root

    width: 300
    height: 200
    modal: Qt.WindowModal

    property alias  titleText: headerText.text
    property alias  messageText: message.text
    property alias  messageTextFont: message.font
    property bool   yesNoMB: false

    signal accepted()
    signal rejected()

    background: Rectangle {
        radius: 20
        border.color: "#1a1a1a"
        border.width: 3
        color: "#FF2400"
    }

    header: Rectangle {
        width: parent.width
        height: 40
        color: "transparent"
        opacity: 0.8
        border.color: "#1a1a1a"
        border.width: 3
        radius: 20

        Text {
            id: headerText
            font.pixelSize: 14
            font.bold: true
            font.family: appFontFamily
            color: "white"
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 15
        }
    }

    Text {
        id: message
        font.pixelSize: 19
        font.bold: true
        font.family: appFontFamily
        color: "white"
        anchors.centerIn: parent
    }

    footer: Item {

        id: footerRow
        height: 40
        width: root.width

        SimpleButton {

            id: noBtn
            text: "NO"
            visible: root.yesNoMB
            font.pixelSize: 14
            font.family: appFontFamily
            color: "transparent"
            textColor: "white"
            opacity: 0.8

            anchors.right: okBtn.left
            anchors.rightMargin: 10
            anchors.verticalCenter: okBtn.verticalCenter

            width: 100
            height: footerRow.height / 2

            onClicked: {

                root.visible = false
                root.rejected()
            }
        }

        SimpleButton {

            id: okBtn
            text: noBtn.visible ? "YES" : "OK"
            font.pixelSize: 14
            font.bold: true
            font.family: appFontFamily
            color: "transparent"
            textColor: "white"

            anchors.right: footerRow.right
            anchors.rightMargin: 10
            anchors.bottom: footerRow.bottom
            anchors.bottomMargin: 15

            width: 100
            height: footerRow.height / 2

            onClicked: {

                root.visible = false
                root.accepted()
            }
        }
    }
}
