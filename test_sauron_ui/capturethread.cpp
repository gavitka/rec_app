#include "capturethread.h"
#include "VideoCapture.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QWindow>

#include "backend.h"

extern QWindow* windowRef;

CaptureThread::CaptureThread(int shots_per_second):
    m_stop (false),
    m_pause (false),
    m_shots_per_second (shots_per_second)
{
    screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();

    m_shots_per_second = 12;
}

void CaptureThread::run() {
    int w, h, fps;
    const char* fpussy;

    w = BackEnd::getInstance()->outWidth();
    h = BackEnd::getInstance()->outHeight();
    fps = BackEnd::getInstance()->framesPerSecond();
    QByteArray ba = BackEnd::getInstance()->outFileName().toLatin1();
    fpussy = ba.data();

    VideoCapture vc;
    vc.Init(w, h, fps, 2500, fpussy);
    while (true) {

        while(m_pause) {
          QThread::msleep(100);
        }

        vc.AddFrame(CaptureWindow());

        if(m_stop) {
            break;
        }

        QThread::msleep((int)1000/m_shots_per_second);
    }
    vc.Finish();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen() {
    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());
    return image;
}

QImage CaptureThread::CaptureWindow()
{
    QPixmap pixmap = QPixmap::grabWindow((WId)BackEnd::getInstance()->getHwnd());
    QImage image (pixmap.toImage());
    return image;
}
