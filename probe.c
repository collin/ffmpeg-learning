#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

AVFormatContext *pFormatContext = NULL;

int main(int argc, char *argv[])
{

  // exit with failure unless arg list is length 1
  if (argc < 2)
  {
    printf("Usage: %s inputpath\n", argv[0]);
    return -1;
  }
  printf("Probing %s\n\n", argv[1]);

  AVFormatContext *pFormatContext = avformat_alloc_context();

  avformat_open_input(&pFormatContext, argv[1], NULL, NULL);
  printf("Timebase: %d/%d\n", pFormatContext->streams[0]->time_base.num, pFormatContext->streams[0]->time_base.den);
  printf("Format: %s\n", pFormatContext->iformat->long_name);
  printf("\n");

  // for loop iteratin through streams
  for (int i = 0; i < pFormatContext->nb_streams; i++)
  {
    AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
    printf("Stream %d: ", i);
    if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      printf("Video\n");
      printf(" ┬ resolution %dx%d\n", pLocalCodecParameters->width, pLocalCodecParameters->height);
      printf(" ├ timebase %d/%d\n", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
      printf(" └ duration %llds\n", pFormatContext->streams[i]->duration / pFormatContext->streams[i]->time_base.den);
    }
    else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      printf("Audio\n");
      printf(" ┬ %d channels\n", pLocalCodecParameters->channels);
      printf(" ├ sample rate %d\n", pLocalCodecParameters->sample_rate);
      printf(" ├ timebase %d/%d\n", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
      printf(" └ duration %llds\n", pFormatContext->streams[i]->duration / pFormatContext->streams[i]->time_base.den);
    }
    printf("\n");
  }

  return 0; // Return success
}
