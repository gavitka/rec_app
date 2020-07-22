#pragma once

#include "pch.h"

#include <QThread>
#include <QScreen>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>


#include "backend.h"

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread();
    ~CaptureThread();

    void run();
    void Stop() {
        if(isPaused()) togglepause();
        this->m_stop = true;
    }

    void togglepause() {
        this->m_pause = this->m_pause ? false : true;
    }

    bool isPaused() {
        return m_pause;
    }

    HWND getHwnd(){return m_hwnd;}
    void setHwnd(HWND value){
        m_hwnd = value;
    }

    void kick() {
        m_timer.restart();
    }

    int FPS(){return m_currentFPS;}

    static QImage CaptureScreen(QScreen* screen);
    static QImage CaptureScreen2(QScreen* screen);
    static QImage CaptureWindow(QScreen* screen, HWND hwnd);

    qint64 getShotTimeout(){
        return (1000 / getShotsPerSecond());
    }

    int getShotsPerSecond(){
        return BackEnd::getInstance()->shotsPerSecond();
    }

    qint64 getRemainingTime() {

    }

    QImage fixAspectRatio(QImage img);

private:
    void checkSleeping(bool makeSleeping);

    bool m_stop;
    bool m_pause;
    HWND m_hwnd;
    QScreen* m_screen;
    //int m_shots_per_second;
    //qint64 m_shotTimeOut;
    int m_recMode;
    QElapsedTimer m_timer;
    int m_currentFPS = 0;

    int m_width;
    int m_height;
    qreal m_asp;
    int m_fps;
    int m_bitRate;
    int m_cropmode;
    bool m_sleepflag;

    QByteArray m_filenameb;
    const char* m_filename;
    //QMutex mux;

signals:
    void resultReady();
    void errorHappened();
    void sleepingSignal(bool sleepflag);

};
