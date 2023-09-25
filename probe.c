#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
  {
    //   fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);

  // SwsContext *sws_ctx = NULL;

  AVFormatContext *format_ctx = avformat_alloc_context();

  AVCodecContext *v_codec_ctx = NULL;
  AVCodec *v_codec = NULL;
  AVStream *v_stream = NULL;
  int v_stream_index = -1;

  AVCodecContext *a_codec_ctx = NULL;
  AVCodec *a_codec = NULL;
  AVStream *a_stream = NULL;
  int a_stream_index = -1;

  AVPacket *packet = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  // exit with failure unless arg list is length 1
  if (argc < 2)
  {
    printf("Usage: %s inputpath\n", argv[0]);
    return -1;
  }
  printf("Probing %s\n\n", argv[1]);

  avformat_open_input(&format_ctx, argv[1], NULL, NULL);
  avformat_find_stream_info(format_ctx, NULL);

  printf("Timebase: %d/%d\n", format_ctx->streams[0]->time_base.num, format_ctx->streams[0]->time_base.den);
  printf("Format: %s\n", format_ctx->iformat->long_name);
  printf("\n");

  // for loop iteratin through streams
  for (int i = 0; i < format_ctx->nb_streams; i++)
  {
    char *codec_name = avcodec_get_name(format_ctx->streams[i]->codecpar->codec_id);
    printf("Stream %d: ", i);
    if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      printf("Video\n");
      printf(" ┬ resolution %dx%d\n", format_ctx->streams[i]->codecpar->width, format_ctx->streams[i]->codecpar->height);
      printf(" ├   timebase %d/%d\n", format_ctx->streams[i]->time_base.num, format_ctx->streams[i]->time_base.den);
      printf(" ├      codec %s\n", codec_name);
      printf(" └   duration %llds\n", format_ctx->streams[i]->duration / format_ctx->streams[i]->time_base.den);
    }
    else if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      printf("Audio\n");
      // printf(" ┬    channels %d\n", format_ctx->streams[i]->codecpar->channels);
      printf(" ├ sample rate %d\n", format_ctx->streams[i]->codecpar->sample_rate);
      printf(" ├    timebase %d/%d\n", format_ctx->streams[i]->time_base.num, format_ctx->streams[i]->time_base.den);
      printf(" ├       codec %s\n", codec_name);
      printf(" └    duration %llds\n", format_ctx->streams[i]->duration / format_ctx->streams[i]->time_base.den);
    }
    printf("\n");
  }

  // any of this could fail
  v_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  printf("Video stream index: %d\n", v_stream_index);
  if (v_stream_index < 0)
  {
    printf("Error finding video stream\n");
    return -1;
  }
  v_stream = format_ctx->streams[v_stream_index];
  if (v_stream == NULL)
  {
    printf("Error finding video stream\n");
    return -1;
  }
  v_codec = avcodec_find_decoder(v_stream->codecpar->codec_id);
  if (v_codec == NULL)
  {
    printf("Error finding video codec\n");
    return -1;
  }
  v_codec_ctx = avcodec_alloc_context3(v_codec);
  if (v_codec_ctx == NULL)
  {
    printf("Error allocating video codec context\n");
    return -1;
  };
  if (avcodec_parameters_to_context(v_codec_ctx, v_stream->codecpar) < 0)
  {
    printf("Error copying video codec parameters to context\n");
    return -1;
  }
  if (avcodec_open2(v_codec_ctx, v_codec, NULL) < 0)
  {
    printf("Error opening video codec\n");
    return -1;
  }
  printf("\n");
  printf("v_codec width %d\n", v_codec_ctx->width);
  printf("v_codec height %d\n", v_codec_ctx->height);
  printf("\n");

  // Create a window with the specified position, dimensions, and flags.
  SDL_Window *screen = SDL_CreateWindow( // [2]
      "SDL Video Player",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      v_codec_ctx->width / 2,
      v_codec_ctx->height / 2,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

  if (!screen)
  {
    // could not set video mode
    printf("SDL: could not set video mode - exiting.\n");

    // exit with Error
    return -1;
  }

  SDL_GL_SetSwapInterval(1);

  // A structure that contains a rendering state.
  SDL_Renderer *renderer = NULL;

  // Use this function to create a 2D rendering context for a window.
  renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE); // [3]

  // A structure that contains an efficient, driver-specific representation
  // of pixel data.
  SDL_Texture *texture = NULL;

  // Use this function to create a texture for a rendering context.
  texture = SDL_CreateTexture( // [4]
      renderer,
      SDL_PIXELFORMAT_YV12,
      SDL_TEXTUREACCESS_STREAMING,
      v_codec_ctx->width,
      v_codec_ctx->height);

  // a_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  // a_stream = format_ctx->streams[a_stream_index];
  // a_codec = avcodec_find_decoder(a_stream->codecpar->codec_id);
  // a_codec_ctx = avcodec_alloc_context3(a_codec);
  // // avcodec_parameters_to_context(*a_codec_ctx, a_stream->codecpar);
  // if (avcodec_parameters_to_context(a_codec_ctx, a_stream->codecpar) < 0)
  // {
  //   printf("Error copying video codec parameters to context\n");
  //   return -1;
  // }
  // avcodec_open2(a_codec_ctx, a_codec, NULL);

  // struct SwsContext *sws_ctx = sws_getContext(
  //     v_codec_ctx->width,
  //     v_codec_ctx->height,
  //     v_codec_ctx->pix_fmt,
  //     v_codec_ctx->width,
  //     v_codec_ctx->height,
  //     AV_PIX_FMT_YUV420P,
  //     SWS_BILINEAR,
  //     NULL,
  //     NULL,
  //     NULL);

  // int numBytes;
  // uint8_t *buffer = NULL;

  // numBytes = av_image_get_buffer_size(
  //     AV_PIX_FMT_YUV420P,
  //     v_codec_ctx->width,
  //     v_codec_ctx->height,
  //     32);
  // buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

  // av_image_fill_arrays(
  //     frame->data,
  //     frame->linesize,
  //     buffer,
  //     AV_PIX_FMT_YUV420P,
  //     v_codec_ctx->width,
  //     v_codec_ctx->height,
  //     32);

  SDL_Event event;
  int frameCount = 0;
  while (av_read_frame(format_ctx, packet) >= 0)
  {
    frameCount++;
    // printf("Frame %d\n", frameCount);
    // printf("%d %d", a_stream_index, packet->stream_index);
    if (packet->stream_index == v_stream_index)
    {
      // printf("📺 (%d) Packet\n", frameCount);
      // printf(" ┬      pts %lld\n", packet->pts);
      // printf(" ├      dts %lld\n", packet->dts);
      // printf(" ├ duration %lld\n", packet->duration);
      // printf(" ├     size %d\n", packet->size);
      // printf(" └      pos %lld\n", packet->pos);

      int response = avcodec_send_packet(v_codec_ctx, packet);
      if (response < 0)
      {
        printf("Error sending packet for decoding %s.\n", av_err2str(response));
        break;
      }

      while (response >= 0)
      {
        response = avcodec_receive_frame(v_codec_ctx, frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
          break;
        }
        else if (response < 0)
        {
          printf("Error during decoding %s\n", av_err2str(response));
          return response;
        }
        if (response >= 0)
        {
          if (frame->pict_type == AV_PICTURE_TYPE_I)
          {

            printf(
                "Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS %d]\n",
                v_codec_ctx->frame_number,
                av_get_picture_type_char(frame->pict_type),
                frame->pkt_size,
                frame->format,
                frame->pts,
                frame->key_frame,
                frame->coded_picture_number);
          }
          // sws_scale(
          //     sws_ctx,
          //     (uint8_t const *const *)frame->data,
          //     frame->linesize,
          //     0,
          //     v_codec_ctx->height,
          //     frame->data,
          //     frame->linesize);

          SDL_Delay(av_q2d(v_stream->time_base) * frame->duration * 1000);

          SDL_UpdateYUVTexture(
              texture,
              NULL,
              frame->data[0],
              frame->linesize[0],
              frame->data[1],
              frame->linesize[1],
              frame->data[2],
              frame->linesize[2]);

          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, NULL);
          SDL_RenderPresent(renderer);
        }
      }
    }
    else if (packet->stream_index == a_stream_index)
    {
      // printf("🔊 (%d) Packet\n", frameCount);
      // printf(" ┬      pts %lld\n", packet->pts);
      // printf(" ├      dts %lld\n", packet->dts);
      // printf(" ├ duration %lld\n", packet->duration);
      // printf(" └      pos %lld\n", packet->pos);
    }

    av_packet_unref(packet);
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
    {
      SDL_Quit();
      exit(0);
    }
    break;

    default:
    {
      // nothing to do
    }
    break;
    }
  }

  return 0; // Return success
}
