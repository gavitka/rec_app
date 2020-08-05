#include "capturethread.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QWindow>
#include <QDebug>
#include <QPainter>
#include <QtWin>
#include <vector>

#include "backend.h"
#include "hooks_dll/mousehook.h"
#include "windows.h"
#include "blwindow.h"

extern BLWindow* wnd;

CaptureWorker::CaptureWorker():
    m_updatetimer(this)
{ }

CaptureWorker::~CaptureWorker() {
}

void CaptureWorker::start()
{
    try {
        Init();
        // Start capture
        QTimer::singleShot(0, this, &CaptureWorker::Capture);
    } catch (std::exception e) {
        qDebug(e.what());
        exit(-1);
    }
}

void CaptureWorker::stop()
{
    if(m_pause == true)
        m_pause = false;
    m_stop = true;
}

void CaptureWorker::togglepause()
{
    this->m_pause = !this->m_pause;
    m_status = this->m_pause ? RECORD_STATUS::Pause : RECORD_STATUS::Rec;
    emit statusChanged();
}

bool CaptureWorker::isPaused() {
    return m_pause;
}

void CaptureWorker::kick()
{
    if(m_sleeptimer.elapsed() >= 3000)
        m_sleeptimer.restart();
}

bool CaptureWorker::CheckWindow()
{
    HWND t = GetForegroundWindow();
    if(m_backEnd->appList()->size() > 0) {
        for(int i = 0; i < m_backEnd->appList()->size(); ++i) {
            auto &w =  m_backEnd->appList()->at(i);
            if(w.hwnd == t && w.hwnd!= m_hwnd && w.selected) {
                m_hwnd = w.hwnd;
                return true;
            }
        }
    }
    return false;
}

void CaptureWorker::Init()
{
    m_screen = QGuiApplication::primaryScreen();
    if (wnd)
        m_screen = wnd->screen();

    m_updatetimer.setInterval(1000);
    connect(&m_updatetimer, &QTimer::timeout, this, &CaptureWorker::update, Qt::DirectConnection);

    m_updatetimer.start();
    m_sleeptimer.start();

    setStatus(RECORD_STATUS::Rec);
    emit statusChanged();

    m_backEnd = BackEnd::getInstance();

    m_playFPS =     m_backEnd->playFPS();
    m_width =       m_backEnd->getWidth();
    m_height =      m_backEnd->getHeight();
    m_recMode =     m_backEnd->recordMode();
    m_bitRate =     m_backEnd->bitRate();

    QString _filename = m_backEnd->fileName();
    m_b1 = _filename.toLatin1();

    QString _tempfilename = QDir().tempPath() + QDir::separator() + "tmp.h264";
    m_b2 = _tempfilename.toLatin1();

    m_asp = (qreal)m_width/m_height;

    CheckWindow();

    int err;

    if (!(oformat = av_guess_format(nullptr, tempfilename(), nullptr)))
        throw std::exception("Failed to define output format");

    if ((err = avformat_alloc_output_context2(&ofctx, oformat, nullptr, tempfilename()) < 0))
        throw std::exception("Failed to allocate output context");

    if (!(codec = avcodec_find_encoder(oformat->video_codec)))
        throw std::exception("Failed to find encoder");

    if (!(videoStream = avformat_new_stream(ofctx, codec)))
        throw std::exception("Failed to create new stream");

    if (!(cctx = avcodec_alloc_context3(codec)))
        throw std::exception("Failed to allocate codec context");

    videoStream->codecpar->codec_id = oformat->video_codec;
    videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    videoStream->codecpar->width = m_width;
    videoStream->codecpar->height = m_height;
    videoStream->codecpar->format = AV_PIX_FMT_YUV420P;
    videoStream->codecpar->bit_rate = m_bitRate * 1000;
    videoStream->time_base = { 1, m_playFPS };

    avcodec_parameters_to_context(cctx, videoStream->codecpar);
    cctx->time_base = { 1, m_playFPS };
    cctx->max_b_frames = 2;
    cctx->gop_size = 12;
    if (videoStream->codecpar->codec_id == AV_CODEC_ID_H264) {
        av_opt_set(cctx, "preset", "ultrafast", 0);
    }
    if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
        cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    avcodec_parameters_from_context(videoStream->codecpar, cctx);

    if ((err = avcodec_open2(cctx, codec, nullptr)) < 0)
        throw std::exception("Failed to open codec");

    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofctx->pb, tempfilename(), AVIO_FLAG_WRITE)) < 0)
            throw std::exception("Failed to open file");
    }

    if ((err = avformat_write_header(ofctx, nullptr)) < 0)
        throw std::exception("Failed to write header");

    av_dump_format(ofctx, 0, tempfilename(), 1);
}

