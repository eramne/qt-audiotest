#pragma once

#include <QObject>
#include <QAudio>
#include <QFile>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QAudioSink>
#include <QDebug>
#include <QtQml>
#include <QIODevice>
#include <QBuffer>
#include <QtMath>
#include <QSoundEffect>
#include "audiostream.h"

class TestAudioThing : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit TestAudioThing(QObject *parent = nullptr);

    QFile sourceFile;
    QAudioSink* audio;

    QBuffer* buffer;
    AudioStream* data;

signals:

};

