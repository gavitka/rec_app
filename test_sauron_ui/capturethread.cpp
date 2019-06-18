#include "capturethread.h"
#include "VideoCapture.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QWindow>
#include <QDebug>
#include <QPainter>
#include <QtWin>

#include "backend.h"
#include "mousehook.h"
#include "perfomancetimer.h"

extern QWindow* windowRef;

CaptureThread::CaptureThread(int shots_per_second):
    m_stop (false),
    m_pause (false),
    m_shots_per_second (shots_per_second)
{
    screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();

    m_currentFPS = m_shots_per_second;
    m_shotToumeOut = static_cast<int>(1000/m_shots_per_second);

    m_recMode = BackEnd::getInstance()->recordMode();
    m_hwnd = BackEnd::getInstance()->getHwnd();

    if(m_recMode == RECORD_MODE::Window) {
        // qDebug() << "Installing mouse hook";
        InstallMouseHook((HWND)windowRef->winId(), m_hwnd);
    }

    m_timer.start();
}

CaptureThread::~CaptureThread()
{
    RemoveHooks();
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
        QElapsedTimer timer;
        timer.start();
        PerfomanceTimer::getInstance()->reset();
        while(m_pause) {
            QThread::msleep(100);
        }
        if(m_recMode == RECORD_MODE::Window) {
            if (m_timer.elapsed() < 2000) {
                BackEnd::getInstance()->addOutPutText("Recording Frame\n");
                vc.AddFrame(CaptureWindow());
            }
            else {
                qDebug() << "Sleeping...." ;
            }
        }
        else {
            BackEnd::getInstance()->addOutPutText("Recording Frame\n");
            vc.AddFrame(CaptureScreen());
        }
        if(m_stop) {
            break;
        }
        //        qDebug() << "Shot timeout:" << m_shotToumeOut
        //                 << "Elapsed time:" << timer.elapsed();
        qint64 remainingTime = m_shotToumeOut - timer.elapsed();
        if(remainingTime > 0) {
            QThread::msleep(remainingTime);
            m_currentFPS = m_shots_per_second;
        }
        else {
            m_currentFPS = (int) 1000 / timer.elapsed();
        }
        //        qDebug() << "Remaining Time " << remainingTime;
        qDebug() << "FPS:" << m_currentFPS;
    }
    vc.Finish();

    PerfomanceTimer::getInstance()->getResult();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen() {
    QPixmap pixmap = screen->grabWindow(0);
    PerfomanceTimer::getInstance()->elapsed("Sreenshot_grabbing");
    QPixmap pixmap_cursor(":/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    PerfomanceTimer::getInstance()->elapsed("Draw_cursor");
    QImage image (pixmap.toImage());
    PerfomanceTimer::getInstance()->elapsed("Screenshot_to_image");
    return image;
}

QImage CaptureThread::CaptureWindow() {
    QPixmap pixmap = screen->grabWindow((WId)m_hwnd);
    QPixmap pixmap_cursor(":/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    QImage image (pixmap.toImage());
    return image;
}

QImage CaptureThread::CaptureScreen2(){
    // get the device context of the screen
    const wchar_t* dcName = L"DISPLAY";

    HDC hScreenDC = CreateDC(dcName, nullptr, nullptr, nullptr);
    // and a device context to put it in
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    int width = GetDeviceCaps(hScreenDC, HORZRES);
    int height = GetDeviceCaps(hScreenDC, VERTRES);

    // maybe worth checking these are positive values
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

    QImage image(QtWin::imageFromHBITMAP(hScreenDC, hBitmap, width, height));

    // get a new bitmap
    HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemoryDC, hBitmap);/**/

    PerfomanceTimer::getInstance()->elapsed("Capturing bitmap");

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP) SelectObject(hMemoryDC, hOldBitmap);

    PerfomanceTimer::getInstance()->elapsed("BitBlt");

    //QImage image(QtWin::imageFromHBITMAP( hBitmap));

    PerfomanceTimer::getInstance()->elapsed("Creating QImage");

    // clean up
    DeleteDC(hMemoryDC);
    DeleteDC(hScreenDC);

    return image;
    // now your image is held in hBitmap. You can save it or do whatever with it
}

