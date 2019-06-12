#pragma once

#include <QThread>
#include <QScreen>
#include <QMutex>

class CaptureThread : public QThread
{

    Q_OBJECT

public:
    CaptureThread();
    void run();
    void Stop() {
        this->m_stop = true;
    }

    void pause() {
        this->m_pause = this->m_pause ? false : true;
    }

    bool isPaused() {
        return m_pause;
    }

    int shots_per_second;

private:
    QImage CaptureScreen();
    bool m_stop;
    bool m_pause;
    QScreen* screen;
    //QMutex mux;

signals:
    void resultReady();

};
