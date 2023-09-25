TODO: figure out how to link c libraries in a more portable way

Currently, I think the only way to make this code work is to

```
brew install zig ffmpeg sdl2
```

Currently hardcoded in build.zig to find ffmpeg/sdl2 at:

```
/opt/homebrew/Cellar/ffmpeg/6.0_1
/opt/homebrew/Cellar/sdl2/2.28.3
```

Try it out:

```
zig build && zig-out/bin/probe 10s-bars.frag.mp4
```

Should see something like:

```
Timebase: 1/12800
Format: QuickTime / MOV

Stream 0: Video
 ┬ resolution 192x108
 ├ timebase 1/12800
 └ duration 10s

Stream 1: Audio
 ┬ 1 channels
 ├ sample rate 48000
 ├ timebase 1/48000
 └ duration 10s
```

This will also open an SDL window and draw the video to it.

The video will use the frame duration to delay drawing between frames. However
it is not actually syncing to time, so if the playback gets out of sync due to
constrained resources, it will not attempt to catch up to real time.
