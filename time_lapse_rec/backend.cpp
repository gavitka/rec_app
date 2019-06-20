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

#include "capturethread.h"

BackEnd* BackEnd::m_instance = nullptr;

extern QWindow* wnd;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_thr(nullptr),
    m_hwnd(nullptr),
    m_settings("Gavitka software", "Time lapse recorder")
{
    //    setOutWidth(1920/4); //deleteme
    //    setOutHeight(1200/4); //deleteme
    //    setFramesPerSecond(25); //deleteme
    //    setShotsPerSecond(3); //deleteme
    //    setOutFileName("c:/dev/rec_app/filename.mp4"); //deleteme
    // m_hwnd = nullptr
    m_framesPerSecond = 24;
    m_shotsPerSecond = 3;

    setRecordStatus(RECORD_STATUS::Idle);
    m_recMode = RECORD_MODE::Screen;
    m_filePrefix = "record";
    m_recordTimer = QElapsedTimer();
    m_elapsedTime = 0;

    setFilePath( m_settings.value("filePath").toString());
    setFilePrefix(m_settings.value("filePrefix").toString());
    setBitRateIndex(m_settings.value("bitRate").toInt());
    setResolutionIndex(m_settings.value("resolution").toInt());
    setFrameRateIndex(m_settings.value("frameRate").toInt());
    m_recMode = m_settings.value("recMode").toInt();
    setSleepMode(m_settings.value("sleepMode").toBool());

    m_resolutionList.append(new ListElement(RESOLUTIONS::res1080p, "1080p"));
    m_resolutionList.append(new ListElement(RESOLUTIONS::res720p, "720p"));
    m_resolutionList.append(new ListElement(RESOLUTIONS::res360p, "360p"));

    m_bitRateList.append(new ListElement(BITRATES::b500, "500Kbps"));
    m_bitRateList.append(new ListElement(BITRATES::b1500, "1500Kbps"));
    m_bitRateList.append(new ListElement(BITRATES::b2000, "2000Kbps"));
    m_bitRateList.append(new ListElement(BITRATES::b2500, "2500Kbps"));
    m_bitRateList.append(new ListElement(BITRATES::b3000, "3000Kbps"));

    m_frameRateList.append(new ListElement(FRAMERATES::x1, "1x"));
    m_frameRateList.append(new ListElement(FRAMERATES::x2, "2x"));
    m_frameRateList.append(new ListElement(FRAMERATES::x4, "4x"));
    m_frameRateList.append(new ListElement(FRAMERATES::x8, "8x"));
    m_frameRateList.append(new ListElement(FRAMERATES::x16, "16x"));

    m_imgpreview = QImage(300, 200, QImage::Format_RGBA8888);
    m_imgpreview.fill(qRgb(66, 66, 66));
    m_screen = QGuiApplication::primaryScreen();
    setImageSource("image://preview/");
    //m_recMode = RECORD_MODE::Window;
    //m_sleepMode = true;
    if (wnd) m_screen = wnd->screen();
    refreshUI();
}

BackEnd::~BackEnd()
{
    delete m_screen;
}

void BackEnd::kick() {
    if(m_thr != nullptr) {
        m_thr->kick();
    }
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

    m_settings.setValue("recMode", m_recMode);

    emit recModeChanged();
    emit recordReadyChanged();
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
    } else if (recordStatus() == RECORD_STATUS::Rec) {
        return m_elapsedTime + m_recordTimer.elapsed();
    } else {
        return 0;
    }
}

void BackEnd::refreshImage()
{
    if(recMode() == RECORD_MODE::Window) {
        m_imgpreview = CaptureThread::CaptureWindow(m_screen, m_hwnd);
    } else {
        m_imgpreview = CaptureThread::CaptureScreen(m_screen);
    }

    emit imageSourceChanged();
}


void BackEnd::setResolutionIndex(int value) {
    m_resolutionIndex = value;
    switch(value) {
    case RESOLUTIONS::res360p:
        m_width = 480;
        m_height = 360;
        break;
    case RESOLUTIONS::res720p:
        m_width = 1280;
        m_height = 720;
        break;
    case RESOLUTIONS::res1080p:
        m_width = 1920;
        m_height = 1080;
        break;
    }
    m_settings.setValue("resolution", value);
    emit resolutionIndexChanged();
}

void BackEnd::setBitRateIndex(int value) {
    m_bitRateIndex = value;
    switch(value) {
    case BITRATES::b500:
        m_bitRate = 500;
        break;
    case BITRATES::b1500:
        m_bitRate = 1500;
        break;
    case BITRATES::b2000:
        m_bitRate = 2000;
        break;
    case BITRATES::b2500:
        m_bitRate = 2500;
        break;
    case BITRATES::b3000:
        m_bitRate = 3000;
        break;
    }
    m_settings.setValue("bitRate", value);
    emit bitRateIndexChanged();
}

void BackEnd::setFrameRateIndex(int value) {
    m_frameRateIndex = value;
    switch(value) {
    case FRAMERATES::x1:
        m_shotsPerSecond = m_framesPerSecond / 1;
        break;
    case FRAMERATES::x2:
        m_shotsPerSecond = m_framesPerSecond / 2;
        break;
    case FRAMERATES::x4:
        m_shotsPerSecond = m_framesPerSecond / 4;
        break;
    case FRAMERATES::x8:
        m_shotsPerSecond = m_framesPerSecond / 8;
        break;
    case FRAMERATES::x16:
        m_shotsPerSecond = m_framesPerSecond / 16;
        break;
    }
    m_settings.setValue("frameRate", value);
    qDebug() << "m_frameRateIndex" << m_frameRateIndex;
    emit frameRateIndexChanged();
}


//void BackEnd::setFrameRate(int i) {
//    if(!i) i = 1;
//    switch(i) {
//    case FRAMERATES::x1:
//        m_shotsPerSecond = m_framesPerSecond / 1;
//        break;
//    case FRAMERATES::x2:
//        m_shotsPerSecond = m_framesPerSecond / 2;
//        break;
//    case FRAMERATES::x4:
//        m_shotsPerSecond = m_framesPerSecond / 4;
//        break;
//    case FRAMERATES::x8:
//        m_shotsPerSecond = m_framesPerSecond / 8;
//        break;
//    case FRAMERATES::x16:
//        m_shotsPerSecond = m_framesPerSecond / 16;
//        break;
//    }
//    m_settings.setValue("frameRate", i);
//}

//void BackEnd::setBitRate(int i) {
//    if(!i) i = 1;
//    switch(i) {
//    case BITRATES::b500:
//        m_bitRate = 500;
//        break;
//    case BITRATES::b1500:
//        m_bitRate = 1500;
//        break;
//    case BITRATES::b2000:
//        m_bitRate = 2000;
//        break;
//    case BITRATES::b2500:
//        m_bitRate = 2500;
//        break;
//    case BITRATES::b3000:
//        m_bitRate = 3000;
//        break;
//    }
//    m_settings.setValue("bitRate", i);
//}

//void BackEnd::setResolution(int i) {
//    if(!i) i = 1;
//    switch(i) {
//    case RESOLUTIONS::res360p:
//        m_width = 480;
//        m_height = 360;
//        break;
//    case RESOLUTIONS::res720p:
//        m_width = 1280;
//        m_height = 720;
//        break;
//    case RESOLUTIONS::res1080p:
//        m_width = 1920;
//        m_height = 1080;
//        break;
//    }
//    m_settings.setValue("resolution", i);
//}

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
    emit recordReadyChanged();
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
