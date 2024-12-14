import QtQuick 2.15
import QtQuick.Controls 2.15

SimplePage {
    id: root

    property int progressValue: 0
    property int progressMax: 100

    Timer {
        id: progressTimer
        interval: 1000
        repeat: true
        running: false

        onTriggered: {
            if (root.progressValue < root.progressMax / 3) {
                root.progressValue += 5;
            } else if (root.progressValue < root.progressMax / 2) {
                root.progressValue += 10;
            } else if (root.progressValue < root.progressMax) {
                root.progressValue += 15;
            } else {

                if (driverListModel.scanResults === "") {
                    loadingText.textVisible = true;
                    return;
                }

                progressTimer.stop()
                root.progressValue = 0
                scanBtn.enabled = true
                loadingText.textVisible = false
                mainView.push(driverListPage)
            }
        }
    }

    SimpleButton {
        id: scanBtn
        color: "transparent"
        radius: width / 2

        width: 150
        height: width
        border.color: "#FF2400"
        border.width: 5

        image.source: "qrc:/Images/MainPageIcons/scan.png"
        image.width: 64
        image.height: 64

        anchors.centerIn: parent

        onClicked: {

            if (progressTimer.running)
                return

            driverListModel.scanDrivers()

            scanBtn.enabled = false
            progressTimer.start()
        }

        states: [
            State {
                name: "entered"
                when: scanBtn.containsMouse || progressTimer.running

                PropertyChanges {
                    target: scanBtn
                    color: "#FF2400"
                }
            },
            State {
                name: "exited"
                when: !scanBtn.containsMouse && !progressTimer.running

                PropertyChanges {
                    target: scanBtn
                    color: "transparent"
                }
            }
        ]

        transitions: [
            Transition {
                from: "entered"
                to: "exited"

                ParallelAnimation {
                    ColorAnimation { target: scanBtn; property: "color"; duration: 500; easing.type: Easing.OutSine }
                }
            },
            Transition {
                from: "exited"
                to: "entered"

                ParallelAnimation {
                    ColorAnimation { target: scanBtn; property: "color"; duration: 500; easing.type: Easing.OutSine }
                }
            }
        ]
    }

    LoadingText {
        id: loadingText

        loadingText: "Checking for updates"
        textVisible: false
        isRunning: true
        textFont.pixelSize: 16

        anchors.top: scanBtn.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: scanBtn.horizontalCenter
    }

    AnimatedImage {
        id: animatedScan

        width: 200
        height: 200
        source: "qrc:/Gifs/gifs/scanning.gif"
        visible: loadingText.textVisible

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
    }

    Canvas {
        id: progressCircle
        width: scanBtn.width + 20
        height: scanBtn.height + 20
        anchors.centerIn: scanBtn

        onPaint: {
            var ctx = progressCircle.getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var startAngle = -Math.PI / 2
            var endAngle = startAngle + (2 * Math.PI * root.progressValue / root.progressMax)

            ctx.strokeStyle = "#FF2400"
            ctx.lineWidth = 8
            ctx.beginPath()
            ctx.arc(width / 2, height / 2, width / 2 - ctx.lineWidth, startAngle, endAngle, false)
            ctx.stroke()
        }

        Connections {
            target: root
            function onProgressValueChanged () {
                progressCircle.requestPaint()
            }
        }
    }
}