void CaptureWorker::Capture()
{
    if(m_stop) {
        QTimer::singleShot(0, this, &CaptureWorker::Finish);
        return;
    }

    //qDebug() << "working";

    // TODO: Add mutex
    QElapsedTimer frametimer;
    frametimer.start();

    if(!m_pause)
    {
        if(m_sleeptimer.elapsed() < 3000 || !BackEnd::getInstance()->sleepMode()) {
            CaptureFrame();
            checkSleeping(false);
        } else {
            checkSleeping(true);
        }
    }

    qint64 remaining_timeout = getShotTimeout() - frametimer.elapsed();
    remaining_timeout = (remaining_timeout < 0) ? 0: remaining_timeout;
    QTimer::singleShot(remaining_timeout, this, &CaptureWorker::Capture);
}

void CaptureWorker::Finish()
{
    // flush avcodec
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    while(true) {
        avcodec_send_frame(cctx, nullptr);
        if (avcodec_receive_packet(cctx, &pkt) == 0) {
            av_interleaved_write_frame(ofctx, &pkt);
            av_packet_unref(&pkt);
        }
        else break;
    }

    av_write_trailer(ofctx);
    if (!(oformat->flags & AVFMT_NOFILE)) {
        int err = avio_close(ofctx->pb);
        if (err < 0) {
            throw std::exception("Failed to close file");
        }
    }

    Clear();
    Remux();

    setStatus(RECORD_STATUS::Idle);
    emit finished();
}

void CaptureWorker::CaptureFrame()
{
    QImage image;
    if(m_recMode == RECORD_MODE::Window) {
        if(!IsWindow(m_hwnd)) return;
        image = CaptureWindow(m_screen, m_hwnd);
    } else {
        image = CaptureScreen(m_screen);
    }

    image = fixAspectRatio(image);

    int err;
    if (!videoFrame) {

        videoFrame = av_frame_alloc();
        videoFrame->format = AV_PIX_FMT_YUV420P;
        videoFrame->width = cctx->width;
        videoFrame->height = cctx->height;

        if ((err = av_frame_get_buffer(videoFrame, 32)) < 0) {
            throw std::exception("Failed to allocate picture");
        }
    }

    bool reinitialize_context = false;

    // If somebody resized window
    if(image.width() != lastimagewidth || image.height() != lastimageheight){
        reinitialize_context = true;
        lastimagewidth = image.width();
        lastimageheight = image.height();
    }

    if (!swsCtx || reinitialize_context) {
        swsCtx = sws_getContext(
                    image.width(),
                    image.height() - 0,
                    AV_PIX_FMT_BGRA,
                    cctx->width,
                    cctx->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC,
                    nullptr,
                    nullptr,
                    nullptr
                    );
    }

    int srcstride[1];
    srcstride[0] = image.width() * 4;
    const uchar* planes[1];
    planes[0] = image.bits();
    sws_scale(
            swsCtx,
            planes,
            srcstride,
            0,
            image.height() - 0,
            videoFrame->data,
            videoFrame->linesize
            );

    videoFrame->pts = frameCounter++;

    if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
        throw std::exception("Failed to send frame");
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    if (avcodec_receive_packet(cctx, &pkt) == 0) {
        pkt.flags |= AV_PKT_FLAG_KEY;
        av_interleaved_write_frame(ofctx, &pkt);
        av_packet_unref(&pkt);
    }
}

