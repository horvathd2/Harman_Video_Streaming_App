#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavdevice/avdevice.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifdef _WIN32
    static const char* input_format = "dshow";
    static const char* webcam_url = "video=FHD Camera"; 
#elif __linux__
    static const char* input_format = "v4l2"; 
    static const char* webcam_url = "/dev/video0"; 
#else
    #error "Unable to compile. Unsupported platform"
#endif

#define WIDTH 1280  //1920
#define HEIGHT 720  //1080

void flip_frame(uint8_t *data, int width, int height, int linesize){
    int x, y;
    for(y = 0; y < height; y++){
        uint8_t *row1 = data + y * linesize;
        uint8_t *row2 = data + (height - y - 1) * linesize;
        for(x = 0; x < width; x++){
            uint8_t tmp = row1[x];
            row1[x] = row2[x];
            row2[x] = tmp;
        }
    }
}

void change_frame_colours(uint8_t *data, int width, int height, int linesize){
    int x, y;
    for(y = 0; y < height; y++){
        uint8_t *row = data + y * linesize;
        for(x = 0; x< width; x++){
            row[x*3+0] = 255 - row[x*3+0]; //RED
            row[x*3+1] = 255 - row[x*3+1]; //GREEN
            row[x*3+2] = 255 - row[x*3+2]; //BLUE
        }
    }
}

int main(){
    avdevice_register_all();
    
    #ifdef __linux__
        avcodec_register_all();
    #endif

    AVFormatContext *fmt_ctx = NULL;
    const AVInputFormat *input_fmt = av_find_input_format(input_format);

    if(!input_fmt){
        printf("Invalid input format...\n");
        return -1;
    }

    if(avformat_open_input(&fmt_ctx, webcam_url, input_fmt, NULL) != 0){
        printf("Unable to open webcam...\n");
        return -1;
    }

    if(avformat_find_stream_info(fmt_ctx, NULL) < 0){
        printf("Unable to find video stream information...\n");
        return -1;
    }

    int video_stream_index = -1;
    for(int i = 0; i < fmt_ctx->nb_streams; i++){
        if(fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }

    if(video_stream_index == -1){
        printf("No video stream provided...\n");
        return -1;
    }

    //OPEN DECODER

    AVCodecParameters *codecparam = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec *decoder = avcodec_find_decoder(codecparam->codec_id);
    AVCodecContext *dec_ctx = avcodec_alloc_context3(decoder);
    
    avcodec_parameters_to_context(dec_ctx, codecparam);

    if (!decoder) {
        printf("Decoder not required for raw video stream...\n");
        decoder = avcodec_find_decoder(AV_CODEC_ID_RAWVIDEO);
    }else if(avcodec_open2(dec_ctx, decoder, NULL) < 0){
        printf("Unable to fetch a decoder...\n");
        return -1;
    }

    AVFrame *frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, WIDTH, HEIGHT, 1);
    uint8_t *buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer,
                         AV_PIX_FMT_RGB24, WIDTH, HEIGHT, 1);

    struct SwsContext *sws_ctx = sws_getContext(
        dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    //H264 ENCODER

    const AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext *enc_ctx = avcodec_alloc_context3(encoder);
    enc_ctx->width = WIDTH;
    enc_ctx->height = HEIGHT;
    enc_ctx->time_base = (AVRational){1,10};
    enc_ctx->framerate = (AVRational){10,1};
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if(avcodec_open2(enc_ctx,encoder,NULL) < 0){
        printf("Unable to fetch a H264 encoder...\n");
        return -1;
    }

    AVFrame *yuv_frame = av_frame_alloc();
    yuv_frame->format = AV_PIX_FMT_YUV420P;
    yuv_frame->width = WIDTH;
    yuv_frame->height = HEIGHT;
    av_frame_get_buffer(yuv_frame,32);

    struct SwsContext *rgb2yuv_ctx = sws_getContext(
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,
        WIDTH, HEIGHT, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    //H264 OUTPUT FILE

    FILE *outfile = fopen("output.h264", "wb");
    if (!outfile) {
        printf("Could not open output file...\n");
        return -1;
    }

    //SDL INIT AND DISPLAY

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)){
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Harman Video Stream App",
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_RGB24,
                SDL_TEXTUREACCESS_STREAMING,
                WIDTH, HEIGHT);

    //MAIN LOOP FOR READING FRAMES AND DISPLAYING THEM ON THE WINDOW

    AVPacket *enc_packet = av_packet_alloc();
    AVPacket *packet = av_packet_alloc();

    while(1){
        if(av_read_frame(fmt_ctx, packet) < 0) break;

        if(packet->stream_index == video_stream_index){
            if(avcodec_send_packet(dec_ctx, packet) == 0){
                while(avcodec_receive_frame(dec_ctx, frame) == 0){
                    sws_scale(sws_ctx, (const uint8_t* const*) frame->data,
                            frame->linesize, 0, dec_ctx->height,
                            rgb_frame->data, rgb_frame->linesize);
                    
                    flip_frame(rgb_frame->data[0], WIDTH, HEIGHT, rgb_frame->linesize[0]);
                    
                    change_frame_colours(rgb_frame->data[0], WIDTH, HEIGHT, rgb_frame->linesize[0]);

                    //UPDATE TEXTURE AND RENDER FRAMES

                    SDL_UpdateTexture(texture, NULL, rgb_frame->data[0], rgb_frame->linesize[0]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_Event e;
                    while(SDL_PollEvent(&e)){
                        if(e.type == SDL_QUIT){
                            goto exit_label;
                        }
                    }

                    //H264 ENCODING
                    sws_scale(rgb2yuv_ctx, (const uint8_t* const*) rgb_frame->data,
                            rgb_frame->linesize, 0, HEIGHT,
                            yuv_frame->data, yuv_frame->linesize);

                    yuv_frame->pts++;

                    if (avcodec_send_frame(enc_ctx, yuv_frame) == 0) {
                        //av_init_packet(enc_packet);
                        enc_packet = av_packet_alloc();
                        enc_packet->data = NULL;
                        enc_packet->size = 0;

                        while (avcodec_receive_packet(enc_ctx, enc_packet) == 0) {
                            fwrite(enc_packet->data, 1, enc_packet->size, outfile);
                            av_packet_unref(enc_packet);
                        }
                    }
                }
            }
        }

        av_packet_unref(packet);
    }

    //CLEANUP

    exit_label:
    av_packet_free(&enc_packet);

    fclose(outfile);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    sws_freeContext(rgb2yuv_ctx);
    av_frame_free(&yuv_frame);
    avcodec_free_context(&enc_ctx);

    sws_freeContext(sws_ctx);
    av_free(buffer);
    av_frame_free(&rgb_frame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    
    return 0;
}