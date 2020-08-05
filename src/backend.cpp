#pragma once

#include "backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QDataStream>
#include <QFile>
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
#include <QMessageBox>

#include "capturethread.h"
#include "hooks_dll/mousehook.h"
#include "blwindow.h"
#include "hooks.h"

#define HOOKS
//#undef HOOKS

#define TIMEOUT1 30
#define TIMEOUT2 5

using namespace WinToastLib;

BackEnd* BackEnd::m_instance = nullptr;

extern BLWindow* wnd;

extern HWND g_hwnd;

class CustomHandler : public WinToastLib::IWinToastHandler {
public:
    void toastActivated() const {
        qDebug() << "The user clicked in this toast";
        wnd->requestActivate();
    }

    void toastActivated(int actionIndex) const {
        qDebug() << "The user clicked on button #" << actionIndex << L" in this toast";
        wnd->requestActivate();
        // TODO: Show Window
    }

    void toastFailed() const {
        qDebug() << "Error showing current toast";
    }
    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            qDebug() << "The user dismissed this toast";
            BackEnd::getInstance()->setNotifyMode(false);
            break;
        case ApplicationHidden:
            qDebug() << "The application hid the toast using ToastNotifier.hide()";
            break;
        case TimedOut:
            qDebug() << "The toast has timed out";
            // TODO: Do nothink
            break;
        default:
            qDebug() << "Toast not activated";
            break;
        }
    }
};


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
    m_settings("Gavitka software", "Time lapse recorder"),
    m_recordTimer(),
    m_activitytimer(this)
{
    //m_windowHandles = new std::vector<HWND>;

    setFilePath(m_settings.value("filePath").toString());
    setFilePrefix(m_settings.value("filePrefix").toString());
    setBitRateIndex(m_settings.value("bitRate").toInt());
    setResolutionIndex(m_settings.value("resolution").toInt());
    setCropIndex(m_settings.value("crop").toInt());
    setFrameRateIndex(m_settings.value("frameRate").toInt());

    m_recMode = m_settings.value("recMode").toInt();
    m_sleepMode = m_settings.value("sleepMode").toBool();

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

    m_screen = QGuiApplication::primaryScreen();

    // Application list model
    m_appList = new AppList(nullptr);
    connect(m_appList, &AppList::selectedChanged, this, &BackEnd::selectedChangedSlot);
    m_appList->update();

    if (wnd != nullptr) m_screen = wnd->screen();
    refreshUI();

    m_activitytimer.callOnTimeout(this, &BackEnd::startCheckActivity);
    m_activitytimer_small.callOnTimeout(this, &BackEnd::stopCheckAvtivity);

    WinToast::instance()->setAppName(QCoreApplication::applicationName().toStdWString());
    WinToast::instance()->setAppUserModelId(
                WinToast::configureAUMI(QCoreApplication::organizationName().toStdWString(),
                                        L"rec_app",
                                        L"rec_app",
                                        L"1.0")); // TODO: update
    if (!WinToast::instance()->initialize()) {
        qDebug() << "Error, your system in not compatible!";
    }

    m_templ = WinToastTemplate(WinToastTemplate::Text02);
    m_templ.setTextField(
                QString("Bruh...").toStdWString(),
                WinToastTemplate::FirstLine);
    m_templ.setTextField(
                (QString("Looks like you are active on windows that ") +
                        "you selected. Didn't you kinda forget " +
                        "to start recording?").toStdWString(),
                WinToastTemplate::SecondLine);
    m_templ.setAudioPath(WinToastTemplate::AudioSystemFile::Reminder);
    m_templ.setDuration(WinToastTemplate::Duration::System);

    setNotifyMode(m_settings.value("notifyMode").toBool());
}


BackEnd::~BackEnd()
{
    m_appList->unsetHooks();
    delete m_screen;
}


void BackEnd::kick()
{
    //qDebug() << "being kicked";
    if(m_capture != nullptr) {
        m_capture->kick();
    }
    if(m_checkactivity) {
        if (WinToast::instance()->showToast(m_templ, new CustomHandler()) < 0) {
            throw new std::exception("Could not launch your toast notification!");
        }
        stopCheckAvtivity();
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

    if(m_recMode == RECORD_MODE::Window && m_appList->size() <= 0) return;

    clearCheckActivity();

    m_capture = new CaptureWorker();
    m_capture->moveToThread(&m_thread);

    connect(m_capture, &CaptureWorker::sleepingChanged, this, &BackEnd::sleepingChangedSlot);
    connect(m_capture, &CaptureWorker::statusChanged, this, &BackEnd::statusChangedSlot);
    connect(this, &BackEnd::stopSignal, m_capture, &CaptureWorker::stop);
    connect(this, &BackEnd::startSignal, m_capture, &CaptureWorker::start);
    connect(m_capture, &CaptureWorker::finished, this, &BackEnd::handleResults);
    connect(m_capture, &CaptureWorker::finished, m_capture, &QObject::deleteLater);

    connect(m_capture, &CaptureWorker::updateVector, this, &BackEnd::updateVectorSlot);

    m_capture->setCrop((m_cropIndex == CROP_MODE::Crop) ? true : false);
    m_capture->setBitRate(m_bitRate);
    m_capture->setFrameRate(m_recordFPS);

    m_thread.start();

    m_recordTime = 0;
    m_recordTimer.start();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BackEnd::timerUpdateSlot);
    m_timer->start(1000);
    m_appList->setHooks();

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
    initCheckActivity();
    if(m_capture != nullptr && m_thread.isRunning()) {
        m_recordTime +=  m_recordTimer.elapsed();
        m_timer->stop();
        delete m_timer;
        emit stopSignal();

        m_appList->unsetHooks();
    }
}


