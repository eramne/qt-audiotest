#pragma once

#include <QObject>
#include <QtQml>
#include <QIODevice>
#include <QAudioSink>
#include <QAudioDevice>
#include <QAudioFormat>
#include <QBuffer>
#include <QtMath>
#include <QDebug>
#include <QMediaDevices>

class AudioController : public QBuffer
{
    Q_OBJECT
    QML_ELEMENT
public:

    explicit AudioController(QObject *parent = nullptr);
    virtual ~AudioController();

    QAudioSink *audio;
    qint64 samplesProcessed = 0;

    bool atEnd() const override;
    qint64 size() const override;
    qint64 bytesAvailable() const override;
    qint64 pos() const override;
    bool seek(qint64 pos) override;

    Q_PROPERTY(qreal volume MEMBER volume NOTIFY volumeChanged)
    Q_PROPERTY(qreal carrierFrequency MEMBER carrierFrequency NOTIFY carrierFrequencyChanged)
    Q_PROPERTY(qreal modulatorFrequency MEMBER modulatorFrequency NOTIFY modulatorFrequencyChanged)
    Q_PROPERTY(qreal modulationIndex MEMBER modulationIndex NOTIFY modulationIndexChanged)

signals:
    void volumeChanged();
    void carrierFrequencyChanged();
    void modulatorFrequencyChanged();
    void modulationIndexChanged();

public slots:
    void start();
    void stop();
    void suspend();
    void resume();
    qreal getTime();

private:
    qreal volume = 0;
    qreal carrierFrequency = 0;
    qreal modulatorFrequency = 0;
    qreal modulationIndex = 0;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
};
