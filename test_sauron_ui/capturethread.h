#pragma once

#include <QThread>
#include <QScreen>
#include <QMutex>

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

private:
    QImage CaptureScreen();
    QImage CaptureWindow();
    bool m_stop;
    bool m_pause;
    HWND m_hwnd;
    QScreen* screen;
    int m_shots_per_second;
    int m_recMode;
    //QMutex mux;

signals:
    void resultReady();

};
