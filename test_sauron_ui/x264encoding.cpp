#include "x264encoding.h"
#include <QFile>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QWindow>
#include <stdio.h>

extern QWindow* windowRef;


void start_encoding() {

    encoder_init();
}


void encoder_init() {

    x264_t          *context; // check
    int ret;

    // Parameters

    x264_param_t    param; // check

    x264_param_default_preset(&param, "veryfast", "zerolatency");

    param.i_threads                = 1;
    param.rc.i_bitrate             = 10000;

    param.i_log_level              = X264_LOG_WARNING;
    //param.rc.i_rc_method           = X264_RC_ABR;

    param.i_csp                    = X264_CSP_I420;

    param.i_width                  = 352;
    param.i_height                 = 288;

    param.i_fps_num                = 30;
    param.i_fps_den                = 1;
    param.rc.f_rf_constant         = 25;
    param.rc.f_rf_constant_max     = 35;

    param.b_intra_refresh          = 1;
    param.b_repeat_headers         = 1;
    param.b_annexb                 = 1;

    x264_param_apply_profile(&param, "baseline");

    QFile file("C:/dev/rec_app/bullshit.mp4");

    context = x264_encoder_open(&param);
    if(context == nullptr) {
        fprintf(stderr, "Shit!\n");
        return;
    }

    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);

    x264_nal_t      *nals;
    int             nal_count;

    ret = x264_encoder_headers( context, &nals, &nal_count );
    if (ret < 0) {
        fprintf(stderr, "encode failed\n");
        return;
    }
    for (int i = 0;  i < nal_count ; ++i)
    {
        out.writeRawData(reinterpret_cast<const char*>(nals[i].p_payload), nals[i].i_payload);
        fprintf(stderr, "Writing headers %d\n", nals->i_payload);
    }

    for (int i = 0; i < 125; ++i)
    {
        encode_frame(context, param, out);
    }


    fprintf(stderr, "Done\n");
    file.close();

}

void encode_frame(x264_t *context, x264_param_t &param, QDataStream &out) {

    x264_nal_t      *nals;
    int             nal_count;
    int             ret;
    x264_picture_t  pic, pic_out;

    int             width, height;

    width = param.i_width;
    height = param.i_height;

    QScreen *screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();
    if (!screen)
        return;

    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());

    x264_picture_alloc(&pic, X264_CSP_I420, width, height);

    struct SwsContext* convertCtx = sws_getContext(image.width(), image.height(),
                                                   AV_PIX_FMT_RGBA, width, height,
                                                   AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR,
                                                   nullptr, nullptr, nullptr);

    int srcstride[1];
    srcstride[0] = image.width()*4;
    const uchar* planes[1];
    planes[0] = image.bits();
    sws_scale(convertCtx, planes,
              srcstride, 0, image.height(), pic.img.plane, pic.img.i_stride);

    ret = x264_encoder_encode(context, &nals, &nal_count, &pic, &pic_out);
    if (ret < 0) {
        fprintf(stderr, "encode failed\n");
        return;
    }

    for (int i = 0;  i < nal_count ; ++i)
    {
        out.writeRawData(reinterpret_cast<const char*>(nals[i].p_payload), nals[i].i_payload);
        fprintf(stderr, "Writing payload %d\n", nals->i_payload);
    }
}
