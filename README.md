TODO: figure out how to link c libraries in a more portable way

Currently, I think the only way to make this code work is to

```
brew install zig
brew install ffmpeg
```

Currently hardcoded in build.zig to find ffmpeg at:

```
/opt/homebrew/Cellar/ffmpeg/6.0_1
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
