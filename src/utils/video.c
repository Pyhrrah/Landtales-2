#include "./../../include/utils/video.h"

int init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erreur: SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0) {
        fprintf(stderr, "Erreur: Impossible d'initialiser SDL_mixer: %s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur: Mix_OpenAudio failed: %s\n", Mix_GetError());
        return -1;
    }

    return 0;
}

void cleanup_audio() {
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void play_audio(const char *filename) {
    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        fprintf(stderr, "Erreur: Impossible de charger la musique: %s\n", Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        fprintf(stderr, "Erreur: Impossible de jouer la musique: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        return;
    }
}


void display_video_frame(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_Rect *rect,
                         SDL_Texture *texture, SDL_Renderer *renderer, double fpsrendering) {
    if (avcodec_send_packet(ctx, pkt) < 0) return;
    if (avcodec_receive_frame(ctx, frame) < 0) return;

    SDL_UpdateYUVTexture(texture, rect,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_RenderPresent(renderer);
    SDL_Delay((Uint32)(fpsrendering * 1000));
}

void play_video(const char *filename, SDL_Renderer *renderer) {

    AVFormatContext *format_ctx = NULL;
    if (avformat_open_input(&format_ctx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible de récupérer les infos du flux\n");
        avformat_close_input(&format_ctx);
        return;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < (unsigned int)format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "Erreur: aucun flux vidéo trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    AVCodecParameters *codec_params = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        fprintf(stderr, "Erreur: codec non trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_params);

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le codec\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return;
    }

    
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                             codec_params->width, codec_params->height);

    SDL_Rect rect = {0, 0, codec_params->width, codec_params->height};
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    int fps = format_ctx->streams[video_stream_index]->avg_frame_rate.num /
              format_ctx->streams[video_stream_index]->avg_frame_rate.den;
    double fpsrendering = 1.0 / fps;

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        if (av_read_frame(format_ctx, packet) >= 0) {
            if (packet->stream_index == video_stream_index) {
                display_video_frame(codec_ctx, packet, frame, &rect, texture, renderer, fpsrendering);
            }
            av_packet_unref(packet);
        } else {
            break;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
}