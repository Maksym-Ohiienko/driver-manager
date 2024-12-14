import QtQuick 2.15
import QtQuick.Controls.Fusion

CheckBox {
    id: root

    signal clicked()

    width: background.width + textMetrics.width + 10
    height: background.height

    TextMetrics {
        id: textMetrics
        text: root.text
        font: root.font
    }

    background: Rectangle {
        id: background

        width: indicator.width
        height: indicator.height
        border.width: 2
        border.color: root.checked ? "white" : "#404040"
        radius: 5
        color: "#0c0c0c"
    }

    indicator: Item {
        id: indicator
        width: 32
        height: 32

        Image {
            id: indicatorImage
            anchors.centerIn: parent
            source: "qrc:/Images/DriverListPageIcons/actionCross.png"
            smooth: true
            visible: root.checked

            width: parent.width - 8
            height: parent.height - 8
        }
    }

    contentItem: Text {
        id: contentText

        text: root.text
        font: root.font
        color: background.border.color

        verticalAlignment: Text.AlignVCenter
        leftPadding: text !== "" ? root.indicator.width + 5 : 0
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            root.clicked()
        }
    }
}
