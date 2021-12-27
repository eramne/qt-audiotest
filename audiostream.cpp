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
    qreal frequency = 440; //standard A4
    qreal volume = 1;

    int i = 0;
    while (i < maxSize) {
        qreal t = samplesProcessed/qreal(audio->format().sampleRate());

        qreal osc1 = qSin(M_PI * frequency * t);
        qreal osc2 = qSin(M_PI * (frequency*qPow(2.0,400.0/1200.0)) * t); //400 cents = major third
        qreal osc3 = qSin(M_PI * (frequency*qPow(2.0,700.0/1200.0)) * t); //700 cents = perfect fifth

        qint16 value = qMax(qMin(((osc1+osc2+osc3)/3)*32767*volume,32767.0),-32767.0);

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
