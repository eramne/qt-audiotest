import QtQuick
import QtQuick.Window
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import audiotest 1.0

Window {
    width: 1000
    height: 600
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
        property real cmFreqRatio: carrierFrequency/modulatorFrequency
        playing: false

        carrierAttack: carrierAttackDial.value;
        carrierDecay: carrierDecayDial.value;
        carrierSustain: carrierSustainDial.value;
        carrierRelease: carrierReleaseDial.value;

        modulatorAttack: modulatorAttackDial.value;
        modulatorDecay: modulatorDecayDial.value;
        modulatorSustain: modulatorSustainDial.value;
        modulatorRelease: modulatorReleaseDial.value;

        onCarrierFrequencyChanged: {
            modulatorFrequency = carrierFrequency * 1/cmFreqRatio
        }

        Component.onCompleted: {
            carrierFrequencyChanged();
            modulatorFrequencyChanged();

            start();
        }
    }

    Pane {
        id: pane
        anchors.fill: parent
        padding: 20
        focus: true

        Keys.enabled: true;
        Keys.onPressed: function (event) {
            if (event.key === Qt.Key_Space && !event.isAutoRepeat) {
                audio.setTime(0);
                audio.playing = true;
            }
        }
        Keys.onReleased: function (event) {
            if (event.key === Qt.Key_Space && !event.isAutoRepeat) {
                audio.timeReleased = audio.getTime();
                audio.playing = false;
            }
        }

        Flow {
            width: parent.width
            spacing: 20

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
                    let maxX = (10/440)*(width/600);
                    let minY = -1;
                    let maxY = 1;

                    let resolution = width;
                    let offsetX = audio.getTime()-(audio.getTime()%((2)/audio.carrierFrequency))
                    let carrierEnvelope = audio.envelope(audio.getTime(), audio.playing, audio.timeReleased, audio.carrierAttack, audio.carrierDecay, audio.carrierSustain, audio.carrierRelease);
                    let modulatorEnvelope = audio.envelope(audio.getTime(), audio.playing, audio.timeReleased, audio.modulatorAttack, audio.modulatorDecay, audio.modulatorSustain, audio.modulatorRelease);
                    for (let i = 0; i < resolution; i++) {
                        let canvasX = i*(width/resolution)
                        let theoreticalX = utils.map(canvasX, [0, width], [minX, maxX]) - offsetX
                        let theoreticalY = carrierEnvelope*audio.volume*Math.sin(  (Math.PI*audio.carrierFrequency*(theoreticalX))  +  modulatorEnvelope*audio.modulationIndex*Math.sin(Math.PI*audio.modulatorFrequency*(theoreticalX))  )
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
                                value: 0.5
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

                                Component.onCompleted: {
                                    audio.carrierFrequencyChanged.connect(function () {
                                        value = 1200*Math.log2(audio.carrierFrequency/6.875);
                                    });
                                }

                                onFreqChanged: {
                                    audio.carrierFrequency = freq;
                                }
                            }

                            TextField {
                                id: carrierFrequencyTextField
                                implicitWidth: 65
                                Layout.alignment: Qt.AlignVCenter
                                onAccepted: audio.carrierFrequency = text
                                Component.onCompleted: {
                                    audio.carrierFrequencyChanged.connect(function () {
                                        text = Math.round(audio.carrierFrequency*1000)/1000
                                    });
                                }
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

                    Column {
                        spacing: 10

                        Text {
                            text: "ADSR Envelope"
                        }

                        Canvas {
                            id: carrierEnvelopeCanvas
                            width: parent.width
                            height: 80

                            onPaint: {
                                const ctx = getContext("2d");
                                ctx.clearRect(0, 0, carrierEnvelopeCanvas.width, carrierEnvelopeCanvas.height);

                                ctx.strokeStyle = Qt.rgba(1, 0, 0, 1);
                                ctx.lineWidth = 1;
                                ctx.beginPath();

                                let minX = 0;
                                let maxX = 8;
                                let minY = 0;
                                let maxY = 1;

                                let resolution = width;
                                for (let i = 0; i < resolution; i++) {
                                    let canvasX = i*(width/resolution)
                                    let theoreticalX = utils.map(canvasX, [0, width], [minX, maxX])
                                    let theoreticalY = audio.envelope(theoreticalX, theoreticalX <= 6 ? true : false, 6, audio.carrierAttack, audio.carrierDecay, audio.carrierSustain, audio.carrierRelease);
                                    let canvasY = utils.map(theoreticalY, [minY, maxY], [height, 0]);
                                    ctx.lineTo(canvasX, canvasY);
                                }
                                ctx.stroke();

                                if (audio.playing || audio.getTime() <= maxX) {
                                    ctx.strokeStyle = Qt.rgba(0, 0, 0, 1);
                                    ctx.beginPath();
                                    let tmpX = -1;
                                    if (audio.playing && audio.getTime() <= audio.carrierAttack + audio.carrierDecay) {
                                        tmpX = utils.map(audio.getTime(), [minX, maxX], [0, width]);
                                    } else if (!audio.playing) {
                                        tmpX = utils.map(6 + audio.getTime() - audio.timeReleased, [minX, maxX], [0, width]);
                                    } else {
                                        tmpX = utils.map(audio.carrierAttack + audio.carrierDecay + Math.min(audio.getTime()/10, 6 - audio.carrierAttack - audio.carrierDecay), [minX, maxX], [0, width]);
                                    }


                                    ctx.moveTo(tmpX, 0);
                                    ctx.lineTo(tmpX, height);
                                    ctx.stroke();
                                }
                            }

                            Timer {
                                running: true
                                repeat: true
                                interval: 1000/30
                                onTriggered: carrierEnvelopeCanvas.requestPaint()
                            }
                        }

                        Row {
                            spacing: 10

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Attack"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: carrierAttackDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Decay"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: carrierDecayDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Sustain"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: carrierSustainDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.001
                                    to: 0.999
                                    value: 0.999
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Release"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: carrierReleaseDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
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

                                Component.onCompleted: {
                                    audio.modulatorFrequencyChanged.connect(function () {
                                        value = 1200*Math.log2(audio.modulatorFrequency/6.875);
                                    });
                                }

                                onFreqChanged: {
                                    audio.modulatorFrequency = freq;
                                }

                                /*onFreqChanged: {
                                    modulatorFrequencyTextField.text = Math.round(freq*1000)/1000
                                }*/
                            }

                            TextField {
                                id: modulatorFrequencyTextField
                                implicitWidth: 65
                                Layout.alignment: Qt.AlignVCenter
                                //onAccepted: modulatorFrequencySlider.value = 1200*Math.log2(text/6.875)
                                onAccepted: audio.modulatorFrequency = text
                                Component.onCompleted: {
                                    audio.modulatorFrequencyChanged.connect(function () {
                                        text = Math.round(audio.modulatorFrequency*1000)/1000
                                    });
                                }
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

                    Column {
                        spacing: 10

                        Text {
                            text: "ADSR Envelope"
                        }

                        Canvas {
                            id: modulatorEnvelopeCanvas
                            width: parent.width
                            height: 80

                            onPaint: {
                                const ctx = getContext("2d");
                                ctx.clearRect(0, 0, modulatorEnvelopeCanvas.width, modulatorEnvelopeCanvas.height);

                                ctx.strokeStyle = Qt.rgba(1, 0, 0, 1);
                                ctx.lineWidth = 1;
                                ctx.beginPath();

                                let minX = 0;
                                let maxX = 8;
                                let minY = 0;
                                let maxY = 1;

                                let resolution = width;
                                for (let i = 0; i < resolution; i++) {
                                    let canvasX = i*(width/resolution)
                                    let theoreticalX = utils.map(canvasX, [0, width], [minX, maxX])
                                    let theoreticalY = audio.envelope(theoreticalX, theoreticalX <= 6 ? true : false, 6, audio.modulatorAttack, audio.modulatorDecay, audio.modulatorSustain, audio.modulatorRelease);
                                    let canvasY = utils.map(theoreticalY, [minY, maxY], [height, 0]);
                                    ctx.lineTo(canvasX, canvasY);
                                }
                                ctx.stroke();

                                if (audio.playing || audio.getTime() <= maxX) {
                                    ctx.strokeStyle = Qt.rgba(0, 0, 0, 1);
                                    ctx.beginPath();
                                    let tmpX = -1;
                                    if (audio.playing && audio.getTime() <= audio.modulatorAttack + audio.modulatorDecay) {
                                        tmpX = utils.map(audio.getTime(), [minX, maxX], [0, width]);
                                    } else if (!audio.playing) {
                                        tmpX = utils.map(6 + audio.getTime() - audio.timeReleased, [minX, maxX], [0, width]);
                                    } else {
                                        tmpX = utils.map(audio.modulatorAttack + audio.modulatorDecay + Math.min(audio.getTime()/10, 6 - audio.modulatorAttack - audio.modulatorDecay), [minX, maxX], [0, width]);
                                    }


                                    ctx.moveTo(tmpX, 0);
                                    ctx.lineTo(tmpX, height);
                                    ctx.stroke();
                                }
                            }

                            Timer {
                                running: true
                                repeat: true
                                interval: 1000/30
                                onTriggered: modulatorEnvelopeCanvas.requestPaint()
                            }
                        }

                        Row {
                            spacing: 10

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Attack"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: modulatorAttackDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Decay"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: modulatorDecayDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Sustain"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: modulatorSustainDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.0001
                                    to: 0.999
                                    value: 0.999
                                    inputMode: Dial.Vertical
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Text {
                                    text: "Release"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Dial {
                                    id: modulatorReleaseDial
                                    implicitWidth: 60
                                    implicitHeight: 60
                                    from: 0.01
                                    to: 2
                                    value: 0.01
                                    inputMode: Dial.Vertical
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
