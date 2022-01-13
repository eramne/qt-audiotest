import QtQuick
import QtQuick.Window
import audiotest 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    AudioController {
        volume: 0.5
        modulatorFrequency: 4
        modulationIndex: 10

        Component.onCompleted: {
            start();
        }
    }
}
