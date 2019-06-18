#pragma once

#include "pch.h"

#include <QThread>
#include <QScreen>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(int shots_per_second);
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

private:
    QImage CaptureScreen();
    QImage CaptureScreen2();
    QImage CaptureWindow();
    bool m_stop;
    bool m_pause;
    HWND m_hwnd;
    QScreen* screen;
    int m_shots_per_second;
    int m_recMode;
    QElapsedTimer m_timer;
    qint64 m_shotToumeOut;
    int m_currentFPS = 0;
    //QMutex mux;

signals:
    void resultReady();

};
