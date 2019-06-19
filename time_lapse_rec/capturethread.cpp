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

extern QWindow* wnd;

CaptureThread::CaptureThread():
    m_stop (false),
    m_pause (false)
{
    m_screen = QGuiApplication::primaryScreen();
    if (wnd)
        m_screen = wnd->screen();

    m_shots_per_second = BackEnd::getInstance()->shotsPerSecond();
    m_shotTimeOut = static_cast<int>(1000/m_shots_per_second);

    m_currentFPS = m_shots_per_second;
    m_fps = BackEnd::getInstance()->framesPerSecond();
    m_width = BackEnd::getInstance()->getWidth();
    m_height = BackEnd::getInstance()->getHeight();
    m_recMode = BackEnd::getInstance()->recordMode();
    m_hwnd = BackEnd::getInstance()->getHwnd();
    m_bitRate = BackEnd::getInstance()->bitRate();

    QString s = BackEnd::getInstance()->fileName();
    qDebug() << "filename from BackEnd" << s;
    m_filenameb = BackEnd::getInstance()->fileName().toLatin1();
    m_filename = m_filenameb.data();

    if(m_recMode == RECORD_MODE::Window) {
        InstallMouseHook((HWND)wnd->winId(), m_hwnd);
    }

    m_timer.start();
}

CaptureThread::~CaptureThread()
{
    //delete m_filename;
    RemoveHooks();
}

void CaptureThread::run() {
    VideoCapture vc;
    vc.Init(m_width, m_height, m_fps, 2500, m_filename);
    while (true) {
        QElapsedTimer timer;
        timer.start();
        PerfomanceTimer::getInstance()->reset();
        while(m_pause) {
            QThread::msleep(100);
        }
        if(m_recMode == RECORD_MODE::Window) {
            if (m_timer.elapsed() < 2000) {
                vc.AddFrame(CaptureThread::CaptureWindow(m_screen, m_hwnd));
            }
            else {
                qDebug() << "Sleeping...." ;
            }
        }
        else {
            vc.AddFrame(CaptureThread::CaptureScreen(m_screen));
        }
        if(m_stop) {
            break;
        }
        qint64 remainingTime = m_shotTimeOut - timer.elapsed();
        if(remainingTime > 0) {
            QThread::msleep(remainingTime);
            m_currentFPS = m_shots_per_second;
        }
        else {
            m_currentFPS = (int) 1000 / timer.elapsed();
        }
        qDebug() << "FPS:" << m_currentFPS;
    }
    vc.Finish();

    PerfomanceTimer::getInstance()->getResult();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen(QScreen* screen) {
    QPixmap pixmap = screen->grabWindow(0);
    PerfomanceTimer::getInstance()->elapsed("Sreenshot_grabbing");
    QPixmap pixmap_cursor("qrc:/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    PerfomanceTimer::getInstance()->elapsed("Draw_cursor");
    QImage image (pixmap.toImage());
    PerfomanceTimer::getInstance()->elapsed("Screenshot_to_image");
    return image;
}

QImage CaptureThread::CaptureWindow(QScreen* screen, HWND hwnd) {
    QPixmap pixmap = screen->grabWindow((WId)hwnd);
    QPixmap pixmap_cursor("qrc:/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    QImage image (pixmap.toImage());
    return image;
}

QImage CaptureThread::CaptureScreen2(QScreen* screen){
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

