#include "testaudiothing.h"

TestAudioThing::TestAudioThing(QObject *parent) : QObject(parent)
{

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    QAudioFormat format = info.preferredFormat();
    format.setSampleRate(44100);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioSink(format, this);
    data = new AudioStream(this, audio);
    data->open(QIODevice::ReadWrite);
    QObject::connect(audio, &QAudioSink::stateChanged, this, [=](){
        qDebug() << audio->state() << audio->error() << audio->bufferSize();
    });
    audio->setBufferSize(2);
    audio->start(data);

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [=](){
        qDebug() << audio->state() << audio->error() << audio->bufferSize() << data->size() << audio->bytesFree() << data->isOpen() << data->isReadable() << data->pos() << audio->processedUSecs() << format.sampleFormat();
    });
    timer->start(1000);
}
