#include "hffplayer.h"

#include "confile.h"
#include "hlog.h"
#include "hstring.h"
#include "hscope.h"
#include "htime.h"

#define DEFAULT_BLOCK_TIMEOUT   10  // s

std::atomic_flag HFFPlayer::s_ffmpeg_init = ATOMIC_FLAG_INIT;

static void list_devices() {
    AVFormatContext* fmt_ctx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);
#ifdef _WIN32
    const char drive[] = "dshow";
#elif defined(__linux__)
    const char drive[] = "v4l2";
#else
    const char drive[] = "avfoundation";
#endif
    AVInputFormat* ifmt = av_find_input_format(drive);
    if (ifmt) {
        avformat_open_input(&fmt_ctx, "video=dummy", ifmt, &options);
    }
    avformat_close_input(&fmt_ctx);
    avformat_free_context(fmt_ctx);
    av_dict_free(&options);
}

// NOTE: avformat_open_input,av_read_frame block
static int interrupt_callback(void* opaque) {
    if (opaque == NULL) return 0;
    HFFPlayer* player = (HFFPlayer*)opaque;
    if (player->quit ||
        time(NULL) - player->block_starttime > player->block_timeout) {
        hlogi("interrupt quit=%d media.src=%s", player->quit, player->media.src.c_str());
        return 1;
    }
    return 0;
}

HFFPlayer::HFFPlayer()
: HVideoPlayer()
, HThread() {
    fmt_opts = NULL;
    codec_opts = NULL;
    fmt_ctx = NULL;
    codec_ctx = NULL;
    packet = NULL;
    frame = NULL;
    sws_ctx = NULL;

    block_starttime = time(NULL);
    block_timeout = DEFAULT_BLOCK_TIMEOUT;
    quit = 0;

    if (!s_ffmpeg_init.test_and_set()) {
        // av_register_all();
        // avcodec_register_all();
        avformat_network_init();
        avdevice_register_all();
        list_devices();
    }
}

HFFPlayer::~HFFPlayer() {
    close();
}

