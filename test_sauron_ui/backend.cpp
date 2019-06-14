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
#include <QDir>
#include <QRegularExpression>

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
        // kdj jhstart
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

    QString value = m_outFileName;
    setOutFileName(value);
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

bool BackEnd::recMode(){
//    qDebug() << "Get Rec Mode: " << m_recMode << endl;
    if(m_recMode == RECORD_MODE::Window){
        return true;
    }
    else{
        return false;
    }
}

void BackEnd::setRecMode(bool value){
//    qDebug() << "Set Rec Mode: " << value << endl;
    if(value == true) {
        m_recMode = RECORD_MODE::Window;
    }
    else {
        m_recMode = RECORD_MODE::Screen;
    }
    emit recModeChanged();
}

void BackEnd::setOutFileName(QString value){
    if(value.right(4) != ".mp4") {
        value = value + ".mp4";
    }
    QFileInfo fi(value);
    QString fname = fi.fileName();
    QDir dir = fi.dir();
    if(QFileInfo::exists(value)) {
        while(QFileInfo::exists(value))
        {
            // (.*)\_?(\d)+\.(\w+)
            QRegularExpression re("(.*?)\\_?(\\d*)\\.(\\w+)");
            QRegularExpressionMatch match = re.match(fname);
            if(match.hasMatch()) {
                QString num = match.captured(2);
                int inum = num.toInt();
                fname = match.captured(1)+"_"+ QString::number(inum + 1) +
                        "." + match.captured(3);
                value = dir.absolutePath() + QDir::separator() + fname;
                fi = QFileInfo(value);
            }
            else {
                value = dir.absolutePath() + QDir::separator() + fname;
                break;
            }
        }
    }
    else {
        if(!dir.exists()) {
            qDebug() << "Folder not exists" << value << endl;
            value = "";
        }
        else {
            // [^!?<>:/\|"*\/] - windows banned characters
            // !?<>:|\"* -> !?<>:|\\\\"*
            QRegularExpression re("!?<>:|\\\"*\\\\\\/");
            QRegularExpressionMatch match = re.match(value);
            if (match.hasMatch()) {
                qDebug() << "Wrong filename" << value << endl;
                value = "";
            }
            value = fi.absoluteDir().absolutePath() + "\\" + fname;
        }
    }
    if (value.isEmpty() && m_outFileName.isEmpty()) {
        value = QDir::home().absolutePath() + "\\record.mp4";
    }
    if(!value.isEmpty()) {
        m_outFileName = value;
    }
    emit outFileNameChanged();
}

void BackEnd::getWindowsList()
{
    //std::vector<std::wstring> titles;
    m_dataList.clear();
    EnumWindows(getWindowsListCallback, reinterpret_cast<LPARAM>(&m_dataList));
    // At this point, titles if fully populated and could be displayed, e.g.:
    //    for (const auto& title : titles) {
    //        QString str = QString::fromStdWString(title);
    //        addOutPutText(QString("Title: ") + str + "\n");
    //    }
    //    return;

    emit windowListChanged();
}

BOOL CALLBACK getWindowsListCallback(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
        return TRUE;
    }

    QList<QObject*>& dataList = *reinterpret_cast<QList<QObject*>*>(lParam);
    dataList.append(new WindowObject(hwnd, QString::fromStdWString(windowTitle)));

    return TRUE;
}

void BackEnd::setWindow(int index)
{
    WindowObject* currentWindow = (WindowObject*)m_dataList.at(index);
    HWND hwnd = currentWindow->getHwnd();
    m_hwnd = hwnd;
    //setRecordMode(RECORD_MODE::Window);
    qDebug() << "HWND selected: " << currentWindow->getHwnd();
}


