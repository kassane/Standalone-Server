const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const ssl = b.option(bool, "Ssl", "Enable openssl support [default: false]") orelse false;
    const tests = b.option(bool, "Tests", "Building testing [default: false]") orelse false;

    const libasio_dep = b.dependency("asio", .{
        .target = target,
        .optimize = optimize,
    });
    const libasio = libasio_dep.artifact("asio");

    if (ssl)
        buildExe(b, .{
            .lib = libasio,
            .path = "examples/https_examples.cpp",
            .ssl = ssl,
            .fuzzer = tests,
        })
    else
        buildExe(b, .{
            .lib = libasio,
            .path = "examples/http_examples.cpp",
            .ssl = ssl,
            .fuzzer = tests,
        });

    if (tests) {
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/fuzzers/http_header_field_value_semicolon_separated_attributes_parse.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/fuzzers/http_header_parse.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/fuzzers/query_string_parse.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/fuzzers/percent_decode.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/parse_test.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        buildExe(b, .{
            .lib = libasio,
            .path = "tests/status_code_test.cpp",
            .ssl = ssl,
            .fuzzer = tests,
        });
        // buildExe(b, .{
        //     .lib = libasio,
        //     .path = "tests/io_test.cpp",
        //     .ssl = ssl,
        //     .fuzzer = tests,
        // });
        if (ssl) buildExe(b, .{
            .lib = libasio,
            .path = "tests/crypto_test.cpp",
            .ssl = ssl,
            .fuzzer = tests,
        });
    }
}

fn buildExe(b: *std.Build, info: BuildInfo) void {
    const exe = b.addExecutable(.{
        .name = info.filename(),
        .optimize = info.lib.optimize,
        .target = info.lib.target,
    });
    exe.installLibraryHeaders(info.lib);
    exe.addIncludePath("include");
    exe.addCSourceFile(info.path, switch (info.fuzzer) {
        true => &.{
            "-Wall",
            "-Wextra",
            "-std=c++17",
            "-Wno-thread-safety",
            // "-fsanitize=fuzzer",
        },
        else => &.{
            "-Wall",
            "-Wextra",
            "-std=c++17",
        },
    });
    exe.defineCMacro("ASIO_STANDALONE", null);
    if (info.fuzzer) {
        exe.defineCMacro("LSAN_OPTIONS", "detect_leaks=0");
        exe.addIncludePath("tests");
    }
    if (info.ssl) {
        exe.linkSystemLibrary("crypto");
        exe.linkSystemLibrary("ssl");
    }
    if (info.lib.target.isWindows()) {
        exe.linkSystemLibrary("ws2_32");
        exe.linkSystemLibrary("mswsock");
    }
    exe.linkLibrary(info.lib);
    exe.linkLibCpp();

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step(
        b.fmt("{s}", .{info.filename()}),
        b.fmt("Run the {s} test", .{info.filename()}),
    );
    run_step.dependOn(&run_cmd.step);
}

const BuildInfo = struct {
    lib: *std.Build.CompileStep,
    path: []const u8,
    ssl: bool,
    fuzzer: bool,

    fn filename(self: BuildInfo) []const u8 {
        var split = std.mem.split(u8, std.fs.path.basename(self.path), ".");
        return split.first();
    }
};
