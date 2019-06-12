#pragma once

#include "backend.h"

#include <QDataStream>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QTextStream>
#include <QWindow>

#include <QWidget>
#include <QtWidgets>
#include <QPixmap>
#include <QQmlApplicationEngine>

#include <QThread>

#include "x264encoding.h"
#include "ffmpeg_encoder.h"
#include "VideoCapture.h"

extern QWindow* windowRef;

void BackEnd::setOutputText(QString s) {
    if (s== m_output_text) return;

    m_output_text = s;
    emit outputTextChanged();
}

QImage capturescreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();
    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());
    return image;
}

void BackEnd::startRecording() {
    addOutPutText("Recording Started\n");

//    ffmpeg_encoder encoder;
//    encoder.encode();
    VideoCapture vc;
    vc.Init(352,288,25,2500);
    for (int i = 0; i < 1000; ++i) {
        vc.AddFrame(capturescreen());
        QThread::msleep(100);
    }
    vc.Finish();

    addOutPutText("Recording Finished\n");
}

void BackEnd::stopRecording() {
    addOutPutText("Recording Stopped\n");
}
