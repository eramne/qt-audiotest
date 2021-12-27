#include "testaudiothing.h"

TestAudioThing::TestAudioThing(QObject *parent) : QObject(parent)
{

    /*sourceFile.setFileName("C:/Users/student/Downloads/pcm0808s.wav");
    sourceFile.open(QIODevice::ReadOnly);
    qDebug() << sourceFile.exists();*/

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    QAudioFormat format = info.preferredFormat();
    format.setSampleRate(44100);
    format.setSampleFormat(QAudioFormat::Int16);
    /*qInfo() << "description:" << info.description();
    qInfo() << "id:" << info.id();
    qInfo() << "isDefault:" << info.isDefault();
    qInfo() << "maximumChannelCount:" << info.maximumChannelCount();
    qInfo() << "maximumSampleRate:" << info.maximumSampleRate();
    qInfo() << "prefferedSampleRate:" << info.preferredFormat().sampleRate();
    qInfo() << "prefferedChannelCount:" << info.preferredFormat().channelCount();
    qInfo() << "minimumChannelCount:" << info.minimumChannelCount();
    qInfo() << "minimumSampleRate:" << info.minimumSampleRate();
    qInfo() << "mode:" << info.mode();
    qInfo() << "supportedSampleFormats:" << info.supportedSampleFormats();
    qInfo() << "prefferedSampleFormat:" << info.preferredFormat().sampleFormat();
    qInfo() << "bytesPerSample:" << info.preferredFormat().bytesPerSample();
    qInfo() << "bytesPerFrame:" << info.preferredFormat().bytesPerFrame();*/

    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    /*buffer = new QBuffer(this);

    int sampleRate = format.sampleRate();
    qreal frequency = 261.63;

    buffer->open(QIODevice::ReadWrite);
    buffer->seek(0);

    audio = new QAudioSink(format, this);

    for (int i = 0; i < sampleRate*2; i++) {
        qreal t = buffer->size() / qreal(sampleRate);
        qint16 value = (qSin(M_PI * frequency * t))*32767;
        buffer->buffer().append((char) (value & 0xff));
        buffer->buffer().append((char) ((value >> 8) & 0xff));
    }*/

    /*for (int i = 0; i < sampleRate/100; i++) {
        buffer->buffer().append((char)0);
    }

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [=](){
        qDebug() << "timer";
        qDebug() << buffer->size();
        qDebug() << audio->state();

        for (int i = 0; i < sampleRate*2; i++) {
            qreal t = buffer->size() / qreal(sampleRate);
            qint16 value = (qSin(M_PI * frequency * t))*32767;
            buffer->buffer().append((char) (value & 0xff));
            buffer->buffer().append((char) ((value >> 8) & 0xff));
        }

        if (audio->state() == QAudio::IdleState) {
            buffer->open(QIODevice::ReadWrite);
            audio->start(buffer);
        }

        buffer->buffer().remove(0,buffer->buffer().size()-sampleRate);
    });
    timer->start(1000);*/

    //audio->start(buffer);

    data = new AudioStream(this);
    data->open(QIODevice::ReadWrite);
    audio = new QAudioSink(format, this);
    QObject::connect(audio, &QAudioSink::stateChanged, this, [=](){
        qDebug() << audio->state() << audio->error() << audio->bufferSize();
    });
    audio->setBufferSize(2);

    //QProcess* test = new QProcess(this);
    audio->start(data);

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [=](){
        qDebug() << audio->state() << audio->error() << audio->bufferSize() << data->size() << audio->bytesFree() << data->isOpen() << data->isReadable() << data->pos() << audio->processedUSecs() << format.sampleFormat();
    });
    timer->start(1000);
}
