# Http-Server

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-064F8C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A multithreaded HTTP/1.1 server built from scratch in modern C++ using raw POSIX sockets — no external HTTP frameworks (no Boost.Asio, no cpp-httplib). Every part of the protocol — connection handling, request parsing, response construction, compression — is implemented manually at the socket level.

## Highlights

- **Concurrent client handling** — spawns a dedicated thread per connection, allowing multiple clients to be served simultaneously without blocking
- **Persistent connections (HTTP keep-alive)** — reuses a single TCP socket across multiple requests instead of reconnecting for every call, reducing connection overhead
- **On-the-fly gzip compression** — compresses response bodies when the client advertises support via `Accept-Encoding`, cutting payload size for compressible content
- **Manual HTTP/1.1 parsing** — request line, headers, and body are parsed directly from the raw byte stream, with no parsing library
- **File I/O endpoints** — reads and writes files from disk in response to `GET`/`POST` requests
- **Modular, readable codebase** — connection handling, request parsing, and response building are cleanly separated

## Tech Stack

| Layer | Tooling |
|---|---|
| Language | C++17, POSIX sockets, `std::thread` |
| Build system | CMake |
| Dependency management | vcpkg |
| Scripts | Shell |

## Project Structure

```
Http-Server/
├── src/                      # Server source code (connection handling, parsing, routing)
├── CMakeLists.txt            # Build configuration
├── vcpkg.json                 # Dependency manifest
├── vcpkg-configuration.json   # vcpkg registry configuration
└── run.sh                     # Build & run entrypoint
```

## Getting Started

**Prerequisites:** a C++17-capable compiler, CMake, and [vcpkg](https://vcpkg.io).

```bash
git clone https://github.com/vinaayy06/Http-Server.git
cd Http-Server
./run.sh
```

The script configures CMake with the vcpkg toolchain, builds the binary, and starts the server on `localhost:4221`.

### Try it out

```bash
# Basic request
curl -v http://localhost:4221/

# Echo endpoint
curl -v http://localhost:4221/echo/hello

# gzip compression
curl -v --compressed http://localhost:4221/echo/hello

# Write a file
curl -v -X POST --data "sample content" http://localhost:4221/files/test.txt

# Read it back
curl -v http://localhost:4221/files/test.txt
```

## How It Works

1. The server opens a listening TCP socket and accepts incoming connections in a loop.
2. Each accepted connection is handed off to a new detached thread, so the main loop can immediately accept the next connection — no client has to wait on another.
3. Within each thread, the raw request is read from the socket and parsed into method, path, headers, and body.
4. The request is routed to the appropriate handler based on path (echo, file read/write, etc.).
5. If the client supports gzip, the response body is compressed before being written back to the socket.
6. If the connection is `keep-alive` (the HTTP/1.1 default), the socket stays open for further requests instead of closing after one response.

## Why I Built This

I wanted low-level, hands-on experience with the things a web framework normally hides:

- Raw networking (`socket`, `bind`, `listen`, `accept`) instead of a framework's connection layer
- The actual bytes of the HTTP/1.1 protocol — status lines, headers, keep-alive semantics
- Concurrency in C++ — thread lifecycle, safe resource cleanup, avoiding race conditions across client threads
- Systems programming fundamentals that carry over to any backend or infrastructure work

## Possible Improvements

- Replace detached threads with a fixed-size thread pool to cap resource usage under high load
- Add chunked transfer-encoding support for streaming responses
- Add basic routing/middleware abstraction for extensibility

## License

MIT — see [LICENSE](LICENSE) for details.
