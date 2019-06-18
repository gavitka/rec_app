#pragma once

#include "pch.h"

#include <QFile>
#include <QObject>
#include <QQmlEngine>
#include <thread>
#include <QList>
#include <QSettings>

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

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString outputText READ outputText WRITE setOutputText NOTIFY outputTextChanged)
    Q_PROPERTY(bool stopEnabled READ stopEnabled NOTIFY stopEnabledChanged)
    Q_PROPERTY(bool lockParam READ lockParam NOTIFY lockParamChanged)

    Q_PROPERTY(QString startButtonText READ startButtonText NOTIFY startButtonTextChanged)

    Q_PROPERTY(int outWidth READ outWidth WRITE setOutWidth NOTIFY outWidthChanged)
    Q_PROPERTY(int outHeight READ outHeight WRITE setOutHeight NOTIFY outHeightChanged)

    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)
    Q_PROPERTY(int shotsPerSecond READ shotsPerSecond WRITE setShotsPerSecond NOTIFY shotsPerSecondChanged)

    Q_PROPERTY(QString outFileName READ outFileName WRITE setOutFileName NOTIFY outFileNameChanged)

    Q_PROPERTY(QList<QObject*> windowList READ windowList NOTIFY windowListChanged)

    Q_PROPERTY(int mouseX READ mouseX WRITE setMouseX NOTIFY mouseXChanged)
    Q_PROPERTY(int mouseY READ mouseY WRITE setMouseX NOTIFY mouseYChanged)

    Q_PROPERTY(bool recMode READ recMode WRITE setRecMode NOTIFY recModeChanged)

public:

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

    BackEnd(QObject *parent = nullptr) :
        QObject(parent),
        m_thr(nullptr)
    {
        setOutWidth(1920/4);
        setOutHeight(1200/4);
        setFramesPerSecond(25);
        setShotsPerSecond(3);
        setOutFileName("c:/dev/rec_app/filename.mp4");
        setRecordStatus(RECORD_STATUS::Idle);
        m_recMode = RECORD_MODE::Screen;

        refreshUI();
    }

    void kick(){
        if(m_thr != nullptr) {
            m_thr->kick();
        }
    }


    void addOutPutText(QString text) {
        setOutputText(outputText() + text);
    }

    /* <PROP> */

    QString outputText() {return m_output_text;}
    void setOutputText(QString s);

    bool stopEnabled();

    QString startButtonText();

    int mouseX(){return m_mousex;}
    void setMouseX(int value){m_mousex = value; emit mouseXChanged();}

    int mouseY(){return m_mousey;}
    void setMouseY(int value){m_mousey = value; emit mouseYChanged();}

    int outWidth(){return m_outWidth;}
    void setOutWidth(int value){
        m_outWidth = value;
        emit outWidthChanged();
    }

    int outHeight(){return m_outHeight;}
    void setOutHeight(int value){
        m_outHeight = value;
        emit outHeightChanged();
    }

    int framesPerSecond(){return m_framesPerSecond;}
    void setFramesPerSecond(int value){
        m_framesPerSecond = value;
        emit framesPerSecondChanged();
    }

    int shotsPerSecond(){return m_shotsPerSecond;}
    void setShotsPerSecond(int value){
        m_shotsPerSecond = value;
        emit shotsPerSecondChanged();
    }

    QString outFileName(){return m_outFileName;}
    void setOutFileName(QString value);

    bool lockParam();

    int recordStatus(){return m_record_status;}
    void setRecordStatus(int value){
        m_record_status = value;
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
    /* </PROP> */

    QSettings* getSettings(){return &m_settings;}


signals:

    void outputTextChanged();
    void stopEnabledChanged();
    void startButtonTextChanged();
    void outWidthChanged();
    void outHeightChanged();
    void framesPerSecondChanged();
    void shotsPerSecondChanged();
    void outFileNameChanged();
    void lockParamChanged();
    void windowListChanged();
    void mouseXChanged();
    void mouseYChanged();
    void recModeChanged();

public slots:

    void startRecording();
    void stopRecording();
    void handleResults();
    void refreshUI();
    void getWindowsList();
    void setWindow(int index);

private:

    static BackEnd* m_instance;
    QString m_output_text;
    CaptureThread* m_thr;

    int m_outWidth;
    int m_outHeight;

    int m_mousex;
    int m_mousey;

    int m_framesPerSecond;
    int m_shotsPerSecond;
    bool m_lockParam;

    int m_record_status;
    int m_recMode;

    QString m_outFileName;
    QList<QObject*> m_dataList;
    HWND m_hwnd;
    QSettings m_settings;



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


BOOL CALLBACK getWindowsListCallback(HWND hWnd, LPARAM lParam);
