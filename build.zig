const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const exe = b.addExecutable(.{ .name = "probe" });
    exe.addCSourceFiles(&.{"probe.c"}, &.{});

    // FFMPEG
    exe.addIncludePath(.{ .path = "/opt/homebrew/Cellar/ffmpeg/6.0_1/include" });
    exe.addLibraryPath(.{ .path = "/opt/homebrew/Cellar/ffmpeg/6.0_1/lib" });

    exe.linkSystemLibraryName("avutil");
    exe.linkSystemLibraryName("avformat");
    exe.linkSystemLibraryName("avcodec");
    exe.linkSystemLibraryName("swscale");

    // SDL2
    exe.addIncludePath(.{ .path = "/opt/homebrew/Cellar/sdl2/2.28.3/include" });
    exe.addLibraryPath(.{ .path = "/opt/homebrew/Cellar/sdl2/2.28.3/lib" });
    exe.linkSystemLibraryName("sdl2");

    exe.linkLibC();
    b.installArtifact(exe);
}
