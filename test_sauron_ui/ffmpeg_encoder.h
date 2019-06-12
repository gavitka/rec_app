#pragma once

#include <QFile>
#include <QScreen>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class ffmpeg_encoder
{
public:
    ffmpeg_encoder();
    ~ffmpeg_encoder();

    void encode();

private:
    QFile file;
    const AVCodec *codec;
    AVCodecContext *c;
    QByteArray endcode;
    QScreen *screen;
    int width;
    int height;

    AVOutputFormat *oformat;
    AVFormatContext *ofctx;

    AVStream *videoStream;
    AVFrame *videoFrame;

    void encode2(AVFrame *frame, AVPacket *pkt, QDataStream &out);

};
