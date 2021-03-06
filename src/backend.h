#pragma once

#include "pch.h"

#include <QFile>
#include <QObject>
#include <QQmlEngine>
#include <thread>
#include <QList>
#include <QSettings>
#include <QDateTime>
#include <QTimer>
#include <QDir>
#include <QImage>
#include <QDebug>
#include <QQuickImageProvider>
#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QScreen>
#include <QThread>

#include "applistmodel.h"
#include "captureworker.h"
#include "lib/wintoastlib.h"

class CaptureWorker;

enum FRAMERATES{
    x1 = 0, x2, x4, x8, x16
};

enum RESOLUTIONS{
    res1080p = 0, res720p, res360p
};

enum CROP_MODE {
    Crop = 0,
    Fill
};

enum BITRATES{
    b500 = 0, b1000, b1500, b2000, b2500, b3000
};

class ListElement;

////////////////////////////////////////////////////////////////////////////////////

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool lockParam READ lockParam NOTIFY statusChanged)
    Q_PROPERTY(QString recordingState READ recordingState NOTIFY statusChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY statusChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY timeChanged)
    Q_PROPERTY(QString recordingTime READ recordingTime NOTIFY timeChanged)
    Q_PROPERTY(bool isSleeping READ isSleeping NOTIFY sleepingChanged)

    Q_PROPERTY(bool recMode READ recMode NOTIFY recModeChanged)

    Q_PROPERTY(QString filePrefix READ filePrefix WRITE setFilePrefix NOTIFY filePrefixChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePathUrl NOTIFY filePathChanged)
    Q_PROPERTY(QString fileUrl READ fileUrl NOTIFY filePathChanged)
    Q_PROPERTY(QString fileLabel READ fileLabel NOTIFY filePathChanged)

    Q_PROPERTY(bool sleepMode READ sleepMode WRITE setSleepMode NOTIFY sleepModeChanged)
    Q_PROPERTY(bool notifyMode READ notifyMode WRITE setNotifyMode NOTIFY notifyModeChanged)
    Q_PROPERTY(bool recordReady READ recordReady NOTIFY recordReadyChanged)

    Q_PROPERTY(QList<QObject*> resolutionList READ resolutionList NOTIFY resolutionListChanged)
    Q_PROPERTY(int resolutionIndex READ resolutionIndex WRITE setResolutionIndex NOTIFY resolutionIndexChanged)

    Q_PROPERTY(QList<QObject*> cropList READ cropList NOTIFY cropListChanged)
    Q_PROPERTY(int cropIndex READ cropIndex WRITE setCropIndex NOTIFY cropIndexChanged)

    Q_PROPERTY(QList<QObject*> bitRateList READ bitRateList NOTIFY bitRateListChanged)
    Q_PROPERTY(int bitRateIndex READ bitRateIndex WRITE setBitRateIndex NOTIFY bitRateIndexChanged)

    Q_PROPERTY(QList<QObject*> frameRateList READ frameRateList NOTIFY frameRateListChanged)
    Q_PROPERTY(int frameRateIndex READ frameRateIndex WRITE setFrameRateIndex NOTIFY frameRateIndexChanged)

    Q_PROPERTY(AppManager* appManager READ appManager NOTIFY appManagerChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    enum States {
        IdleState = 0,
        CheckActivityState,
        RecordState,
        SleepingState,
        ClosingState
    };

public:

    // ----------------- <SINGLETON>  -----------------

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static BackEnd* getInstance();

    // ----------------- </SINGLETON>  -----------------

    BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    // ----------------- <PROP>  -----------------

    QString startButtonText();

    bool lockParam();

    int recordStatus();

    bool recMode();

    QString filePrefix();
    void setFilePrefix(QString value);

    QString recordingState();

    QString recordingTime();

    QString filePath();
    void setFilePathUrl(QString value);
    void setFilePath(QString value);
    QString fileUrl();
    QString fileName();
    QString fileLabel();
    bool checkfilename(QString filename);

    bool sleepMode();
    void setSleepMode(bool value);

    QList<QObject*> resolutionList();
    QList<QObject*> cropList();
    QList<QObject*> bitRateList();
    QList<QObject*> frameRateList();

    int resolutionIndex();
    void setResolutionIndex(int value);

    int cropIndex();
    void setCropIndex(int value);

    int bitRateIndex();
    void setBitRateIndex(int value);

    int frameRateIndex();
    void setFrameRateIndex(int value);

    QString statusLine();

    bool recordReady();

    bool isRecording();

    bool isSleeping();

    bool notifyMode();
    void setNotifyMode(bool value);

    QColor color();
    void setColor(QColor value);

    AppManager *appManager();

    // ----------------- </PROP>  -----------------

    QSettings *getSettings();
    qint64 getElapsedTime();
    QImage getPreview(int index);

    int playFPS();

    int bitRate();

    int recordFPS();

    int getWidth();
    int getHeight();

    void refreshImage();
    void kick();

signals:

    void statusChanged();
    void timeChanged();
    void recModeChanged();
    void filePrefixChanged();
    void recordingTimeChanged();
    void filePathChanged();
    void imageSourceChanged();
    void sleepModeChanged();
    void notifyModeChanged();
    void resolutionListChanged();
    void cropListChanged();
    void bitRateListChanged();
    void frameRateListChanged();
    void frameRateIndexChanged();
    void bitRateIndexChanged();
    void resolutionIndexChanged();
    void cropIndexChanged();
    void statusLineChanged();
    void recordReadyChanged();
    void isRecordingChanged();
    void closeReady();
    void recordStatusChanged();
    void stopSignal();
    void startSignal();
    void sleepingChanged();
    void appManagerChanged();
    void colorChanged();

public slots:

    void statusChangedSlot();
    void startRecording();
    void pauseRecording();
    void stopRecording();
    void handleResults();
    void refreshUI();
    void timerUpdateSlot();
    void sleepingChangedSlot();
    void close();
    void selectedChangedSlot();
    void colorChange();
    void checkActivity();

private:

    static BackEnd* m_instance;
    int m_width;
    int m_height;

    int m_playFPS = 24;
    int m_recordFPS = 3;
    bool m_lockParam;
    //int m_recMode = RECORD_MODE::Screen;
    int m_bitRate;

    QSettings m_settings;
    QString m_filePrefix = "prefix";
    QTimer* m_timer;
    QDir m_filePath;
    QElapsedTimer m_recordTimer;
    qint64 m_recordTime = 0;
    QScreen* m_screen;
    bool m_sleepMode = false;
    bool m_notifyMode = false;

    QList<QObject*> m_resolutionList;
    QList<QObject*> m_cropList;
    QList<QObject*> m_bitRateList;
    QList<QObject*> m_frameRateList;
    int m_resolutionIndex;
    int m_cropIndex;
    int m_frameRateIndex;
    int m_bitRateIndex;

    bool m_sleepingflag = false;
    bool m_windowCloseFlag = false;

    CaptureWorker* m_capture = nullptr;
    QThread m_thread;

    WinToastLib::WinToastTemplate m_templ;

    States m_currentstate;
    AppManager *m_appmanager;

    QColor m_color;
    QTimer m_colortimer;

    QTimer m_activitytimer;
    bool m_activity = false;
};

////////////////////////////////////////////////////////////////////////////////////

class ThumbProvider : public QQuickImageProvider
{

public:
    ThumbProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

////////////////////////////////////////////////////////////////////////////////////

class ListElement : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:

    ListElement(int i, QString v) :
        QObject(),
        index(i),
        m_name(v)
    { }

    QString name(){
        return m_name;
    }

    void setName(QString value){
        m_name = value;
        emit nameChanged();
    }

signals:

    void nameChanged();

private:

    int index;
    QString m_name;

};
