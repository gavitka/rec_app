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

BackEnd* BackEnd::m_instance = nullptr;

void BackEnd::setOutputText(QString s) {
    if (s== m_output_text) return;

    m_output_text = s;
    emit outputTextChanged();
}

bool BackEnd::stopEnabled() {
    return !lockParam();
}

QString BackEnd::startButtonText() {
    QString ret;
    if(recordStatus() == RECORD_STATUS::Idle) {
        ret = "Start recording";
    }
    else if(recordStatus() == RECORD_STATUS::Pause) {
        ret = "Resume recording";
    }
    else {
        ret = "Pause recording";
    }
    return ret;
}

void BackEnd::startRecording() {
    if(recordStatus() == RECORD_STATUS::Idle) {
        //start
        m_thr = new CaptureThread(shotsPerSecond()); // specify the capture frame rate
        connect(m_thr, &CaptureThread::resultReady, this, &BackEnd::handleResults);
        m_thr->start();
        setRecordStatus(RECORD_STATUS::Rec);
        addOutPutText("Recording Started\n");
    }
    else {
        //pause
        m_thr->togglepause();
        if(m_thr->isPaused()) {
            setRecordStatus(RECORD_STATUS::Pause);
            addOutPutText("Recording paused\n");
        }
        else {
            setRecordStatus(RECORD_STATUS::Rec);
            addOutPutText("Recording resumed\n");
        }
    }
}

void BackEnd::stopRecording() {
    if(m_thr != nullptr && m_thr->isRunning()) {
        m_thr->Stop();
        setRecordStatus(RECORD_STATUS::Idle);
        addOutPutText("Recording Stopped\n");
    }
}

void BackEnd::handleResults()
{
    addOutPutText("Recording Finished\n");
    delete m_thr;
}

void BackEnd::refreshUI()
{
    emit lockParamChanged();
    emit stopEnabledChanged();
    emit startButtonTextChanged();
}

bool BackEnd::lockParam(){
    if (recordStatus() == RECORD_STATUS::Idle) {
        return true;
    }
    else {
        return false;
    }
}
