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
    Q_PROPERTY(bool playing MEMBER playing NOTIFY playingChanged)
    Q_PROPERTY(qreal timeReleased MEMBER timeReleased NOTIFY timeReleasedChanged)
    Q_PROPERTY(qreal carrierFrequency MEMBER carrierFrequency NOTIFY carrierFrequencyChanged)

    Q_PROPERTY(qreal carrierAttack MEMBER carrierAttack NOTIFY carrierAttackChanged)
    Q_PROPERTY(qreal carrierDecay MEMBER carrierDecay NOTIFY carrierDecayChanged)
    Q_PROPERTY(qreal carrierSustain MEMBER carrierSustain NOTIFY carrierSustainChanged)
    Q_PROPERTY(qreal carrierRelease MEMBER carrierRelease NOTIFY carrierReleaseChanged)

    Q_PROPERTY(qreal modulatorFrequency MEMBER modulatorFrequency NOTIFY modulatorFrequencyChanged)
    Q_PROPERTY(qreal modulationIndex MEMBER modulationIndex NOTIFY modulationIndexChanged)

    Q_PROPERTY(qreal modulatorAttack MEMBER modulatorAttack NOTIFY modulatorAttackChanged)
    Q_PROPERTY(qreal modulatorDecay MEMBER modulatorDecay NOTIFY modulatorDecayChanged)
    Q_PROPERTY(qreal modulatorSustain MEMBER modulatorSustain NOTIFY modulatorSustainChanged)
    Q_PROPERTY(qreal modulatorRelease MEMBER modulatorRelease NOTIFY modulatorReleaseChanged)

signals:
    void volumeChanged();
    void playingChanged();
    void timeReleasedChanged();
    void carrierFrequencyChanged();

    void carrierAttackChanged();
    void carrierDecayChanged();
    void carrierSustainChanged();
    void carrierReleaseChanged();

    void modulatorFrequencyChanged();
    void modulationIndexChanged();

    void modulatorAttackChanged();
    void modulatorDecayChanged();
    void modulatorSustainChanged();
    void modulatorReleaseChanged();

public slots:
    void start();
    void stop();
    void suspend();
    void resume();
    qreal getTime();
    void setTime(qreal time);
    qreal envelope(qreal time, bool down, qreal timeReleased, qreal attack, qreal decay, qreal sustain, qreal release);

private:
    qreal volume = 0;
    bool playing = false;
    qreal timeReleased = 0;
    qreal carrierFrequency = 0;

    qreal carrierAttack = 0.01;
    qreal carrierDecay = 0.01;
    qreal carrierSustain = 0.999;
    qreal carrierRelease = 0.01;

    qreal modulatorFrequency = 0;
    qreal modulationIndex = 0;

    qreal modulatorAttack = 0.01;
    qreal modulatorDecay = 0.01;
    qreal modulatorSustain = 0.999;
    qreal modulatorRelease = 0.01;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
};