void CaptureWorker::Remux()
{
    AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
    int err;

    try {
        [&](){
            if ((err = avformat_open_input(&ifmt_ctx, tempfilename(), nullptr, nullptr)) < 0) {
                throw std::exception("Failed to open input file for remuxing");
                return;
            }
            if ((err = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
                throw std::exception("Failed to retrieve input stream information");
                return;
            }
            if ((err = avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, filename()))) {
                throw std::exception("Failed to allocate output context");
                return;
            }

            AVStream *inVideoStream = ifmt_ctx->streams[0];
            AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, nullptr);
            if (!outVideoStream) {
                throw std::exception("Failed to allocate output video stream");
                return;
            }
            outVideoStream->time_base = { 1, m_playFPS };
            avcodec_parameters_copy(outVideoStream->codecpar, inVideoStream->codecpar);
            outVideoStream->codecpar->codec_tag = 0;

            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
                if ((err = avio_open(&ofmt_ctx->pb, filename(), AVIO_FLAG_WRITE)) < 0) {
                    throw std::exception("Failed to open output file");
                    return;
                }
            }

            if ((err = avformat_write_header(ofmt_ctx, nullptr)) < 0) {
                throw std::exception("Failed to write header to output file");
                return;
            }

            AVPacket videoPkt;
            int64_t ts = 0;
            while (true) {
                if ((err = av_read_frame(ifmt_ctx, &videoPkt)) < 0) {
                    break;
                }
                videoPkt.stream_index = outVideoStream->index;
                videoPkt.pts = ts;
                videoPkt.dts = ts;
                videoPkt.duration = av_rescale_q(videoPkt.duration, inVideoStream->time_base, outVideoStream->time_base);
                ts += videoPkt.duration;
                videoPkt.pos = -1;

                if ((err = av_interleaved_write_frame(ofmt_ctx, &videoPkt)) < 0) {
                    throw std::exception("Failed to mux packet");
                    av_packet_unref(&videoPkt);
                    break;
                }
                av_packet_unref(&videoPkt);
            }

            av_write_trailer(ofmt_ctx);
        }();
    } catch (std::exception e)
    {
        qDebug() << "e.what()" << e.what();
    }

    if (ifmt_ctx) {
        avformat_close_input(&ifmt_ctx);
    }
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&ofmt_ctx->pb);
    }
    if (ofmt_ctx) {
        avformat_free_context(ofmt_ctx);
    }
}

const char *CaptureWorker::filename()
{
    return m_b1.constData();
}

const char *CaptureWorker::tempfilename()
{
    return m_b2.constData();
}

void CaptureWorker::Clear()
{
    if (videoFrame) {
        av_frame_free(&videoFrame);
    }
    if (cctx) {
        avcodec_free_context(&cctx);
    }
    if (ofctx) {
        avformat_free_context(ofctx);
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    m_updatetimer.stop();
}

QImage CaptureWorker::CaptureScreen(QScreen* screen)
{
    QPixmap pixmap = screen->grabWindow(0);
    QPixmap pixmap_cursor(":/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    p.setX(p.x() - 32);
    p.setY(p.y() - 32);
    painter.drawPixmap(p, pixmap_cursor);

    return pixmap.toImage();
}

QImage CaptureWorker::CaptureWindow(QScreen* screen, HWND hwnd)
{
    QPixmap pixmap = screen->grabWindow((WId)hwnd);
    QCursor cur(Qt::ArrowCursor);
    QPixmap pixmap_cursor(":/images/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    RECT rect;
    GetWindowRect(hwnd, &rect);
    p.setX(p.x() - rect.left - 32);
    p.setY(p.y() - rect.top - 32);
    painter.drawPixmap(p,pixmap_cursor);
    return pixmap.toImage();
}

qint64 CaptureWorker::getShotTimeout()
{
    return (1000 / m_recordFPS);
}

int CaptureWorker::getShotsPerSecond()
{
    return m_recordFPS;
}

// copies QImage
QImage CaptureWorker::fixAspectRatio(QImage img)
{
    // Possible but not major performance loss here
    qreal asp2 = (qreal)img.width()/img.height();
    if((asp2 > m_asp) ^ (m_cropmode == true)) {
        int newheight = (int)img.width()/m_asp;
        return img.copy(0, (img.height() - newheight)/2, img.width(), newheight);
    }
    else {
        int newwidth = (int)img.height()*m_asp;
        return img.copy( (img.width() - newwidth) / 2, 0, newwidth, img.height());
    }
}

int CaptureWorker::status()
{
    return m_status;
}

void CaptureWorker::setStatus(int value)
{
    m_status = value;
    emit statusChanged();
}

bool CaptureWorker::sleeping()
{
    return m_sleepflag;
}

void CaptureWorker::setFrameRate(int value)
{
    m_recordFPS = value;
}

void CaptureWorker::setCrop(bool value)
{
    m_cropmode = value;
}

void CaptureWorker::setBitRate(int value)
{
    m_bitRate = value;
}

void CaptureWorker::update()
{
    CheckWindow();
    emit updateVector();
}

void CaptureWorker::checkSleeping(bool makeSleeping)
{
    if (m_sleepflag != makeSleeping) {
        m_sleepflag = makeSleeping;
        emit sleepingChanged();
    }
}
