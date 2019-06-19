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

extern QWindow* wnd;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_thr(nullptr)
{
    //    setOutWidth(1920/4); //deleteme
    //    setOutHeight(1200/4); //deleteme
    //    setFramesPerSecond(25); //deleteme
    //    setShotsPerSecond(3); //deleteme
    //    setOutFileName("c:/dev/rec_app/filename.mp4"); //deleteme

    m_framesPerSecond = 24;
    m_shotsPerSecond = 3;

    setRecordStatus(RECORD_STATUS::Idle);
    m_recMode = RECORD_MODE::Screen;
    m_filePrefix = "record";
    m_recordTimer = QElapsedTimer();
    m_elapsedTime = 0;
    m_filePath = m_filePath.home();
    m_imgpreview = QImage(300, 200, QImage::Format_RGBA8888);
    m_imgpreview.fill(qRgb(66, 66, 66));
    m_screen = QGuiApplication::primaryScreen();
    setImageSource("image://preview/");
    m_recMode = RECORD_MODE::Window;
    m_sleepMode = true;
    if (wnd) m_screen = wnd->screen();
    refreshUI();
}

BackEnd::~BackEnd()
{
    delete m_screen;
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
        m_thr = new CaptureThread(); // specify the capture frame rate
        // filename
        // framerate
        connect(m_thr, &CaptureThread::resultReady, this, &BackEnd::handleResults);
        m_thr->start();
        setRecordStatus(RECORD_STATUS::Rec);
    }
    m_elapsedTime = 0;
    m_recordTimer.start();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    m_timer->start(1000);
}

void BackEnd::pauseRecording()
{
    if(recordStatus() == RECORD_STATUS::Rec || recordStatus() == RECORD_STATUS::Pause) {
        //pause
        m_thr->togglepause();
        if(m_thr->isPaused()) {
            m_elapsedTime +=  m_recordTimer.elapsed();
            setRecordStatus(RECORD_STATUS::Pause);
        }
        else {
            m_recordTimer.start();
            setRecordStatus(RECORD_STATUS::Rec);
        }
    }
}

void BackEnd::stopRecording() {
    if(m_thr != nullptr && m_thr->isRunning()) {
        m_elapsedTime +=  m_recordTimer.elapsed();
        m_timer->stop();
        delete m_timer;
        m_thr->Stop();
        setRecordStatus(RECORD_STATUS::Idle);
    }
}

void BackEnd::handleResults()
{
    delete m_thr;
    QString value = m_outFileName;
}

void BackEnd::refreshUI()
{
    emit lockParamChanged();
    //    emit stopEnabledChanged();
    //    emit startButtonTextChanged();
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
    qDebug() << "Set Rec Mode: " << value << endl;
    if(value == true) {
        m_recMode = RECORD_MODE::Window;
    }
    else {
        m_recMode = RECORD_MODE::Screen;
    }

    emit recModeChanged();

    refreshImage();
}

QString BackEnd::recordingTime() {
    qint64 t;
    int sec, min, hour;
    t = getElapsedTime();
    sec = floor(t / 1000);
    min = floor(sec / 60);
    sec = sec % 60;
    hour = floor(min / 60);
    min = min % 60;
    return "Recording time: " + QString("%1:%2:%3").arg(int2str(hour)).arg(int2str(min)).arg(int2str(sec));
}

qint64 BackEnd::getElapsedTime()
{
    if(recordStatus() == RECORD_STATUS::Pause) {
        return m_elapsedTime;
    } else {
        return m_elapsedTime + m_recordTimer.elapsed();
    }
}

void BackEnd::refreshImage()
{
    if(recMode() == RECORD_MODE::Window) {
        m_imgpreview = CaptureThread::CaptureScreen(m_screen);
    } else {
        m_imgpreview = CaptureThread::CaptureWindow(m_screen, m_hwnd);
    }

    emit imageSourceChanged();
}

