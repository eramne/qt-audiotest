#include "audiocontroller.h"

AudioController::AudioController(QObject* parent)
    : QBuffer{parent} {

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    QAudioFormat format = info.preferredFormat();
    format.setSampleFormat(QAudioFormat::Int16);

    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioSink(format, this);
    open(QBuffer::ReadWrite);
    //audio->start(this);
}

AudioController::~AudioController() {
    close();
}

void AudioController::start() {
    audio->start(this);
}

void AudioController::stop() {
    audio->stop();
}

void AudioController::suspend() {
    audio->suspend();
}

void AudioController::resume() {
    audio->resume();
}

qint64 AudioController::readData(char *data, qint64 maxSize) {
    qreal carrier_ang_frequency = M_PI*carrierFrequency;
    qreal modulator_ang_frequency = M_PI*modulatorFrequency;

    int i = 0;
    while (i < maxSize) {
        qreal t = qMax(samplesProcessed/qreal(audio->format().sampleRate()) - 1.0,0.0);

        qreal out = qSin((carrier_ang_frequency * t) + modulationIndex*qSin(modulator_ang_frequency*t));

        qint16 value = qMax(qMin(out*32767*volume,32767.0),-32767.0);

        data[i] = (char) (value & 0xff);
        data[i+1] = (char) ((value >> 8) & 0xff);
        i += 2;
        samplesProcessed++;
    }

    return i;
}

qint64 AudioController::writeData(const char *data, qint64 maxSize) {
    return -1;
}

bool AudioController::atEnd() const {
    return false;
}

qint64 AudioController::size() const {
    return 4096;
}

qint64 AudioController::bytesAvailable() const {
    return 4096;
}

qint64 AudioController::pos() const {
    return 0;
}