void BackEnd::handleResults()
{
     m_capture = nullptr;
     m_thread.quit();
     m_thread.wait();
     if(m_windowCloseFlag == true) {
         clearCheckActivity();
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
        m_filePath.setPath(d.absolutePath());
        m_settings.setValue("filePath", m_filePath.absolutePath());
        emit filePathChanged();
    } else if(m_filePath.isEmpty()) {
        m_filePath.setPath(d.homePath());
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

bool BackEnd::notifyMode()
{
    return m_notifyMode;
}


void BackEnd::setNotifyMode(bool value)
{
    m_notifyMode = value;
    if(m_notifyMode == true && !m_activitytimer.isActive()) {
        initCheckActivity();
    }
    if(m_notifyMode == false && m_activitytimer.isActive()) {
        clearCheckActivity();
    }
    m_settings.setValue("notifyMode", value);
    emit notifyModeChanged();
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

int BackEnd::resolutionIndex()
{
    return m_resolutionIndex;
}

qint64 BackEnd::getElapsedTime()
{
    if(recordStatus() == RECORD_STATUS::Pause)
         return m_recordTime;
     if (recordStatus() == RECORD_STATUS::Rec && m_recordTimer.isValid())
         return m_recordTime + m_recordTimer.elapsed();
    return 0;
}

QImage BackEnd::getPreview(int index)
{
    QImage img;
    if(index >= 0 && index < m_appList->size()) {
        HWND hwnd = m_appList->at(index).hwnd;
        if(IsWindow(hwnd)) {
            img = CaptureWorker::CaptureWindow(m_screen, hwnd);
            if(!img.isNull())
                return img;
        }
    }
    QPixmap symbol(":/images/na.png");
    QPixmap pixmap(300, 200);
    pixmap.fill(qRgb(0, 0, 0));
    int x1 = pixmap.width() / 2 - symbol.width() / 2;
    int y1 = pixmap.height() / 2 - symbol.height() / 2;

    QPainter painter(&pixmap);
    painter.drawPixmap(x1, y1, symbol, 0, 0, 0, 0);
    painter.end();

    img = pixmap.toImage();
    return img;
}

int BackEnd::playFPS()
{
    return m_playFPS;
}

int BackEnd::bitRate()
{
    return m_bitRate;
}

int BackEnd::recordFPS()
{
    return m_recordFPS;
}

int BackEnd::getWidth()
{
    return m_width;
}

int BackEnd::getHeight()
{
    return m_height;
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
    if(m_capture != nullptr) {
        m_capture->setCrop((value == CROP_MODE::Crop) ? true : false);
    }
    emit cropIndexChanged();
}

int BackEnd::bitRateIndex()
{
    return m_bitRateIndex;
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
        m_bitRate = 2500;
        m_bitRateIndex = BITRATES::b2500;
        break;
    }
    if(m_capture != nullptr) {
        m_capture->setBitRate(m_bitRate);
    }
    m_settings.setValue("bitRate", m_bitRateIndex);
    emit bitRateIndexChanged();
}

int BackEnd::frameRateIndex() {return m_frameRateIndex;}

void BackEnd::setFrameRateIndex(int value)
{
    m_frameRateIndex = value;
    switch(value) {
    case FRAMERATES::x1:
        m_recordFPS = m_playFPS / 1;
        break;
    case FRAMERATES::x2:
        m_recordFPS = m_playFPS / 2;
        break;
    case FRAMERATES::x4:
        m_recordFPS = m_playFPS / 4;
        break;
    case FRAMERATES::x8:
        m_recordFPS = m_playFPS / 8;
        break;
    case FRAMERATES::x16:
        m_recordFPS = m_playFPS / 16;
        break;
    }
    if(m_capture != nullptr) {
        m_capture->setFrameRate(m_recordFPS);
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
    if(recordMode() == RECORD_MODE::Window && m_appList->isSelected())
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

bool BackEnd::isSleeping()
{
    if(m_capture != nullptr) {
        return m_capture->sleeping();
    }
    return false;
}

AppList *BackEnd::appList()
{
    return m_appList;
}

QSettings *BackEnd::getSettings(){return &m_settings;}

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
    emit sleepingChanged();
}

void BackEnd::close() {
    if(recordStatus() == RECORD_STATUS::Rec || recordStatus() == RECORD_STATUS::Pause)
    {
       stopRecording();
       m_windowCloseFlag = true;
    }
}

void BackEnd::updateVectorSlot()
{
    // Ask applist to create window handles
    // m_appList->updateVector(m_windowHandles);
    // Update Windows List in dll
    // UpdateWindowsList(m_windowHandles);
}

void BackEnd::selectedChangedSlot()
{
    emit recordReadyChanged();
}


void BackEnd::initCheckActivity()
{
//    if(!m_notifyMode) return;
//    m_activitytimer.setInterval(TIMEOUT1 * 1000);
//    m_activitytimer.start();
}

void BackEnd::startCheckActivity()
{
//    m_appList->setHooks();
//    m_activitytimer_small.setInterval(TIMEOUT2 * 1000);
//    m_activitytimer_small.setSingleShot(true);
//    m_activitytimer_small.start();
//    m_checkactivity = true;
}

void BackEnd::stopCheckAvtivity()
{
//    m_appList->unsetHooks();
//    m_checkactivity = false;
}

void BackEnd::clearCheckActivity()
{
//    m_activitytimer.stop();
//    m_activitytimer_small.stop();
//    stopCheckAvtivity();
//    m_appList->unsetHooks();
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


ThumbProvider::ThumbProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{ }


QImage ThumbProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);

    int index = id.toInt();
    QImage image = BackEnd::getInstance()->getPreview(index);
    if (size)
        *size = QSize(image.width(), image.height());
    return image;
}