int HFFPlayer::open() {
    std::string ifile;

    AVInputFormat* ifmt = NULL;
    switch (media.type) {
    case MEDIA_TYPE_CAPTURE:
    {
        ifile = "video=";
        ifile += media.src;
#ifdef _WIN32
        const char drive[] = "dshow";
#elif defined(__linux__)
        const char drive[] = "v4l2";
#else
        const char drive[] = "avfoundation";
#endif
        ifmt = av_find_input_format(drive);
        if (ifmt == NULL) {
            hloge("Can not find dshow");
            return -5;
        }
    }
        break;
    case MEDIA_TYPE_FILE:
    case MEDIA_TYPE_NETWORK:
        ifile = media.src;
        break;
    default:
        return -10;
    }

    hlogi("ifile:%s", ifile.c_str());
    int ret = 0;
    fmt_ctx = avformat_alloc_context();
    if (fmt_ctx == NULL) {
        hloge("avformat_alloc_context");
        ret = -10;
        return ret;
    }
    defer (if (ret != 0 && fmt_ctx) {avformat_free_context(fmt_ctx); fmt_ctx = NULL;})

    if (media.type == MEDIA_TYPE_NETWORK) {
        if (strncmp(media.src.c_str(), "rtsp:", 5) == 0) {
            std::string str = g_confile->GetValue("rtsp_transport", "video");
            if (strcmp(str.c_str(), "tcp") == 0 ||
                strcmp(str.c_str(), "udp") == 0) {
                av_dict_set(&fmt_opts, "rtsp_transport", str.c_str(), 0);
            }
        }
        av_dict_set(&fmt_opts, "stimeout", "5000000", 0);   // us
    }
    av_dict_set(&fmt_opts, "buffer_size", "2048000", 0);
    fmt_ctx->interrupt_callback.callback = interrupt_callback;
    fmt_ctx->interrupt_callback.opaque = this;
    block_starttime = time(NULL);
    ret = avformat_open_input(&fmt_ctx, ifile.c_str(), ifmt, &fmt_opts);
    if (ret != 0) {
        hloge("Open input file[%s] failed: %d", ifile.c_str(), ret);
        return ret;
    }
    fmt_ctx->interrupt_callback.callback = NULL;
    defer (if (ret != 0 && fmt_ctx) {avformat_close_input(&fmt_ctx);})

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret != 0) {
        hloge("Can not find stream: %d", ret);
        return ret;
    }
    hlogi("stream_num=%d", fmt_ctx->nb_streams);

    video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    subtitle_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_SUBTITLE, -1, -1, NULL, 0);
    hlogi("video_stream_index=%d", video_stream_index);
    hlogi("audio_stream_index=%d", audio_stream_index);
    hlogi("subtitle_stream_index=%d", subtitle_stream_index);

    if (video_stream_index < 0) {
        hloge("Can not find video stream.");
        ret = -20;
        return ret;
    }

    AVStream* video_stream = fmt_ctx->streams[video_stream_index];
    video_time_base_num = video_stream->time_base.num;
    video_time_base_den = video_stream->time_base.den;
    hlogi("video_stream time_base=%d/%d", video_stream->time_base.num, video_stream->time_base.den);

    AVCodecParameters* codec_param = video_stream->codecpar;
    hlogi("codec_id=%d:%s", codec_param->codec_id, avcodec_get_name(codec_param->codec_id));

    AVCodec* codec = NULL;
    if (decode_mode != SOFTWARE_DECODE) {
try_hardware_decode:
        std::string decoder(avcodec_get_name(codec_param->codec_id));
        if (decode_mode == HARDWARE_DECODE_CUVID) {
            decoder += "_cuvid";
            real_decode_mode = HARDWARE_DECODE_CUVID;
        }
        else if (decode_mode == HARDWARE_DECODE_QSV) {
            decoder += "_qsv";
            real_decode_mode = HARDWARE_DECODE_QSV;
        }
        codec = avcodec_find_decoder_by_name(decoder.c_str());
        if (codec == NULL) {
            hlogi("Can not find decoder %s", decoder.c_str());
            // goto try_software_decode;
        }
        hlogi("decoder=%s", decoder.c_str());
    }

    if (codec == NULL) {
try_software_decode:
        codec = avcodec_find_decoder(codec_param->codec_id);
        if (codec == NULL) {
            hloge("Can not find decoder %s", avcodec_get_name(codec_param->codec_id));
            ret = -30;
            return ret;
        }
        real_decode_mode = SOFTWARE_DECODE;
    }

    hlogi("codec_name: %s=>%s", codec->name, codec->long_name);

    codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL) {
        hloge("avcodec_alloc_context3");
        ret = -40;
        return ret;
    }
    defer (if (ret != 0 && codec_ctx) {avcodec_free_context(&codec_ctx); codec_ctx = NULL;})

    ret = avcodec_parameters_to_context(codec_ctx, codec_param);
    if (ret != 0) {
        hloge("avcodec_parameters_to_context error: %d", ret);
        return ret;
    }

    if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        av_dict_set(&codec_opts, "refcounted_frames", "1", 0);
    }
    ret = avcodec_open2(codec_ctx, codec, &codec_opts);
    if (ret != 0) {
        if (real_decode_mode != SOFTWARE_DECODE) {
            hlogi("Can not open hardwrae codec error: %d, try software codec.", ret);
            goto try_software_decode;
        }
        hloge("Can not open software codec error: %d", ret);
        return ret;
    }
    video_stream->discard = AVDISCARD_DEFAULT;

    int sw, sh, dw, dh;
    sw = codec_ctx->width;
    sh = codec_ctx->height;
    src_pix_fmt = codec_ctx->pix_fmt;
    hlogi("sw=%d sh=%d src_pix_fmt=%d:%s", sw, sh, src_pix_fmt, av_get_pix_fmt_name(src_pix_fmt));
    if (sw <= 0 || sh <= 0 || src_pix_fmt == AV_PIX_FMT_NONE) {
        hloge("Codec parameters wrong!");
        ret = -45;
        return ret;
    }

    dw = sw >> 2 << 2; // align = 4
    dh = sh;
    dst_pix_fmt = AV_PIX_FMT_YUV420P;
    std::string str = g_confile->GetValue("dst_pix_fmt", "video");
    if (!str.empty()) {
        if (strcmp(str.c_str(), "YUV") == 0) {
            dst_pix_fmt = AV_PIX_FMT_YUV420P;
        }
        else if (strcmp(str.c_str(), "RGB") == 0) {
            dst_pix_fmt = AV_PIX_FMT_BGR24;
        }
    }
    hlogi("dw=%d dh=%d dst_pix_fmt=%d:%s", dw, dh, dst_pix_fmt, av_get_pix_fmt_name(dst_pix_fmt));

    sws_ctx = sws_getContext(sw, sh, src_pix_fmt, dw, dh, dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
    if (sws_ctx == NULL) {
        hloge("sws_getContext");
        ret = -50;
        return ret;
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    hframe.w = dw;
    hframe.h = dh;
    // ARGB
    hframe.buf.resize(dw * dh * 4);

    if (dst_pix_fmt == AV_PIX_FMT_YUV420P) {
        hframe.type = PIX_FMT_IYUV;
        hframe.bpp = 12;
        int y_size = dw * dh;
        hframe.buf.len = y_size * 3 / 2;
        data[0] = (uint8_t*)hframe.buf.base;
        data[1] = data[0] + y_size;
        data[2] = data[1] + y_size / 4;
        linesize[0] = dw;
        linesize[1] = linesize[2] = dw / 2;
    }
    else {
        dst_pix_fmt = AV_PIX_FMT_BGR24;
        hframe.type = PIX_FMT_BGR;
        hframe.bpp = 24;
        hframe.buf.len = dw * dh * 3;
        data[0] = (uint8_t*)hframe.buf.base;
        linesize[0] = dw * 3;
    }

    // HVideoPlayer member vars
    if (video_stream->avg_frame_rate.num && video_stream->avg_frame_rate.den) {
        fps = video_stream->avg_frame_rate.num / video_stream->avg_frame_rate.den;
    }
    width = sw;
    height = sh;
    duration = 0;
    start_time = 0;
    eof = 0;
    error = 0;
    if (video_time_base_num && video_time_base_den) {
        if (video_stream->duration > 0) {
            duration = video_stream->duration / (double)video_time_base_den * video_time_base_num * 1000;
        }
        if (video_stream->start_time > 0) {
            start_time = video_stream->start_time / (double)video_time_base_den * video_time_base_num * 1000;
        }
    }
    hlogi("fps=%d duration=%lldms start_time=%lldms", fps, duration, start_time);

    HThread::setSleepPolicy(HThread::SLEEP_UNTIL, 1000 / fps);
    return ret;
}

int HFFPlayer::close() {
    if (fmt_opts) {
        av_dict_free(&fmt_opts);
        fmt_opts = NULL;
    }

    if (codec_opts) {
        av_dict_free(&codec_opts);
        codec_opts = NULL;
    }

    if (codec_ctx) {
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        codec_ctx = NULL;
    }

    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
        fmt_ctx = NULL;
    }

    if (frame) {
        av_frame_unref(frame);
        av_frame_free(&frame);
        frame = NULL;
    }

    if (packet) {
        av_packet_unref(packet);
        av_packet_free(&packet);
        packet = NULL;
    }

    if (sws_ctx) {
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }

    hframe.buf.cleanup();
    return 0;
}

