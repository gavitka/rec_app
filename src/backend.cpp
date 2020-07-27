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
#include "hooks_dll/mousehook.h"

#define HOOKS
//#undef HOOKS

BackEnd* BackEnd::m_instance = nullptr;

extern QWindow* wnd;

QObject *BackEnd::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return (QObject*)getInstance();
}

BackEnd *BackEnd::getInstance() {
    if(!m_instance) {
        m_instance = new BackEnd();
    }
    return m_instance;
}

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_hwnd(nullptr),
    m_settings("Gavitka software", "Time lapse recorder"),
    m_recordTimer()
{
    m_windowHandles = new std::vector<HWND>;

    setFilePath(m_settings.value("filePath").toString());
    setFilePrefix(m_settings.value("filePrefix").toString());
    setBitRateIndex(m_settings.value("bitRate").toInt());
    setResolutionIndex(m_settings.value("resolution").toInt());
    setCropIndex(m_settings.value("crop").toInt());
    setFrameRateIndex(m_settings.value("frameRate").toInt());
    m_recMode = m_settings.value("recMode").toInt();
    setSleepMode(m_settings.value("sleepMode").toBool());

    m_resolutionList.append(new ListElement(RESOLUTIONS::res1080p, "1080p"));
    m_resolutionList.append(new ListElement(RESOLUTIONS::res720p, "720p"));
    m_resolutionList.append(new ListElement(RESOLUTIONS::res360p, "360p"));

    m_cropList.append(new ListElement(CROP_MODE::Crop, "Crop"));
    m_cropList.append(new ListElement(CROP_MODE::Fill, "Fill"));

    m_bitRateList.append(new ListElement(BITRATES::b500, "500Kbps"));
    m_bitRateList.append(new ListElement(BITRATES::b1000, "1000Kbps"));
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

    m_windowName = m_settings.value("windowName").toString();
    getWindowsList();

    m_appList = new AppList(nullptr); // Application list model
    m_appList->update();

    setImageSource("image://preview/");
    if (wnd) m_screen = wnd->screen();
    refreshUI();
}

BackEnd::~BackEnd()
{
    delete m_screen;
}

void BackEnd::kick()
{
    if(m_capture != nullptr) {
        m_capture->kick();
    }
}

QString BackEnd::startButtonText()
{
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

void BackEnd::startRecording()
{
    if(recordStatus() != RECORD_STATUS::Idle) return;

    m_capture = new CaptureThread();
    m_capture->moveToThread(&m_thread);

    connect(m_capture, &CaptureThread::sleepingChanged, this, &BackEnd::sleepingChangedSlot);
    connect(m_capture, &CaptureThread::statusChanged, this, &BackEnd::statusChangedSlot);
    connect(this, &BackEnd::stopSignal, m_capture, &CaptureThread::stop);
    connect(this, &BackEnd::startSignal, m_capture, &CaptureThread::start);
    connect(m_capture, &CaptureThread::finished, this, &BackEnd::handleResults);
    connect(m_capture, &CaptureThread::finished, m_capture, &QObject::deleteLater);

    connect(m_capture, &CaptureThread::updateVector, this, &BackEnd::updateVectorSlot);
    connect(m_capture, &CaptureThread::InstallHook, this, &BackEnd::InstallHook);
    connect(m_capture, &CaptureThread::UninstallHook, this, &BackEnd::UninstallHook);

    m_thread.start();

    m_recordTime = 0;
    m_recordTimer.start();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BackEnd::timerUpdateSlot);
    m_timer->start(1000);

    emit startSignal();
}

void BackEnd::pauseRecording()
{
     if(recordStatus() == RECORD_STATUS::Rec || recordStatus() == RECORD_STATUS::Pause) {
         m_capture->togglepause();
         if(recordStatus() == RECORD_STATUS::Pause) {
             m_recordTime +=  m_recordTimer.elapsed();
         }
         else {
             m_recordTimer.start();
         }
     }
}

void BackEnd::stopRecording()
{
    if(m_capture != nullptr && m_thread.isRunning()) {
        m_recordTime +=  m_recordTimer.elapsed();
        m_timer->stop();
        delete m_timer;
        emit stopSignal();
    }
}

void BackEnd::handleResults()
{
     m_capture = nullptr;
     m_thread.quit();
     m_thread.wait();
     if(m_windowCloseFlag == true) {
         emit closeReady();
     }
     refreshUI();
}

void BackEnd::refreshUI()
{
    emit timeChanged();
    emit statusChanged();
}

bool BackEnd::lockParam()
{
    return (recordStatus() == RECORD_STATUS::Idle) ? true : false;
}

