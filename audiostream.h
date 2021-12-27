#pragma once

#include <QIODevice>
#include <QBuffer>
#include <QTime>

class AudioStream : public QBuffer
{
public:

    AudioStream(QObject *parent);
    ~AudioStream();

    //bool isSequential() const override;
    bool atEnd() const override;
    qint64 size() const override;
    qint64 bytesAvailable() const override;
    qint64 pos() const override;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
};
