import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    width: btnTxt.width + 10
    height: btnTxt.height + 5

    property color color: "lightgray"
    property alias containsMouse: btnMouseArea.containsMouse
    property alias radius: btnRect.radius
    property alias text: btnTxt.text
    property alias textColor: btnTxt.color
    property alias font: btnTxt.font
    property alias image: btnImage
    property alias border: btnRect.border

    signal clicked()
    signal entered()
    signal exited()

    Rectangle {
        id: btnRect
        radius: 3
        color: btnMouseArea.containsPress ? Qt.lighter(root.color, 1.2) : root.color
        smooth: true
        border.width: 0

        anchors.fill: parent

        MouseArea {
            id: btnMouseArea
            hoverEnabled: true

            anchors.fill: parent

            onClicked: {
                root.clicked()
            }

            onEntered: {
                root.entered()
                btnMouseArea.cursorShape = Qt.PointingHandCursor
            }
            onExited: {
                root.exited()
                btnMouseArea.cursorShape = Qt.ArrowCursor
            }
        }
    }
    Text {
        id: btnTxt
        anchors.centerIn: parent
        color: "black"
        font.bold: true
        font.pixelSize: 10
    }
    Image {
        id: btnImage
        anchors.centerIn: parent
        width: 48
        height: 48
    }
}
