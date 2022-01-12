#include "audiostream.h"

AudioStream::AudioStream(QObject *parent, QAudioSink* audiosink)
    : QBuffer{parent} {
    audio = audiosink;
}

AudioStream::~AudioStream()
{
    close();
}

qint64 AudioStream::readData(char *data, qint64 maxSize) {
    qreal carrier_frequency = 220;
    qreal modulator_frequency = 440;
    qreal volume = 1;

    qreal mod_index = 1;
    qreal carrier_ang_frequency = M_PI*carrier_frequency;
    qreal modulator_ang_frequency = M_PI*modulator_frequency;

    int i = 0;
    while (i < maxSize) {
        qreal t = qMax(samplesProcessed/qreal(audio->format().sampleRate()) - 5.0,0.0);

        qreal out = qSin((carrier_ang_frequency * t) + mod_index*qSin(modulator_ang_frequency*t));

        qint16 value = qMax(qMin(out*32767*volume,32767.0),-32767.0);

        data[i] = (char) (value & 0xff);
        data[i+1] = (char) ((value >> 8) & 0xff);
        i += 2;
        samplesProcessed++;
    }

    return i;
}

qint64 AudioStream::writeData(const char *data, qint64 maxSize) {
    return -1;
}

bool AudioStream::atEnd() const {
    return false;
}

qint64 AudioStream::size() const {
    return 4096;
}

qint64 AudioStream::bytesAvailable() const {
    return 4096;
}

qint64 AudioStream::pos() const {
    return 0;
}
