#pragma once

#include <QElapsedTimer>
#include <QString>
#include <QtDebug>


struct PerfomanceTimerPoint {
    int index;
    int numtested;
    float avgTime;
    float lastTime;
};

class PerfomanceTimer
{

public:

    static PerfomanceTimer* getInstance() {
        if(!m_instance) {
            m_instance = new PerfomanceTimer();
            m_instance->reset();
        }
        return m_instance;
    }

    void reset() {
        m_timer.start();
        m_lastPointTime = 0;
        m_lastindex = 0;
    }

    void elapsed(QString point) {
        int currentTime = m_timer.elapsed() - m_lastPointTime;
        m_lastPointTime = m_timer.elapsed();
        if(!m_points.contains(point)){
            PerfomanceTimerPoint ptp;
            ptp.index = (m_lastindex ++);
            ptp.numtested = 1;
            ptp.lastTime = currentTime;
            ptp.avgTime = currentTime;
            m_points.insert(point, ptp);
        }
        PerfomanceTimerPoint &ptp = m_points[point];
        ptp.lastTime = currentTime;
        ptp.avgTime = ((ptp.avgTime * ptp.numtested + ptp.lastTime)/(ptp.numtested + 1));
        ptp.numtested++;
        //qDebug() << ptp.index << " " << point << ": " << (int)ptp.lastTime << " avg: " << (int)ptp.avgTime ;
    }

    void getResult(){
        QMapIterator<QString, PerfomanceTimerPoint> i(m_points);
        while (i.hasNext()) {
            i.next();
            //qDebug() << i.value().index << " " << i.key() << " avg: " << i.value().avgTime << endl;
        }
    }

private:

    explicit PerfomanceTimer()
    {
        m_timer.start();
    }

    static PerfomanceTimer* m_instance;
    QElapsedTimer m_timer;
    qint64 m_lastPointTime;
    int m_lastindex;
    QMap<QString, PerfomanceTimerPoint> m_points;

};