int BackEnd::recordStatus()
{
    return (m_capture == nullptr) ? RECORD_STATUS::Idle :  m_capture->status();
}

bool BackEnd::recMode()
{
    return (m_recMode == RECORD_MODE::Window) ? true : false;
}

void BackEnd::setRecMode(bool value)
{
    m_recMode = (value == true) ? RECORD_MODE::Window : RECORD_MODE::Screen;
    m_settings.setValue("recMode", m_recMode);

    emit recModeChanged();
    emit recordReadyChanged();
    refreshImage();
    setImageSource("image://preview/");
}

int BackEnd::recordMode()
{
    return m_recMode;
}

void BackEnd::setRecordMode(int value)
{
    m_recMode = value;
    emit recModeChanged();
}

QList<QObject *> BackEnd::windowList()
{
    return m_dataList;
}

int BackEnd::windowIndex() {
    return m_windowIndex;
}

QString BackEnd::recordingTime()
{
    if(!m_capture) return "";
    qint64 t = getElapsedTime();
    if(t > 0) {
        if(m_capture->sleeping() == false)
            return "Recording time: " + format_time(t);
        else
            return "Sleeping...";
    }
    else return "";
}

QString BackEnd::filePath()
{
    return m_filePath.absolutePath();
}

void BackEnd::setFilePathUrl(QString value)
{
    QUrl u = QUrl(value);
    setFilePath(u.toLocalFile());
}

void BackEnd::setFilePath(QString value)
{
    QDir d = QDir(value);
    if(d.exists()) {
        m_filePath = d.absolutePath();
        m_settings.setValue("filePath", m_filePath.absolutePath());
        emit filePathChanged();
    } else if(m_filePath.isEmpty()) {
        m_filePath = d.homePath();
        emit filePathChanged();
    }
}

QString BackEnd::fileUrl()
{
    QUrl u;
    u = u.fromLocalFile(m_filePath.absolutePath());
    QString s = u.toString();
    return s;
}

QString BackEnd::fileName()
{
    QString filename;
    QDir d;
    int i = 0;
    filename = filePath() + d.separator() + filePrefix() + ".mp4";
    while(QFile(filename).exists() && i <1000) {
        filename = filePath() + d.separator() + filePrefix() + "_" + QString::number(i) + ".mp4";
        ++i;
    }
    return filename;
}

QString BackEnd::fileLabel()
{
    QString s = "<a href=\"" + fileUrl() + "\">" + filePath() + "</a>";
    return s;
}

QString BackEnd::imageSource()
{
    return m_imageSource;
}

void BackEnd::setImageSource(QString value)
{
    int x = QRandomGenerator::global()->generate();
    m_imageSource = value + QString::number(x);
    emit imageSourceChanged();
}

bool BackEnd::sleepMode()
{
    return m_sleepMode;
}

void BackEnd::setSleepMode(bool value)
{
    m_sleepMode = value;
    m_settings.setValue("sleepMode", value);
    emit sleepModeChanged();
}

QList<QObject*> BackEnd::resolutionList()
{
    return m_resolutionList;
}

QList<QObject*> BackEnd::cropList()
{
    return m_cropList;
}

QList<QObject*> BackEnd::bitRateList()
{
    return m_bitRateList;
}

QList<QObject*> BackEnd::frameRateList()
{
    return m_frameRateList;
}

qint64 BackEnd::getElapsedTime()
{
     if(recordStatus() == RECORD_STATUS::Pause)
         return m_recordTime;
     if (recordStatus() == RECORD_STATUS::Rec && m_recordTimer.isValid())
         return m_recordTime + m_recordTimer.elapsed();
    return 0;
}

QImage BackEnd::imgPreview()
{
    return m_imgpreview;
}

int BackEnd::framesPerSecond()
{
    return m_framesPerSecond;
}

int BackEnd::bitRate()
{
    return m_bitRate;
}

int BackEnd::shotsPerSecond()
{
    return m_shotsPerSecond;
}

int BackEnd::getWidth()
{
    return m_width;
}