void BackEnd::setFrameRate(int i) {
    switch(i) {
    case FRAMERATES::en::x1:
        m_shotsPerSecond = m_framesPerSecond / 1;
        break;
    case FRAMERATES::en::x2:
        m_shotsPerSecond = m_framesPerSecond / 2;
        break;
    case FRAMERATES::en::x4:
        m_shotsPerSecond = m_framesPerSecond / 4;
        break;
    case FRAMERATES::en::x8:
        m_shotsPerSecond = m_framesPerSecond / 8;
        break;
    case FRAMERATES::en::x16:
        m_shotsPerSecond = m_framesPerSecond / 16;
        break;
    }
}

void BackEnd::setBitRate(int i) {
    switch(i) {
    case BITRATES::en::b500:
        m_bitRate = 500;
        break;
    case BITRATES::en::b1500:
        m_bitRate = 1500;
        break;
    case BITRATES::en::b2000:
        m_bitRate = 2000;
        break;
    case BITRATES::en::b2500:
        m_bitRate = 2500;
        break;
    case BITRATES::en::b3000:
        m_bitRate = 3000;
        break;
    }
}

void BackEnd::setResolution(int i) {
    switch(i) {
    case RESOLUTIONS::en::res360p:
        m_width = 480;
        m_height = 360;
        break;
    case RESOLUTIONS::en::res720p:
        m_width = 1280;
        m_height = 720;
        break;
    case RESOLUTIONS::en::res1080p:
        m_width = 1920;
        m_height = 1080;
        break;
    }
}

//void BackEnd::setOutFileName(QString value){
//    if(value.right(4) != ".mp4") {
//        value = value + ".mp4";
//    }
//    QFileInfo fi(value);
//    QString fname = fi.fileName();
//    QDir dir = fi.dir();
//    if(QFileInfo::exists(value)) {
//        while(QFileInfo::exists(value))
//        {
//            // (.*)\_?(\d)+\.(\w+)
//            QRegularExpression re("(.*?)\\_?(\\d*)\\.(\\w+)");
//            QRegularExpressionMatch match = re.match(fname);
//            if(match.hasMatch()) {
//                QString num = match.captured(2);
//                int inum = num.toInt();
//                fname = match.captured(1)+"_"+ QString::number(inum + 1) +
//                        "." + match.captured(3);
//                value = dir.absolutePath() + QDir::separator() + fname;
//                fi = QFileInfo(value);
//            }
//            else {
//                value = dir.absolutePath() + QDir::separator() + fname;
//                break;
//            }
//        }
//    }
//    else {
//        if(!dir.exists()) {
//            qDebug() << "Folder not exists" << value << endl;
//            value = "";
//        }
//        else {
//            // [^!?<>:/\|"*\/] - windows banned characters
//            // !?<>:|\"* -> !?<>:|\\\\"*
//            QRegularExpression re("!?<>:|\\\"*\\\\\\/");
//            QRegularExpressionMatch match = re.match(value);
//            if (match.hasMatch()) {
//                qDebug() << "Wrong filename" << value << endl;
//                value = "";
//            }
//            value = fi.absoluteDir().absolutePath() + "\\" + fname;
//        }
//    }
//    if (value.isEmpty() && m_outFileName.isEmpty()) {
//        value = QDir::home().absolutePath() + "\\record.mp4";
//    }
//    if(!value.isEmpty()) {
//        m_outFileName = value;
//    }
//    emit outFileNameChanged();
//}

void BackEnd::getWindowsList()
{
    m_dataList.clear();
    EnumWindows(getWindowsListCallback, reinterpret_cast<LPARAM>(&m_dataList));
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
    refreshImage();
    setImageSource("image://preview/");
    //setRecordMode(RECORD_MODE::Window);
    qDebug() << "HWND selected: " << currentWindow->getHwnd();
}

QString int2str(int i) {
    QString s;
    s = QString::number(i);
    if(s.length() != 2) {
        s = QString("0") + s.right(1);
        return s;
    } else {
        return s;
    }
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);
    QImage image = BackEnd::getInstance()->imgPreview();
    if (size)
        *size = QSize(image.width(), image.height());
    return image;
}
