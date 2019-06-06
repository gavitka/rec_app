#include "backend.h"

#include <QDataStream>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QTextStream>
#include <QWindow>

#include <QWidget>
#include <QtWidgets>
#include <QPixmap>
#include <QQmlApplicationEngine>

#include <QThread>

extern QWindow* windowRef;


BackEnd::BackEnd(QObject *parent) : QObject(parent)
{

}

QString BackEnd::outputText()
{
    return m_output_text;
}

void BackEnd::setOutputText(QString s)
{
    if (s== m_output_text) return;

    m_output_text = s;
    emit outputTextChanged();
}

void BackEnd::addOutPutText(QString text)
{
    setOutputText(outputText() + text);
}

void BackEnd::startRecording()
{
    addOutPutText("Recording Started\n");

    QFile file("bullshit.avi");
    QTextStream cout(stderr);

    //declarations

    const AVCodec *codec;
    AVCodecContext *c = nullptr;
    AVFrame *frame;
    AVPacket *pkt;

    int i, ret, x, y;

    static const char endcodedata[] = {'\x00','\x00','\x01','\xb7'};
    QByteArray endcode =  QByteArray::fromRawData(endcodedata, sizeof(endcodedata));

    /* find the mpeg1video encoder */

    AVCodec* p = nullptr;
    while (p) {
        fprintf(stderr, "Codec: %s\n", p->name);
        p = p->next;
    }

    codec = avcodec_find_encoder_by_name("libx264rgb");
    //libx264
    //libx264rgb
    //AV_CODEC_ID_H264
    //AV_CODEC_ID_MPEG1VIDEO

    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return;
    }

    //memory allocation
    c = avcodec_alloc_context3(codec);
    if(!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return;
    }

    pkt = av_packet_alloc();
    if (!pkt)
        return;

    /* put sample parameters */
    c->bit_rate = 8000000;
    /* resolution must be a multiple of two */
    c->width = 800;
    c->height = 600;

    AVRational r1 = {1, 25};
    AVRational r2 = {25, 1};
    c->time_base = r1;
    c->framerate = r2;

    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_RGB24 ;
    //AV_PIX_FMT_RGB24
    //AV_PIX_FMT_YUV420P

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "fast", 0);

    /* open it */
    ret = avcodec_open2(c, codec, nullptr);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec\n");
        return;
    }

    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    //frame->format = AV_PIX_FMT_RGB24;
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        fprintf(stderr, "could not alloc the frame data\n");
        return;
    }

// Later
    QScreen *screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();
    if (!screen)
        return;

    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());

    for (i = 0; i < 1500; i++) {
        fflush(stdout);

//        /* make sure the frame data is writable */

        QThread::msleep(100);

        QPixmap pixmap = screen->grabWindow(0);
        QImage image (pixmap.toImage());

        ret = av_frame_make_writable(frame);
        if (ret < 0)
            return;

        /* prepare a dummy image */
        /* RGB */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                int imagey = (int)(image.height() * ((double)y/c->height));
                int imagex = (int)(image.width() * ((double)x/c->width));
                int imagescanline = image.width();
                int imageindex = imagey * imagescanline + imagex;
                uchar singlepixeldata = *(image.bits() + imageindex * 3);
                Q_UNUSED(singlepixeldata);
                frame->data[0][y * frame->linesize[0] + x * 3 + 0] = *(image.bits() + imageindex * 4 + 0);
                frame->data[0][y * frame->linesize[0] + x * 3 + 1] = *(image.bits() + imageindex * 4 + 1);
                frame->data[0][y * frame->linesize[0] + x * 3 + 2] = *(image.bits() + imageindex * 4 + 2);
            }
        }

//        /* prepare a dummy image */
//        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                frame->data[0][y * frame->linesize[0] + x + 0] = x + y + i * 3;
//                //frame->data[0][y * frame->linesize[0] + x * 3 + 1] = 128 + y + i * 2;
//                //frame->data[0][y * frame->linesize[0] + x * 3 + 2] = 64 + x + i * 5;
//            }
//        }

//        /* Cb and Cr */
//        for (y = 0; y < c->height/2; y++) {
//            for (x = 0; x < c->width/2; x++) {
//                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
//                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }

        frame->pts = i;

        /* encode the image */
        encode2(c, frame, pkt, out);
    }

    /* flush the encoder */
    encode2(c, nullptr, pkt, out);

    /* add sequence end code to have a real MPEG file */
    //out << endcode;

    file.close();

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    addOutPutText("Recording Finished\n");
}

void BackEnd::stopRecording()
{
    addOutPutText("Recording Stopped\n");

}

static void encode2(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
   QDataStream &out)
{
   int ret;

   /* send the frame to the encoder */
//   if (frame)
//      printf("Send frame %3"PRId64"\n", frame->pts);

   ret = avcodec_send_frame(enc_ctx, frame);
   if (ret < 0) {
       fprintf(stderr, "Error sending a frame for encoding\n");
       return;
   }

   while (ret >= 0) {
       ret = avcodec_receive_packet(enc_ctx, pkt);
       if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
           return;
       else if (ret < 0) {
           fprintf(stderr, "Error during encoding\n");
           return;
       }

//       printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
       out.writeRawData(reinterpret_cast<const char*>(pkt->data), pkt->size);
       av_packet_unref(pkt);
   }
}





//        /* prepare a dummy image */
//        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                picture->data[0][y * picture->linesize[0] + x] = x + y + i * 3;
//            }
//        }

//        /* Cb and Cr */
//        for (y = 0; y < c->height / 2; y++) {
//            for (x = 0; x < c->width / 2; x++) {
//                picture->data[1][y * picture->linesize[1] + x] = 128 + y + i * 2;
//                picture->data[2][y * picture->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }
