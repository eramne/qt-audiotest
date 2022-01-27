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
        qreal t = getTime();
        qreal carrierEnvelopeValue = envelope(t, playing, timeReleased, carrierAttack, carrierDecay, carrierSustain, carrierRelease);
        qreal modulatorEnvelopeValue = envelope(t, playing, timeReleased, modulatorAttack, modulatorDecay, modulatorSustain, modulatorRelease);

        qreal out = carrierEnvelopeValue*qSin((carrier_ang_frequency * t) + modulatorEnvelopeValue*modulationIndex*qSin(modulator_ang_frequency*t));

        qint16 value = qMax(qMin(out*32767*volume,32767.0),-32767.0);

        data[i] = (char) (value & 0xff);
        data[i+1] = (char) ((value >> 8) & 0xff);
        i += 2;
        samplesProcessed++;
    }

    return i;
}

qreal AudioController::getTime() {
    return samplesProcessed/qreal(audio->format().sampleRate());
}

void AudioController::setTime(qreal time) {
    samplesProcessed = time*qreal(audio->format().sampleRate());
}

qreal AudioController::envelope(qreal time, bool down, qreal timeReleased, qreal attack, qreal decay, qreal sustain, qreal release) {
    if (down) {
        if (time >= 0 && time <= attack) { //attack
            return qPow((1/attack)*time, 3);
        } else if (time >= attack && time <= attack+decay) { //decay
            qreal x = ((sustain - 1.0)/decay)*(time-attack) + 1;
            return qPow((x-sustain)/(1-sustain), 3)*(1-sustain) + sustain;
        } else if (time >= attack + decay) { //sustain
            return sustain;
        }
    } else { //release
        if (time >= timeReleased && time <= timeReleased + release) {
            qreal x = ((-sustain)/release)*(time-timeReleased) + sustain;
            return qPow(x/sustain, 3)*(sustain);
        }
    }
    return 0;
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

bool AudioController::seek(qint64 pos) {
    return false;
}
