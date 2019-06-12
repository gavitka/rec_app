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

//#include "x264encoding.h"
//#include "ffmpeg_encoder.h"

void BackEnd::setOutputText(QString s) {
    if (s== m_output_text) return;

    m_output_text = s;
    emit outputTextChanged();
}

bool BackEnd::stopEnabled() {
    bool ret;
    if(m_thr == nullptr || !m_thr->isRunning() ) {
        ret = false;
    }
    else {
        ret = true;
    }

    return ret;
}

QString BackEnd::startButtonText() {
    QString ret;
    if(m_thr == nullptr || !m_thr->isRunning()) {
        ret = "Start recording";
    }
    else if(m_thr->isPaused()) {
        ret = "Resume recording";
    }
    else {
        ret = "Pause recording";
    }

    return ret;
}

void BackEnd::startRecording() {
    if(m_thr == nullptr || !m_thr->isRunning()) {
        //start
        m_thr = new CaptureThread();
        connect(m_thr, &CaptureThread::resultReady, this, &BackEnd::handleResults);
        m_thr->start();
        addOutPutText("Recording Started\n");
    }
    else {
        //pause
        m_thr->pause();
        addOutPutText("Recording paused\n");
    }
    refreshUI();
}

void BackEnd::stopRecording() {
    if(m_thr != nullptr && m_thr->isRunning()) {
        m_thr->Stop();
        addOutPutText("Recording Stopped\n");
    }
    refreshUI();
}

void BackEnd::handleResults()
{
    addOutPutText("Recording Finished\n");
    delete m_thr;
    refreshUI();
}

void BackEnd::refreshUI()
{
    emit stopEnabledChanged();
    emit startButtonTextChanged();
}
