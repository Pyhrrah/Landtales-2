#include "./../../include/utils/video.h"
#include <stdio.h>


/*

Fichier contenant les fonctions utilitaires pour la vidéo et l'audio

Les fonctions utilisent la bibliothèque FFmpeg pour la lecture de vidéos et la bibliothèque SDL2 pour l'affichage des images et des vidéos.

*/

Mix_Music *currentMusic = NULL;

// Initialisation de l'audio
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
        printf("Erreur lors de l'initialisation de SDL_mixer: %s\n", Mix_GetError());
    }

    return 0;
}

// Nettoyage de l'audio
void cleanup_audio() {
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = NULL;
    }
    Mix_CloseAudio();
    Mix_Quit();
}

// Libération de la mémoire de la musique
void free_music() {
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = NULL;
    }
}

// Lecture de la musique
void play_audio(const char *filename) {
    // Si une musique est déjà en cours, on ne joue pas la nouvelle musique, on garde l'ancienne
    if (Mix_PlayingMusic()) {
        return;  // Rien à faire, la musique est déjà en cours
    }

    // Sinon, on joue la musique
    free_music();  // Libère l'ancienne musique si nécessaire

    currentMusic = Mix_LoadMUS(filename);
    if (!currentMusic) {
        fprintf(stderr, "Erreur: Impossible de charger la musique: %s\n", Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(currentMusic, 1) == -1) {  // Joue une seule fois
        fprintf(stderr, "Erreur: Impossible de jouer la musique: %s\n", Mix_GetError());
        free_music();
    }
}

// Lecture d'un son
void playSong(char *sound) {
    // Si une musique est déjà en cours de lecture, on joue le son supplémentaire
    if (Mix_PlayingMusic()) {
        Mix_Chunk *newSound = Mix_LoadWAV(sound);
        if (newSound == NULL) {
            fprintf(stderr, "Erreur: Impossible de charger le son : %s\n", Mix_GetError());
            return;
        }

        // Joue le son supplémentaire sur un canal libre (par exemple, canal 1)
        if (Mix_PlayChannel(1, newSound, 0) == -1) {
            fprintf(stderr, "Erreur: Impossible de jouer le son : %s\n", Mix_GetError());
        }
        Mix_FreeChunk(newSound);
    } else {
        // Si aucune musique n'est en cours, on la lance en fond et on joue le son supplémentaire
        play_audio(sound);
    }
}

// Vérifie si la musique est terminée et libère la mémoire
void check_and_free_music() {
    if (!Mix_PlayingMusic() && currentMusic) {
        free_music();
    }
}



/*
Pour résumer grossièrement : 
- La vidéo est une suite d'images qui sont affichées à une certaine vitesse pour donner l'illusion du mouvement
- Les images sont stockées dans un fichier vidéo
- Pour lire une vidéo, on doit lire les images du fichier vidéo et les afficher à une certaine vitesse


*/

// Initialisation de la vidéo
void display_video_frame(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_Rect *rect,
                         SDL_Texture *texture, SDL_Renderer *renderer, double fpsrendering) {
    if (avcodec_send_packet(ctx, pkt) < 0) return;
    if (avcodec_receive_frame(ctx, frame) < 0) return;

    // SDL_UpdateYUVTexture sert à mettre à jour la texture avec les données YUV de la frame
    // YUV signifie Y (luminance), U (chrominance bleue) et V (chrominance rouge), il correspond à la manière dont les couleurs sont stockées dans la vidéo
    SDL_UpdateYUVTexture(texture, rect,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_RenderPresent(renderer);
    // On attend le temps nécessaire pour afficher la frame suivante (pour que la vidéo soit lue à la bonne vitesse)
    SDL_Delay((Uint32)(fpsrendering * 1000));
}

// Lecture de la vidéo
void play_video(const char *filename, SDL_Renderer *renderer) {
    // Initialisation de la bibliothèque FFmpeg
    // AVFormatContext est une structure qui contient les informations sur le format du fichier : codecs, résolution, etc.
    AVFormatContext *format_ctx = NULL;
    if (avformat_open_input(&format_ctx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    // Récupération des informations sur le flux
    // avformat_find_stream_info permet de récupérer les informations sur le flux vidéo
    // format_ctx contient les informations sur le format du fichier
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible de récupérer les infos du flux\n");
        // avformat_close_input permet de fermer le fichier
        avformat_close_input(&format_ctx);
        return;
    }

    int video_stream_index = -1;
    // On cherche le flux vidéo dans le fichier
    for (unsigned int i = 0; i < (unsigned int)format_ctx->nb_streams; i++) {
        // Si le flux est de type vidéo, on récupère son index, AVMEDIA_TYPE_VIDEO est une constante qui indique que le flux est de type vidéo
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    // Si aucun flux vidéo n'est trouvé, on arrête
    if (video_stream_index == -1) {
        fprintf(stderr, "Erreur: aucun flux vidéo trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    // AVCodecParameters contient les paramètres du codec
    AVCodecParameters *codec_params = format_ctx->streams[video_stream_index]->codecpar;
    // On cherche le codec correspondant aux paramètres, AVCodec est une structure qui contient les informations sur le codec
    // avcodec_find_decoder permet de trouver le codec correspondant aux paramètres
    const AVCodec *codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        fprintf(stderr, "Erreur: codec non trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    // AVCodecContext contient les informations sur le codec
    // avcodec_alloc_context3 permet d'allouer un contexte de codec
    // avcodec_parameters_to_context permet de copier les paramètres du codec dans le contexte du codec
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_params);

    // avcodec_open2 permet d'ouvrir le codec : il charge les informations sur le codec dans le contexte du codec
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le codec\n");
        // avcodec_free_context permet de libérer la mémoire du contexte du codec
        // avformat_close_input permet de fermer le fichier
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return;
    }

    // On créé une texture pour afficher la vidéo, SDL_PIXELFORMAT_IYUV est un format de pixel pour les vidéos YUV, SDL_TEXTUREACCESS_STREAMING permet de mettre à jour la texture
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                             codec_params->width, codec_params->height);

    SDL_Rect rect = {0, 0, 672, 544};

    // AVPacket est une structure qui contient les données du paquet
    // AVFrame est une structure qui contient les données de la frame

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    // On récupère le nombre d'images par seconde de la vidéo
    // format_ctx->streams[video_stream_index]->avg_frame_rate contient le nombre d'images par seconde

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

        // On lit les paquets de la vidéo
        if (av_read_frame(format_ctx, packet) >= 0) {
            // Si le paquet est de type vidéo, on affiche la frame
            if (packet->stream_index == video_stream_index) {
                // On affiche la frame
                display_video_frame(codec_ctx, packet, frame, &rect, texture, renderer, fpsrendering);
            }
            // On libère la mémoire du paquet
            av_packet_unref(packet);
        } else {
            break;
        }
    }

    SDL_DestroyTexture(texture);
    // On libère la mémoire
    av_frame_free(&frame);
    // On libère la mémoire
    av_packet_free(&packet);
    // On libère la mémoire
    avcodec_free_context(&codec_ctx);
    // On ferme le fichier
    avformat_close_input(&format_ctx);
}