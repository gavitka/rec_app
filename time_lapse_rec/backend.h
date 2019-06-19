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
#include <QQuickImageProvider>
#include <QRandomGenerator>

#include"capturethread.h"

enum RECORD_STATUS{
    Idle,
    Rec,
    Pause
};

enum RECORD_MODE {
    Screen,
    Window
};

class WindowObject;
class ResolutionObject;
class BitrateObject;
class FramerateObject;

QString int2str(int i);

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool lockParam READ lockParam NOTIFY lockParamChanged)
//    Q_PROPERTY(QString startButtonText READ startButtonText NOTIFY startButtonTextChanged) //deleteme
////    Q_PROPERTY(int outWidth READ outWidth WRITE setOutWidth NOTIFY outWidthChanged) //deleteme
////    Q_PROPERTY(int outHeight READ outHeight WRITE setOutHeight NOTIFY outHeightChanged) //deleteme
//    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged) //deleteme
//    Q_PROPERTY(int shotsPerSecond READ shotsPerSecond WRITE setShotsPerSecond NOTIFY shotsPerSecondChanged) //deleteme
//    Q_PROPERTY(QString outFileName READ outFileName WRITE setOutFileName NOTIFY outFileNameChanged) //deleteme
    Q_PROPERTY(QList<QObject*> windowList READ windowList NOTIFY windowListChanged)
//    Q_PROPERTY(int mouseX READ mouseX WRITE setMouseX NOTIFY mouseXChanged)
//    Q_PROPERTY(int mouseY READ mouseY WRITE setMouseX NOTIFY mouseYChanged)
    Q_PROPERTY(bool recMode READ recMode WRITE setRecMode NOTIFY recModeChanged)
    Q_PROPERTY(QString filePrefix READ filePrefix WRITE setFilePrefix NOTIFY filePrefixChanged)
    Q_PROPERTY(QString recordingState READ recordingState NOTIFY recordingStateChanged)
    Q_PROPERTY(QString recordingTime READ recordingTime NOTIFY recordingTimeChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QString fileUrl READ fileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QString imageSource READ imageSource WRITE setImageSource NOTIFY imageSourceChanged)
    Q_PROPERTY(bool sleepMode READ sleepMode WRITE setSleepMode NOTIFY sleepModeChanged)

public:

    // ----------------- <SINGLETON>  -----------------

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);
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

    void kick() {
        if(m_thr != nullptr) {
            m_thr->kick();
        }
    }

    // ----------------- <PROP>  -----------------

    QString startButtonText();

//    int mouseX(){return m_mousex;}
//    void setMouseX(int value){m_mousex = value; emit mouseXChanged();}

//    int mouseY(){return m_mousey;}
//    void setMouseY(int value){m_mousey = value; emit mouseYChanged();}

//    int outWidth(){return m_outWidth;}
//    void setOutWidth(int value){
//        m_outWidth = value;
//        emit outWidthChanged();
//    }

//    int outHeight(){return m_outHeight;}
//    void setOutHeight(int value){
//        m_outHeight = value;
//        emit outHeightChanged();
//    }

//    int framesPerSecond(){return m_framesPerSecond;}
//    void setFramesPerSecond(int value){
//        m_framesPerSecond = value;
//        emit framesPerSecondChanged();
//    }

//    int shotsPerSecond(){return m_shotsPerSecond;}
//    void setShotsPerSecond(int value){
//        m_shotsPerSecond = value;
//        emit shotsPerSecondChanged();
//    }

//    QString outFileName(){return m_outFileName;}
//    void setOutFileName(QString value);

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

    HWND getHwnd(){return m_hwnd;}

    QString filePrefix() {return m_filePrefix;}
    void setFilePrefix(QString value) {
        if (value.right(1) == "_") {value = value.left(value.length()-1);}
        m_filePrefix = value;
        emit filePrefixChanged();
    }

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
    void setFilePath(QString value) {
        QUrl u = QUrl(value);
        QDir d = QDir(u.toLocalFile());
        if(d.exists()) {
            m_filePath = d.absolutePath();
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
        emit sleepModeChanged();
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

//    void outputTextChanged(); //deleteme
//    void stopEnabledChanged(); //deleteme
//    void startButtonTextChanged(); //deleteme
//    void outWidthChanged(); //deleteme
//    void outHeightChanged(); //deleteme
//    void framesPerSecondChanged(); //deleteme
//    void shotsPerSecondChanged(); //deleteme
//    void outFileNameChanged(); //deleteme
    void lockParamChanged();
    void windowListChanged();
//    void mouseXChanged();
//    void mouseYChanged();
    void recModeChanged();
    void filePrefixChanged();
    void recordingStateChanged();
    void recordingTimeChanged();
    void filePathChanged();
    void fileUrlChanged();
    void imageSourceChanged();
    void sleepModeChanged();

public slots:

    void startRecording();
    void pauseRecording();
    void stopRecording();
    void handleResults();
    void refreshUI();
    void getWindowsList();
    void setWindow(int index);
    void timerUpdate() {
        emit recordingTimeChanged();
    }
    void setResolution(int i);
    void setBitRate(int i);
    void setFrameRate(int i);
    QScreen* getScreen(){return m_screen;}

private:

    static BackEnd* m_instance;
    QString m_output_text;
    CaptureThread* m_thr;
    int m_width;
    int m_height;
//    int m_mousex;
//    int m_mousey;
    int m_framesPerSecond;
    int m_shotsPerSecond;
    bool m_lockParam;
    int m_record_status;
    int m_recMode;
    int m_bitRate;
    QString m_outFileName;
    QList<QObject*> m_dataList;
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

class FRAMERATES : public QObject {
    Q_OBJECT

public:
    FRAMERATES() : QObject() { }

    enum en{
        x1 = 1, x2, x4, x8, x16
    };
    Q_ENUMS(en)

    static void declareQML() {
        qmlRegisterType<FRAMERATES>("io.qt.examples.enums", 1, 0, "FRAMERATES");
    }
};

class RESOLUTIONS : public QObject {
    Q_OBJECT

public:
    RESOLUTIONS() : QObject() { }

    enum en{
        res1080p = 1, res720p, res360p
    };
    Q_ENUMS(en)

    static void declareQML() {
        qmlRegisterType<RESOLUTIONS>("io.qt.examples.enums", 1, 0, "RESOLUTIONS");
    }
};

class BITRATES : public QObject {
    Q_OBJECT

public:
    BITRATES() : QObject() { }

    enum en{
        b500 = 1, b1000, b1500, b2000, b2500, b3000
    };
    Q_ENUMS(en)

    static void declareQML() {
        qmlRegisterType<BITRATES>("io.qt.examples.enums", 1, 0, "BITRATES");
    }
};

BOOL CALLBACK getWindowsListCallback(HWND hWnd, LPARAM lParam);
