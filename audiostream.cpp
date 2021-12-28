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
    qreal frequency = 440/2;
    qreal volume = 1;

    int i = 0;
    while (i < maxSize) {
        qreal t = samplesProcessed/qreal(audio->format().sampleRate());

        qreal osc1 = (qSin(M_PI * frequency * t)*qPow(0.7,t)*20 +
                     qSin(M_PI * frequency*2 * t)*qPow(0.8,t)*qSin(4*t)*5 +
                     qSin(M_PI * frequency*3 * t)*qPow(0.6,t)*qSin(2*t)*10 +
                     qSin(M_PI * frequency*4 * t)*qPow(0.3,t)*qSin(0.5*t)*5 +
                     qSin(M_PI * frequency*6 * t)*qPow(0.5,t)*qSin(0.2*t)*3)/43;


        qint16 value = qMax(qMin(osc1*32767*volume,32767.0),-32767.0);

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
