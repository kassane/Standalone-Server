const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const libasio_dep = b.dependency("asio", .{
        .target = target,
        .optimize = optimize,
    });
    const exe = b.addExecutable(.{
        .name = "standalone-server",
        .target = target,
        .optimize = optimize,
    });
    exe.addCSourceFile("examples/http_hello.cpp", &.{
        "-Wall",
        "-Wextra",
        "-std=c++17",
    });
    exe.defineCMacro("ASIO_STANDALONE", null);
    exe.addIncludePath("include");
    const libasio = libasio_dep.artifact("asio");
    exe.linkLibrary(libasio);
    if (target.isWindows())
        exe.linkSystemLibrary("ws2_32");
    exe.linkLibCpp();
    exe.installLibraryHeaders(libasio);
    exe.linkLibCpp();

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
