#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

int main(int argc, char *argv[])
{
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
      printf(" ┬    channels %d\n", format_ctx->streams[i]->codecpar->channels);
      printf(" ├ sample rate %d\n", format_ctx->streams[i]->codecpar->sample_rate);
      printf(" ├    timebase %d/%d\n", format_ctx->streams[i]->time_base.num, format_ctx->streams[i]->time_base.den);
      printf(" ├       codec %s\n", codec_name);
      printf(" └    duration %llds\n", format_ctx->streams[i]->duration / format_ctx->streams[i]->time_base.den);
    }
    printf("\n");
  }

  // any of this could fail
  v_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  v_stream = format_ctx->streams[v_stream_index];
  v_codec = avcodec_find_decoder(v_stream->codecpar->codec_id);
  v_codec_ctx = avcodec_alloc_context3(v_codec);
  // avcodec_parameters_to_context(*v_codec_ctx, v_stream->codecpar);
  avcodec_open2(v_codec_ctx, v_codec, NULL);

  a_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  a_stream = format_ctx->streams[a_stream_index];
  a_codec = avcodec_find_decoder(a_stream->codecpar->codec_id);
  a_codec_ctx = avcodec_alloc_context3(a_codec);
  // avcodec_parameters_to_context(*a_codec_ctx, a_stream->codecpar);
  avcodec_open2(a_codec_ctx, a_codec, NULL);

  int frameCount = 0;
  while (av_read_frame(format_ctx, packet) >= 0)
  {
    frameCount++;
    // printf("Frame %d\n", frameCount);
    // printf("%d %d", a_stream_index, packet->stream_index);
    if (packet->stream_index == v_stream_index)
    {
      printf("📺 (%d) Video\n", frameCount);
      printf(" ┬ pts %lld\n", packet->pts);
      printf(" ├ dts %lld\n", packet->dts);
      printf(" ├ duration %lld\n", packet->duration);
      printf(" └ pos %lld\n", packet->pos);
    }
    else if (packet->stream_index == a_stream_index)
    {
      printf("🔊 (%d) Audio\n", frameCount);
      printf(" ┬ pts %lld\n", packet->pts);
      printf(" ├ dts %lld\n", packet->dts);
      printf(" ├ duration %lld\n", packet->duration);
      printf(" └ pos %lld\n", packet->pos);
    }

    printf("\n");
    av_packet_unref(packet);
  }

  return 0; // Return success
}
