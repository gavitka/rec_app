#include "VideoCapture.h"

#define VIDEO_TMP_FILE "tmp.h264"
#define FINAL_FILE_NAME "record.mp4"


using namespace std;

void VideoCapture::Init(int width, int height, int fpsrate, int bitrate) {

    fps = fpsrate;

    int err;

    if (!(oformat = av_guess_format(nullptr, VIDEO_TMP_FILE, nullptr))) {
        fprintf(stderr,"Failed to define output format");
        return;
    }

    if ((err = avformat_alloc_output_context2(&ofctx, oformat, nullptr, VIDEO_TMP_FILE) < 0)) {
        fprintf(stderr,"Failed to allocate output context");
        Free();
        return;
    }

    if (!(codec = avcodec_find_encoder(oformat->video_codec))) {
       fprintf(stderr,"Failed to find encoder");
        Free();
        return;
    }

    if (!(videoStream = avformat_new_stream(ofctx, codec))) {
        fprintf(stderr,"Failed to create new stream");
        Free();
        return;
    }

    if (!(cctx = avcodec_alloc_context3(codec))) {
        fprintf(stderr,"Failed to allocate codec context");
        Free();
        return;
    }

    videoStream->codecpar->codec_id = oformat->video_codec;
    videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    videoStream->codecpar->width = width;
    videoStream->codecpar->height = height;
    videoStream->codecpar->format = AV_PIX_FMT_YUV420P;
    videoStream->codecpar->bit_rate = bitrate * 1000;
    videoStream->time_base = { 1, fps };

    avcodec_parameters_to_context(cctx, videoStream->codecpar);
    cctx->time_base = { 1, fps };
    cctx->max_b_frames = 2;
    cctx->gop_size = 12;
    if (videoStream->codecpar->codec_id == AV_CODEC_ID_H264) {
        av_opt_set(cctx, "preset", "ultrafast", 0);
    }
    if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
        cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    avcodec_parameters_from_context(videoStream->codecpar, cctx);

    if ((err = avcodec_open2(cctx, codec, nullptr)) < 0) {
        fprintf(stderr,"Failed to open codec");
        Free();
        return;
    }

    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofctx->pb, VIDEO_TMP_FILE, AVIO_FLAG_WRITE)) < 0) {
            fprintf(stderr,"Failed to open file");
            Free();
            return;
        }
    }

    if ((err = avformat_write_header(ofctx, nullptr)) < 0) {
        fprintf(stderr,"Failed to write header");
        Free();
        return;
    }

    av_dump_format(ofctx, 0, VIDEO_TMP_FILE, 1);
}

void VideoCapture::AddFrame(QImage image) {
//void VideoCapture::AddFrame(uint8_t *data) {
    int err;
    if (!videoFrame) {

        videoFrame = av_frame_alloc();
        videoFrame->format = AV_PIX_FMT_YUV420P;
        videoFrame->width = cctx->width;
        videoFrame->height = cctx->height;

        if ((err = av_frame_get_buffer(videoFrame, 32)) < 0) {
            fprintf(stderr,"Failed to allocate picture");
            return;
        }
    }

    if (!swsCtx) {
        swsCtx = sws_getContext(image.width(), image.height(), AV_PIX_FMT_RGBA, cctx->width, cctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
    }

    //int inLinesize[1] = { 4 * cctx->width };

    // From RGB to YUV
    int srcstride[1];
    srcstride[0] = image.width()*4;
    const uchar* planes[1];
    planes[0] = image.bits();
    sws_scale(swsCtx, planes, srcstride, 0, image.height(), videoFrame->data, videoFrame->linesize);

    //sws_scale(swsCtx, &data, inLinesize, 0, cctx->height, videoFrame->data, videoFrame->linesize);

    videoFrame->pts = frameCounter++;

    if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
        fprintf(stderr,"Failed to send frame");
        return;
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

void VideoCapture::Finish() {
    //DELAYED FRAMES
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    for (;;) {
        avcodec_send_frame(cctx, nullptr);
        if (avcodec_receive_packet(cctx, &pkt) == 0) {
            av_interleaved_write_frame(ofctx, &pkt);
            av_packet_unref(&pkt);
        }
        else {
            break;
        }
    }

    av_write_trailer(ofctx);
    if (!(oformat->flags & AVFMT_NOFILE)) {
        int err = avio_close(ofctx->pb);
        if (err < 0) {
            fprintf(stderr,"Failed to close file");
        }
    }

    Free();

    Remux();
}

void VideoCapture::Free() {
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
}

void VideoCapture::Remux() {
    AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
    int err;

    if ((err = avformat_open_input(&ifmt_ctx, VIDEO_TMP_FILE, nullptr, nullptr)) < 0) {
        fprintf(stderr,"Failed to open input file for remuxing");
        goto end;
    }
    if ((err = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
        fprintf(stderr,"Failed to retrieve input stream information");
        goto end;
    }
    if ((err = avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, FINAL_FILE_NAME))) {
        fprintf(stderr,"Failed to allocate output context");
        goto end;
    }

    AVStream *inVideoStream = ifmt_ctx->streams[0];
    AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, nullptr);
    if (!outVideoStream) {
        fprintf(stderr,"Failed to allocate output video stream");
        goto end;
    }
    outVideoStream->time_base = { 1, fps };
    avcodec_parameters_copy(outVideoStream->codecpar, inVideoStream->codecpar);
    outVideoStream->codecpar->codec_tag = 0;

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofmt_ctx->pb, FINAL_FILE_NAME, AVIO_FLAG_WRITE)) < 0) {
            fprintf(stderr,"Failed to open output file");
            goto end;
        }
    }

    if ((err = avformat_write_header(ofmt_ctx, nullptr)) < 0) {
        fprintf(stderr,"Failed to write header to output file");
        goto end;
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
            fprintf(stderr,"Failed to mux packet");
            av_packet_unref(&videoPkt);
            break;
        }
        av_packet_unref(&videoPkt);
    }

    av_write_trailer(ofmt_ctx);

end:
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
