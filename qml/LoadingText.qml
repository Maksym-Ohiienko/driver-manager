import QtQuick 2.15
import QtQuick.Controls.Fusion

Item {
    id: root

    width: text.width + 5
    height: text.height + 10

    property string loadingText: "Loading"
    property alias textFont: text.font
    property alias textVisible: text.visible
    property alias isRunning: timer.running

    Text {
        id: text
        text: root.loadingText
        color: "#FF2400"
        font.pixelSize: 18
        font.family: mainWindow.appFontFamily

        anchors.centerIn: parent
    }
    Timer {
        id: timer
        interval: 500
        repeat: true

        onTriggered: {
            if (text.text.endsWith("...")) {
                text.text = root.loadingText;
            } else {
                text.text += ".";
            }
        }
    }
}
