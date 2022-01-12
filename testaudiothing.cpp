#include "testaudiothing.h"

TestAudioThing::TestAudioThing(QObject *parent) : QObject(parent)
{

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    QAudioFormat format = info.preferredFormat();
    format.setSampleFormat(QAudioFormat::Int16);

    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioSink(format, this);
    data = new AudioStream(this, audio);
    data->open(QBuffer::ReadWrite);
    //audio->setBufferSize(2);
    audio->start(data);
}
