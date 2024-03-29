# Standalone-Server

A very simple, fast, multithreaded, platform independent HTTP and HTTPS server and client library implemented using C++17 and Asio standalone. Created to be an easy way to make REST resources available from C++ applications.

## Requires

- [Zig](https://ziglang.org/download) toolchain v0.12.0 or master

## Features

* Asynchronous request handling
* Thread pool if needed
* Platform independent
* HTTP/1.1 supported, including persistent connections
* HTTPS supported
* Chunked transfer encoding and server-sent events
* Can set timeouts for request/response and content
* Can set max request/response size
* Sending outgoing messages is thread safe
* Client creates necessary connections and perform reconnects when needed

## Usage

See:
[http_examples.cpp](examples/http_examples.cpp)
or
[https_examples.cpp](examples/https_examples.cpp) for example usage.
The following server resources are setup using regular expressions to match request paths:
* `POST /string` - responds with the posted string.
* `POST /json` - parses the request content as JSON, and responds with some of the parsed values.
* `GET /info` - responds with information extracted from the request.
* `GET /match/([0-9]+)` - matches for instance `/match/123` and responds with the matched number `123`.
* `GET /work` - starts a thread, simulating heavy work, and responds when the work is done.
* `GET` - a special default_resource handler is called when a request path does not match any of the above resources.
This resource responds with the content of files in the `web/`-folder if the request path identifies one of these files.

## Dependencies

* standalone Asio
* For HTTPS: OpenSSL libraries

Installation instructions for the dependencies needed to compile the examples on a selection of platforms can be seen below.
Default build with standalone Asio is assumed.

### Debian based distributions

```sh
sudo apt-get install libssl-dev
```

### Arch Linux based distributions

```sh
sudo pacman -S openssl
```

### MacOS

```sh
brew install openssl
```

## Compile and run


Choose **Release_Type** = (ReleaseSafe|ReleaseFast|ReleaseSmall)

Compile with a C++17 compliant compiler (like `zig c++` eql to `clang++ -stdlib=libc++ -fuse-ld=lld`):
```sh
zig build -Doptimize={Release_type}
```

### HTTP

Run the server and client examples: `zig build http_examples -Doptimize=ReleaseSafe`

Direct your favorite browser to for instance http://localhost:8080/

### HTTPS

Before running the server, an RSA private key (server.key) and an SSL certificate (server.crt) must be created.

Run the server and client examples: `zig build https_examples -Doptimize=ReleaseSafe`

Direct your favorite browser to for instance https://localhost:8080/

## Contributing

Contributions are welcome, either by creating an issue or a merge request.
However, before you create a new issue or merge request, please search for previous similar issues or requests.
A response will normally be given within a few days.


## Acknowledgments

Main author: [Ole Christian Eidheim](https://github.com/eidheim) - original project [Simple-Web-Server - Gitlab](https://gitlab.com/eidheim/Simple-Web-Server)