int BackEnd::getHeight()
{
    return m_height;
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

void BackEnd::setResolutionIndex(int value)
{
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

int BackEnd::cropIndex()
{
    return m_cropIndex;
}

void BackEnd::setCropIndex(int value)
{
    m_cropIndex = value;
    m_settings.setValue("crop", value);
    emit cropIndexChanged();
}

void BackEnd::setBitRateIndex(int value)
{
    m_bitRateIndex = value;
    switch(value) {
    case BITRATES::b500:
        m_bitRate = 500;
        break;
    case BITRATES::b1000:
        m_bitRate = 1000;
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
    default:
        m_bitRateIndex = BITRATES::b2500;
        m_bitRate = 2500;
        break;
    }
    m_settings.setValue("bitRate", m_bitRateIndex);
    emit bitRateIndexChanged();
}

void BackEnd::setFrameRateIndex(int value)
{
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
    emit frameRateIndexChanged();
}

QString BackEnd::statusLine() {
    QString s;
    QFileInfo d(fileName());
    s = "Current File: " + d.fileName();
    return s;
}

bool BackEnd::recordReady() {
    if(recordMode() == RECORD_MODE::Screen)
        return true;
    if(recordMode() == RECORD_MODE::Window && getHwnd() != nullptr)
        return true;
    return false;
}

bool BackEnd::isRecording() {
    return (recordStatus() == RECORD_STATUS::Rec ||
            recordStatus() == RECORD_STATUS::Pause) ? true : false;
}

int BackEnd::mouseX()
{
    return m_mousex;
}

void BackEnd::setMouseX(int value)
{
    m_mousex = value;
    emit mousePosChanged();
}

int BackEnd::mouseY()
{
    return m_mousey;
}

void BackEnd::setMouseY(int value)
{
    m_mousey = value;
    emit mousePosChanged();
}

AppList *BackEnd::appList()
{
    return m_appList;
}

QSettings *BackEnd::getSettings(){return &m_settings;}

void BackEnd::getWindowsList()
{
    m_dataList.clear();
    EnumWindows(getWindowsListCallback, reinterpret_cast<LPARAM>(&m_dataList));
    int index = 0;
    for(int i = 0; i < m_dataList.length(); ++i) {
        if( ((WindowObject*)m_dataList.value(i))->name() == m_windowName) {
            index = i;
            break;
        }
    }
    emit windowListChanged();
    setWindowIndex(index);
}

void BackEnd::timerUpdateSlot()
{
    emit timeChanged();
}

void BackEnd::statusChangedSlot()
{
    emit statusChanged();
}

void BackEnd::sleepingChangedSlot()
{
    refreshUI();
}

void BackEnd::close() {
    if(recordStatus() == RECORD_STATUS::Rec || recordStatus() == RECORD_STATUS::Pause)
    {
       stopRecording();
       m_windowCloseFlag = true;
    }
}

void BackEnd::InstallHook()
{
#ifdef HOOKS
    InstallMultiHook((HWND)wnd->winId());
#endif
}

void BackEnd::UninstallHook()
{
#ifdef HOOKS
    UninstallMultiHook();
#endif
}

void BackEnd::updateVectorSlot()
{
    // Ask applist to create window handles
    m_appList->updateVector(m_windowHandles);
    // updates windows list in DLL
    UpdateWindowsList(m_windowHandles);
}

std::vector<HWND>* BackEnd::windowVector() {
    return m_windowHandles;
}

void BackEnd::setWindowIndex(int index)
{
    if(m_dataList.isEmpty()) return;
    if (index >= m_dataList.length()) return;
    WindowObject* currentWindow = (WindowObject*)m_dataList.at(index);
    m_windowIndex = index;
    m_windowName = currentWindow->name();
    m_settings.setValue("windowName", m_windowName);
    HWND hwnd = currentWindow->getHwnd();
    m_hwnd = hwnd;
    refreshImage();
    setImageSource("image://preview/");
    emit windowIndexChanged();
    emit recordReadyChanged();
}

HWND BackEnd::getHwnd()
{
    return m_hwnd;
}

QString BackEnd::filePrefix()
{
    return m_filePrefix;
}

void BackEnd::setFilePrefix(QString value) {
    if (value.right(1) == "_") {
        value = value.left(value.length()-1);
    }
    if(value.isEmpty()) {
        value = "prefix";
    }
    m_filePrefix = value;
    m_settings.setValue("filePrefix", m_filePrefix);
    emit filePrefixChanged();
}

QString BackEnd::recordingState()
{
    if(recordStatus() == RECORD_STATUS::Idle) {
        return "Ready";
    } else if (recordStatus() == RECORD_STATUS::Rec) {
        return "Recording";
    } else if (recordStatus() == RECORD_STATUS::Pause) {
        return "Recording paused";
    }
    return "Ready";
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);
    QImage image = BackEnd::getInstance()->imgPreview();
    if (size)
        *size = QSize(image.width(), image.height());
    return image;
}


BOOL CALLBACK getWindowsListCallback(HWND hwnd, LPARAM lParam)
{
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