int HFFPlayer::seek(int64_t ms) {
    if (fmt_ctx) {
        clear_frame_cache();
        hlogi("seek=>%lldms", ms);
        return av_seek_frame(fmt_ctx, video_stream_index,
                (start_time+ms)/1000/(double)video_time_base_num*video_time_base_den,
                AVSEEK_FLAG_BACKWARD);
    }
    return 0;
}

bool HFFPlayer::doPrepare() {
    int ret = open();
    if (ret != 0) {
        if (!quit) {
            error = ret;
            event_callback(HPLAYER_OPEN_FAILED);
        }
        return false;
    }
    else {
        event_callback(HPLAYER_OPENED);
    }
    return true;
}

bool HFFPlayer::doFinish() {
    int ret = close();
    event_callback(HPLAYER_CLOSED);
    return ret == 0;
}

void HFFPlayer::doTask() {
    // loop until get a video frame
    while (!quit) {
        av_init_packet(packet);

        fmt_ctx->interrupt_callback.callback = interrupt_callback;
        fmt_ctx->interrupt_callback.opaque = this;
        block_starttime = time(NULL);
        //hlogi("av_read_frame");
        int ret = av_read_frame(fmt_ctx, packet);
        //hlogi("av_read_frame retval=%d", ret);
        fmt_ctx->interrupt_callback.callback = NULL;
        if (ret != 0) {
            hlogi("No frame: %d", ret);
            if (!quit) {
                if (ret == AVERROR_EOF || avio_feof(fmt_ctx->pb)) {
                    eof = 1;
                    event_callback(HPLAYER_EOF);
                }
                else {
                    error = ret;
                    event_callback(HPLAYER_ERROR);
                }
            }
            return;
        }

        // NOTE: if not call av_packet_unref, memory leak.
        defer (av_packet_unref(packet);)

        // hlogi("stream_index=%d data=%p len=%d", packet->stream_index, packet->data, packet->size);
        if (packet->stream_index != video_stream_index) {
            continue;
        }

#if 1
        // hlogi("avcodec_send_packet");
        ret = avcodec_send_packet(codec_ctx, packet);
        if (ret != 0) {
            hloge("avcodec_send_packet error: %d", ret);
            return;
        }
        // hlogi("avcodec_receive_frame");
        ret = avcodec_receive_frame(codec_ctx, frame);
        if (ret != 0) {
            if (ret != -EAGAIN) {
                hloge("avcodec_receive_frame error: %d", ret);
                return;
            }
        }
        else {
            break;
        }
#else
        int got_pic = 0;
        // hlogi("avcodec_decode_video2");
        ret = avcodec_decode_video2(codec_ctx, frame, &got_pic, packet);
        // hlogi("avcodec_decode_video2 retval=%d got_pic=%d", ret, got_pic);
        if (ret < 0) {
            hloge("decoder error: %d", ret);
            return;
        }

        if (got_pic)    break;  // exit loop
#endif
    }

    if (sws_ctx) {
        // hlogi("sws_scale w=%d h=%d data=%p", frame->width, frame->height, frame->data);
        int h = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, data, linesize);
        // hlogi("sws_scale h=%d", h);
        if (h <= 0 || h != frame->height) {
            return;
        }
    }

    if (video_time_base_num && video_time_base_den) {
        hframe.ts = frame->pts / (double)video_time_base_den * video_time_base_num * 1000;
    }
    // hlogi("ts=%lldms", hframe.ts);

    push_frame(&hframe);
}
