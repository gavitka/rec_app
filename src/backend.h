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

#include "applistmodel.h"

class CaptureThread;

enum RECORD_STATUS{
    Idle,
    Rec,
    Pause
};

enum RECORD_MODE {
    Screen,
    Window
};

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

class WindowObject;
class ResolutionObject;
class BitrateObject;
class FramerateObject;
class ListElement;

QString int2str(int i);

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool lockParam READ lockParam NOTIFY lockParamChanged)
    Q_PROPERTY(QList<QObject*> windowList READ windowList NOTIFY windowListChanged)
    Q_PROPERTY(int windowIndex READ windowIndex WRITE setWindowIndex NOTIFY windowIndexChanged)
    Q_PROPERTY(int mouseX READ mouseX WRITE setMouseX NOTIFY mouseXChanged)
    Q_PROPERTY(int mouseY READ mouseY WRITE setMouseX NOTIFY mouseYChanged)
    Q_PROPERTY(bool recMode READ recMode WRITE setRecMode NOTIFY recModeChanged)
    Q_PROPERTY(QString filePrefix READ filePrefix WRITE setFilePrefix NOTIFY filePrefixChanged)
    Q_PROPERTY(QString recordingState READ recordingState NOTIFY recordingStateChanged)
    Q_PROPERTY(QString recordingTime READ recordingTime NOTIFY recordingTimeChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePathUrl NOTIFY filePathChanged)
    Q_PROPERTY(QString fileUrl READ fileUrl NOTIFY filePathChanged)
    Q_PROPERTY(QString fileLabel READ fileLabel NOTIFY filePathChanged)
    Q_PROPERTY(QString imageSource READ imageSource WRITE setImageSource NOTIFY imageSourceChanged)
    Q_PROPERTY(bool sleepMode READ sleepMode WRITE setSleepMode NOTIFY sleepModeChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY statusLineChanged)
    Q_PROPERTY(bool recordReady READ recordReady NOTIFY recordReadyChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY recordingStateChanged)

    Q_PROPERTY(QList<QObject*> resolutionList READ resolutionList NOTIFY resolutionListChanged)
    Q_PROPERTY(int resolutionIndex READ resolutionIndex WRITE setResolutionIndex NOTIFY resolutionIndexChanged)
    Q_PROPERTY(QList<QObject*> cropList READ cropList NOTIFY cropListChanged)
    Q_PROPERTY(int cropIndex READ cropIndex WRITE setCropIndex NOTIFY cropIndexChanged)
    Q_PROPERTY(QList<QObject*> bitRateList READ bitRateList NOTIFY bitRateListChanged)
    Q_PROPERTY(int bitRateIndex READ bitRateIndex WRITE setBitRateIndex NOTIFY bitRateIndexChanged)
    Q_PROPERTY(QList<QObject*> frameRateList READ frameRateList NOTIFY frameRateListChanged)
    Q_PROPERTY(int frameRateIndex READ frameRateIndex WRITE setFrameRateIndex NOTIFY frameRateIndexChanged)

    Q_PROPERTY(AppListModel* appListModel READ appListModel NOTIFY appListModelChanged)

