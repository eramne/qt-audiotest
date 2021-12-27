#include "audiostream.h"

AudioStream::AudioStream(QObject *parent)
    : QBuffer{parent} {
}

AudioStream::~AudioStream()
{
    close();
}

qint64 AudioStream::readData(char *data, qint64 maxSize) {
    qreal frequency = 261.63;

    int i = 0;
    while (i < maxSize) {
        qreal t = QTime::currentTime().msecsSinceStartOfDay();
        qint16 value = (qSin(M_PI * frequency * t))*32767;

        data[i] = (char) (value & 0xff);
        data[i+1] = (char) ((value >> 8) & 0xff);
        i += 2;
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
