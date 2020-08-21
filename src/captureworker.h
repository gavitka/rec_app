#pragma once

#include "pch.h"

#include <QThread>
#include <QScreen>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>

#include "backend.h"


class BackEnd;

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavcodec/avfft.h>
    #include <libavdevice/avdevice.h>
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libavutil/opt.h>
    #include <libavutil/common.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/time.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/file.h>
    #include <libswscale/swscale.h>
}

#define VIDEO_TMP_FILE "tmp.h264"
#define FINAL_FILE_NAME "record.mp4"

enum RECORD_STATUS{
    Idle,
    Rec,
    Pause
};

class CaptureWorker : public QThread
{
    Q_OBJECT

public:
    CaptureWorker();
    ~CaptureWorker();

    void togglepause();

    bool isPaused();
    void kick();

    int FPS();
    qint64 getShotTimeout();
    int getShotsPerSecond();

    int status();
    void setStatus(int value);

    bool sleeping();

    void setFrameRate(int value);
    void setCrop(bool value);
    void setBitRate(int value);

public slots:

    void start();
    void stop();
    void update();
    void Finish();

signals:

    void errorHappened();
    void sleepingChanged();
    void updateVector();
    void finished();
    void statusChanged();

    void InstallHook();
    void UninstallHook();

private:

    void checkSleeping(bool makeSleeping);
    bool CheckWindow();

    void Init();
    void Clear();

    void Capture();
    void CaptureFrame();

    void Free();
    void Remux();

    const char *filename();
    const char *tempfilename();

    QImage fixAspectRatio(QImage img);

    bool m_stop = false;
    bool m_pause = false;
    HWND m_hwnd;
    QScreen* m_screen;

    int m_width;
    int m_height;
    qreal m_asp;
    int m_playFPS = 24;
    int m_recordFPS = 3;
    int m_bitRate;
    bool m_cropmode;
    bool m_sleepflag = false;

    QElapsedTimer m_sleeptimer;
    QTimer m_updatetimer;

    AVCodecContext *cctx = nullptr;
    SwsContext *swsCtx = nullptr;
    AVCodec *codec = nullptr;
    AVOutputFormat *oformat = nullptr;
    AVFormatContext *ofctx = nullptr;
    AVStream *videoStream = nullptr;
    AVFrame *videoFrame = nullptr;

    QByteArray m_b1;
    QByteArray m_b2;

    int frameCounter;

    int fps;
    int lastimagewidth;
    int lastimageheight;

    int m_status;
    BackEnd* m_backEnd;
    QMutex m_mux;
};
