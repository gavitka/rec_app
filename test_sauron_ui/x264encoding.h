#pragma once

#define MAX_AV_PLANES 8
#include <QDataStream>
#include <stdint.h>

#define DEBUG_SWSCALE_BUFFERS 1

extern "C" {
#include "x264.h"

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

void start_encoding();

void encode_frame(x264_t *context, x264_param_t &param, QDataStream &out);

void encoder_init();

struct encoder_frame {
    /** Data for the frame/audio */
    uint8_t               *data[MAX_AV_PLANES];

    /** size of each plane */
    uint32_t              linesize[MAX_AV_PLANES];

    /** Number of frames (audio only) */
    uint32_t              frames;

    /** Presentation timestamp */
    int64_t               pts;
};
