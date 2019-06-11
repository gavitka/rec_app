#define VIDEOCAPTURE_API __declspec(dllexport)

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <string> 

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>

#include <libavdevice/avdevice.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavformat/avformat.h>
#include <libavformat/avio.h>

    // libav resample

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


    // hwaccel
#include "libavcodec/vdpau.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_vdpau.h"

    // lib swresample

#include <libswscale/swscale.h>

    std::ofstream logFile;

    void Log(std::string str) {
        logFile.open("Logs.txt", std::ofstream::app);
        logFile.write(str.c_str(), str.size());
        logFile.close();
    }

    typedef void(*FuncPtr)(const char *);
    FuncPtr ExtDebug;
    char errbuf[32];

    void Debug(std::string str, int err) {
        Log(str + " " + std::to_string(err));
        if (err < 0) {
            av_strerror(err, errbuf, sizeof(errbuf));
            str += errbuf;
        }
        Log(str);
        ExtDebug(str.c_str());
    }

    void avlog_cb(void *, int level, const char * fmt, va_list vargs) {
        static char message[8192];
        vsnprintf_s(message, sizeof(message), fmt, vargs);
        Log(message);
    }

    class VideoCapture {
    public:

        VideoCapture() {
            oformat = NULL;
            ofctx = NULL;
            videoStream = NULL;
            videoFrame = NULL;
            swsCtx = NULL;
            frameCounter = 0;

            // Initialize libavcodec
            av_register_all();
            av_log_set_callback(avlog_cb);
        }

        ~VideoCapture() {
            Free();
        }

        void Init(int width, int height, int fpsrate, int bitrate);

        void AddFrame(uint8_t *data);

        void Finish();

    private:

        AVOutputFormat *oformat;
        AVFormatContext *ofctx;

        AVStream *videoStream;
        AVFrame *videoFrame;

        AVCodec *codec;
        AVCodecContext *cctx;

        SwsContext *swsCtx;

        int frameCounter;

        int fps;

        void Free();

        void Remux();
    };

    VIDEOCAPTURE_API VideoCapture* Init(int width, int height, int fps, int bitrate) {
        VideoCapture *vc = new VideoCapture();
        vc->Init(width, height, fps, bitrate);
        return vc;
    };

    VIDEOCAPTURE_API void AddFrame(uint8_t *data, VideoCapture *vc) {
        vc->AddFrame(data);
    }

    VIDEOCAPTURE_API void Finish(VideoCapture *vc) {
        vc->Finish();
    }

    VIDEOCAPTURE_API void SetDebug(FuncPtr fp) {
        ExtDebug = fp;
    };
}