public:

    // ----------------- <SINGLETON>  -----------------

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return (QObject*)getInstance();
    }

    static BackEnd* getInstance() {
        if(!m_instance) {
            m_instance = new BackEnd();
        }
        return m_instance;
    }

    // ----------------- </SINGLETON>  -----------------

    BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    void kick();

    // ----------------- <PROP>  -----------------

    QString startButtonText();

    int mouseX(){return m_mousex;}
    void setMouseX(int value){m_mousex = value; emit mouseXChanged();}

    int mouseY(){return m_mousey;}
    void setMouseY(int value){m_mousey = value; emit mouseYChanged();}

    bool lockParam();

    int recordStatus(){return m_record_status;}
    void setRecordStatus(int value){
        m_record_status = value;
        emit recordingStateChanged();
        refreshUI();
    }

    bool recMode();
    void setRecMode(bool value);

    int recordMode(){return m_recMode;}
    void setRecordMode(int value){
        m_recMode = value;
        emit recModeChanged();
    }

    QList<QObject*> windowList() {
        return m_dataList;
    }

    int windowIndex() {
        return m_windowIndex;
    }
    void setWindowIndex(int value);

    HWND getHwnd(){return m_hwnd;}

    QString filePrefix();
    void setFilePrefix(QString value);

    QString recordingState()
    {
        if(this->recordStatus() == RECORD_STATUS::Idle) {
            return "Ready";
        } else if (this->recordStatus() == RECORD_STATUS::Rec) {
            return "Recording";
        } else if (this->recordStatus() == RECORD_STATUS::Pause) {
            return "Recording paused";
        }
        return "Unknown";
    }

    QString recordingTime();

    QString filePath() {
        return m_filePath.absolutePath();
    }
    void setFilePathUrl(QString value) {
        QUrl u = QUrl(value);
        setFilePath(u.toLocalFile());
    }
    void setFilePath(QString value) {
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

    QString fileUrl() {
        QUrl u;
        u = u.fromLocalFile(m_filePath.absolutePath());
        QString s = u.toString();
        return s;
    }

    QString fileName() {
        QString filename;
        QDir d;
        int i = 0;
        filename = filePath() + d.separator() + filePrefix() + ".mp4";
        while(checkfilename(filename) && i <1000) {
            filename = filePath() + d.separator() + filePrefix() + "_" + QString::number(i) + ".mp4";
            ++i;
        }
        return filename;
    }

    QString fileLabel() {
        QString s = "<a href=\"" + fileUrl() + "\">" + filePath() + "</a>";
        qDebug().noquote() << "Hyperlink" << s;
        return s;
    }

    bool checkfilename(QString filename){
        QFile f(filename);
        return f.exists();
    }

    QString imageSource() {return m_imageSource;}
    void setImageSource(QString value) {
        int x = QRandomGenerator::global()->generate();
        m_imageSource = value + QString::number(x);
        emit imageSourceChanged();
    }

    bool sleepMode() {return m_sleepMode;}
    void setSleepMode(bool value) {
        m_sleepMode = value;
        m_settings.setValue("sleepMode", value);
        emit sleepModeChanged();
    }

    QList<QObject*> resolutionList() {
        return m_resolutionList;
    }
    QList<QObject*> cropList() {
        return m_cropList;
    }
    QList<QObject*> bitRateList() {
        return m_bitRateList;
    }
    QList<QObject*> frameRateList() {
        return m_frameRateList;
    }

    int resolutionIndex() {return m_resolutionIndex;}
    void setResolutionIndex(int value);

    int cropIndex() {return m_cropIndex;}
    void setCropIndex(int value);

    int bitRateIndex() {return m_bitRateIndex;}
    void setBitRateIndex(int value);

    int frameRateIndex() {return m_frameRateIndex;}
    void setFrameRateIndex(int value);

    QString statusLine() {
        QString s;
        QFileInfo d(fileName());
        s = "Current File: " + d.fileName();
        return s;
    }

    bool recordReady() {
        if(recordMode() == RECORD_MODE::Screen)
            return true;
        if(recordMode() == RECORD_MODE::Window && getHwnd() != nullptr)
            return true;
        return false;
    }

    bool isRecording() {
        if(this->recordStatus() == RECORD_STATUS::Rec) return true;
        else return false;
    }

    AppListModel* appListModel() {
        return m_appList;
    }

    // ----------------- </PROP>  -----------------

    QSettings* getSettings(){return &m_settings;}
    qint64 getElapsedTime();
    QImage imgPreview() {return m_imgpreview;}

    int framesPerSecond() {return m_framesPerSecond;}

    int bitRate() {return m_bitRate;}

    int shotsPerSecond() {return m_shotsPerSecond;}

    int getWidth() {return m_width;}
    int getHeight() {return m_height;}

    void refreshImage();

signals:

    void lockParamChanged();
    void windowListChanged();
    void windowIndexChanged();
    void mouseXChanged();
    void mouseYChanged();
    void recModeChanged();
    void filePrefixChanged();
    void recordingStateChanged();
    void recordingTimeChanged();
    void filePathChanged();
    void imageSourceChanged();
    void sleepModeChanged();
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
    void appListModelChanged();
    void closeReady();

public slots:

    void startRecording();
    void pauseRecording();
    void stopRecording();
    void handleResults();
    void handleError();
    void refreshUI();
    void getWindowsList();
    void timerUpdate() {
        emit recordingTimeChanged();
    }
    QScreen* getScreen(){return m_screen;}
    void handleSleeping(bool sleeping);
    void close();

private:

    static BackEnd* m_instance;
    QString m_output_text;
    CaptureThread* m_thr;
    int m_width;
    int m_height;
    int m_mousex;
    int m_mousey;
    int m_framesPerSecond;
    int m_shotsPerSecond;
    bool m_lockParam;
    int m_record_status;
    int m_recMode;
    int m_bitRate;
    QString m_outFileName;
    QList<QObject*> m_dataList;
    int m_windowIndex;
    HWND m_hwnd;
    QSettings m_settings;
    QString m_filePrefix;
    QTimer* m_timer;
    QDir m_filePath;
    QElapsedTimer m_recordTimer;
    qint64 m_elapsedTime;
    QImage m_imgpreview;
    QScreen* m_screen;
    QString m_imageSource;
    bool m_sleepMode;
    QList<QObject*> m_resolutionList;
    QList<QObject*> m_cropList;
    QList<QObject*> m_bitRateList;
    QList<QObject*> m_frameRateList;
    int m_resolutionIndex;
    int m_cropIndex;
    int m_bitRateIndex;
    int m_frameRateIndex;
    QString m_windowName;
    bool m_sleepingflag;
    AppListModel* m_appList;
    bool m_windowCloseFlag = false;
};

class WindowObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    WindowObject(HWND hwnd, QString name):
        m_hwnd (hwnd),
        m_name (name)
    { }

    QString name() {return m_name;}
    void setName(QString value) { m_name = value; emit nameChanged();}

    HWND getHwnd() {return m_hwnd;}

signals:
    void nameChanged();

private:
    HWND m_hwnd;
    QString m_name;
};

class PreviewImageProvider : public QQuickImageProvider {

public:
    PreviewImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    { }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

class ListElement : public QObject {
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


BOOL CALLBACK getWindowsListCallback(HWND hWnd, LPARAM lParam);
