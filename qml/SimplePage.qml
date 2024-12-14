import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    property alias backgroundColor: backgroundRect.color
    visible: false

    background: Rectangle {
        id: backgroundRect
        color: "#1a1a1a"
    }
}
