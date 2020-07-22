#include "capturethread.h"
#include "VideoCapture.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QWindow>
#include <QDebug>
#include <QPainter>
#include <QtWin>
#include <vector>

#include "backend.h"
#include "hooks_dll/mousehook.h"
#include "perfomancetimer.h"
#include "windows.h"

extern QWindow* wnd;

CaptureThread::CaptureThread():
    m_stop (false),
    m_pause (false),
    m_updatetimer(this)
{
    m_screen = QGuiApplication::primaryScreen();
    if (wnd)
        m_screen = wnd->screen();

    m_currentFPS = getShotsPerSecond();
    m_fps = BackEnd::getInstance()->framesPerSecond();
    m_width = BackEnd::getInstance()->getWidth();
    m_height = BackEnd::getInstance()->getHeight();
    m_cropmode = BackEnd::getInstance()->cropIndex();
    m_asp = (qreal)m_width/m_height;
    m_recMode = BackEnd::getInstance()->recordMode();
    m_hwnd = BackEnd::getInstance()->getHwnd();
    m_bitRate = BackEnd::getInstance()->bitRate();
    m_sleepflag = false;

    QString s = BackEnd::getInstance()->fileName();

    m_filenameb = BackEnd::getInstance()->fileName().toLatin1();
    m_filename = m_filenameb.data();

    // Set up update window handles

    m_windowHandles = new std::vector<HWND>();
    m_updatetimer.setInterval(1000);
    connect(&m_updatetimer, &QTimer::timeout, this, &CaptureThread::update);
    connect(this, &CaptureThread::requestVector, (BackEnd::getInstance())->appList(), &AppList::updateVector);
    InstallMultiHook((HWND)wnd->winId(), m_windowHandles);
    m_updatetimer.start();

    m_timer.start();
}

CaptureThread::~CaptureThread() {
    m_updatetimer.stop();
    UninstallMultiHook();
}

void CaptureThread::run() {
    VideoCapture vc;
    vc.Init(m_width, m_height, m_fps, 2500, m_filename); // TODO: use actual bitrate
    while (true) {
        QElapsedTimer timer;
        timer.start();
        PerfomanceTimer::getInstance()->reset();
        while(m_pause) {
            QThread::msleep(100);
        }
        if (m_timer.elapsed() < 3000 || !BackEnd::getInstance()->sleepMode()) {
            if(m_recMode == RECORD_MODE::Window) {
                if(IsWindow(m_hwnd)) {
                    QImage img2 = fixAspectRatio(CaptureThread::CaptureWindow(m_screen, m_hwnd));
                    PerfomanceTimer::getInstance()->elapsed("Image_crop");
                    vc.AddFrame(img2);
                } else {
                    if(m_stop == false) {
                        emit errorHappened();
                    }
                }
            }
            else {
                QImage img2 = fixAspectRatio(CaptureThread::CaptureScreen(m_screen));
                PerfomanceTimer::getInstance()->elapsed("Image_crop");
                vc.AddFrame(img2);
            }
            checkSleeping(false);
        }
        else {
            checkSleeping(true);
            qDebug() << "Sleeping...";
        }
        if(m_stop) {
            break;
        }
        qint64 remainingTime = getShotTimeout() - timer.elapsed();
        if(remainingTime > 0) {
            QThread::msleep((ulong)remainingTime);
            m_currentFPS = getShotsPerSecond();
        }
        else {
            m_currentFPS = (int) 1000 / timer.elapsed();
        }
        //qDebug() << "FPS:" << m_currentFPS;
    }
    vc.Finish();

    PerfomanceTimer::getInstance()->getResult();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen(QScreen* screen) {
    QPixmap pixmap = screen->grabWindow(0);
    PerfomanceTimer::getInstance()->elapsed("Sreenshot_grabbing");
    QPixmap pixmap_cursor(":/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    p.setX(p.x() - 32);
    p.setY(p.y() - 32);
    painter.drawPixmap(p, pixmap_cursor);
    PerfomanceTimer::getInstance()->elapsed("Draw_cursor");
    QImage image (pixmap.toImage());
    PerfomanceTimer::getInstance()->elapsed("Screenshot_to_image");
    return image;
}

QImage CaptureThread::CaptureWindow(QScreen* screen, HWND hwnd) {
    QPixmap pixmap = screen->grabWindow((WId)hwnd);
    QCursor cur(Qt::ArrowCursor);// = w.cursor();
    QPixmap pixmap_cursor(":/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    RECT rect;
    GetWindowRect(hwnd, &rect);
    p.setX(p.x() - rect.left - 32);
    p.setY(p.y() - rect.top - 32);
    painter.drawPixmap(p,pixmap_cursor);
    QImage image (pixmap.toImage());
    return image;
}

QImage CaptureThread::fixAspectRatio(QImage img) {
    // Possible but not major performance loss here
    m_cropmode = BackEnd::getInstance()->cropIndex();
    qreal asp2 = (qreal)img.width()/img.height();
    if((asp2 > m_asp) ^ (m_cropmode == CROP_MODE::Crop)) {
        int newheight = (int)img.width()/m_asp;
        return img.copy(0, (img.height() - newheight)/2, img.width(), newheight);
    }
    else {
        int newwidth = (int)img.height()*m_asp;
        return img.copy( (img.width() - newwidth) / 2, 0, newwidth, img.height());
    }
}

void CaptureThread::update()
{
    emit requestVector(m_windowHandles);
}

void CaptureThread::checkSleeping(bool makeSleeping) {
    if (m_sleepflag != makeSleeping) {
        m_sleepflag = makeSleeping;
        emit sleepingSignal(m_sleepflag);
    }
}

QImage CaptureThread::CaptureScreen2(QScreen* screen) {
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

