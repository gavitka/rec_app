#pragma once

#include <QThread>
#include <QScreen>
#include <QMutex>

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(int shots_per_second);
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

private:
    QImage CaptureScreen();
    bool m_stop;
    bool m_pause;
    QScreen* screen;
    int m_shots_per_second;
    //QMutex mux;

signals:
    void resultReady();

};
