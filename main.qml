import QtQuick
import QtQuick.Window
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import audiotest 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    QtObject {
        id: utils

        function map(value, from, to) {
            return to[0] + (value - from[0]) * (to[1] - to[0]) / (from[1] - from[0]);
        }
    }

    AudioController {
        id: audio
        volume: carrierVolumeSlider.value
        carrierFrequency: carrierFrequencySlider.freq
        modulationIndex: modulatorStrengthSlider.value
        modulatorFrequency: modulatorFrequencySlider.freq

        Component.onCompleted: {
            start();
        }
    }

    Pane {
        anchors.fill: parent
        padding: 20

        Flow {
            width: parent.width

            Canvas {
                id: canvas
                width: parent.width
                height: 200

                onPaint: {
                    const ctx = getContext("2d");
                    ctx.clearRect(0, 0, canvas.width, canvas.height);

                    ctx.strokeStyle = Qt.rgba(1, 0, 0, 1);
                    ctx.lineWidth = 1;
                    ctx.beginPath();

                    let originY = height/2;
                    let minX = 0;
                    let maxX = 10/440;
                    let minY = -1;
                    let maxY = 1;

                    let resolution = width;
                    for (let i = 0; i < resolution; i++) {
                        let canvasX = i*(width/resolution)
                        let theoreticalX = utils.map(canvasX, [0, width], [minX, maxX])
                        let theoreticalY = audio.volume*Math.sin((Math.PI*audio.carrierFrequency*(/*audio.getTime()*/+theoreticalX))+audio.modulationIndex*Math.sin(Math.PI*audio.modulatorFrequency*(audio.getTime()+theoreticalX)))
                        let canvasY = utils.map(theoreticalY, [minY, maxY], [0, height])
                        ctx.lineTo(canvasX, canvasY);
                    }
                    ctx.stroke();
                }
                Timer {
                    running: true
                    repeat: true
                    interval: 1
                    onTriggered: canvas.requestPaint()
                }
            }


            GroupBox {
                title: "Carrier"
                implicitWidth: 400

                ColumnLayout {
                    spacing: 10

                    Column {
                        Text {
                            text: "Volume"
                        }

                        RowLayout {
                            spacing: 5
                            Slider {
                                id: carrierVolumeSlider
                                from: 0
                                stepSize: 0.05
                                snapMode: Slider.SnapAlways
                                to: 1
                                Layout.alignment: Qt.AlignVCenter
                            }

                            Text {
                                text: Math.floor(carrierVolumeSlider.value*1000)/1000
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }

                    Column {
                        Text {
                            text: "Frequency"
                        }

                        RowLayout {
                            spacing: 5
                            Slider {
                                id: carrierFrequencySlider
                                implicitWidth: 300
                                from: 0
                                to: 14000
                                stepSize: 100
                                snapMode: Slider.SnapAlways
                                value: 1200*Math.log2(220/6.875)
                                property real freq: 6.875*Math.pow(2, value/1200)
                                Layout.alignment: Qt.AlignVCenter

                                onFreqChanged: {
                                    carrierFrequencyTextField.text = Math.round(freq*1000)/1000
                                }
                            }

                            TextField {
                                id: carrierFrequencyTextField
                                implicitWidth: 65
                                Layout.alignment: Qt.AlignVCenter
                                onAccepted: carrierFrequencySlider.value = 1200*Math.log2(text/6.875)
                                validator: DoubleValidator {
                                    decimals: 3
                                    notation: DoubleValidator.StandardNotation
                                    bottom: 0
                                    top: 20000
                                }
                                selectByMouse: true
                            }
                        }
                    }
                }
            }


            GroupBox {
                title: "Modulator"
                implicitWidth: 400

                ColumnLayout {
                    spacing: 10

                    Column {
                        Text {
                            text: "Strength"
                        }

                        RowLayout {
                            spacing: 5
                            Slider {
                                id: modulatorStrengthSlider
                                implicitWidth: 300
                                from: 0
                                stepSize: 0.05
                                snapMode: Slider.SnapAlways
                                to: 30
                                Layout.alignment: Qt.AlignVCenter
                            }

                            Text {
                                text: Math.floor(modulatorStrengthSlider.value*1000)/1000
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }

                    Column {
                        Text {
                            text: "Frequency"
                        }

                        RowLayout {
                            spacing: 5
                            Slider {
                                id: modulatorFrequencySlider
                                implicitWidth: 300
                                from: 0
                                to: 14000
                                stepSize: 100
                                snapMode: Slider.SnapAlways
                                value: 1200*Math.log2(220/6.875)
                                property real freq: 6.875*Math.pow(2, value/1200)
                                Layout.alignment: Qt.AlignVCenter

                                onFreqChanged: {
                                    modulatorFrequencyTextField.text = Math.round(freq*1000)/1000
                                }
                            }

                            TextField {
                                id: modulatorFrequencyTextField
                                implicitWidth: 65
                                Layout.alignment: Qt.AlignVCenter
                                onAccepted: modulatorFrequencySlider.value = 1200*Math.log2(text/6.875)
                                validator: DoubleValidator {
                                    decimals: 3
                                    notation: DoubleValidator.StandardNotation
                                    bottom: 0
                                    top: 20000
                                }
                                selectByMouse: true
                            }
                        }
                    }
                }
            }
        }
    }
}
