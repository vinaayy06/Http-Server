# Http-Server

An HTTP/1.1 server built from scratch in modern C++ using raw POSIX sockets — no external HTTP frameworks. Built as part of the [CodeCrafters "Build Your Own HTTP Server"](https://codecrafters.io) challenge.

## Features

- **Request parsing** — parses HTTP/1.1 request lines, headers, and bodies manually from raw socket data
- **Concurrent connections** — spawns a detached thread per client connection (`std::thread` + `.detach()`) so multiple clients are served simultaneously
- **Persistent connections (keep-alive)** — supports HTTP/1.1 persistent connections, reusing the same TCP socket across multiple requests
- **gzip compression** — compresses response bodies with gzip when the client advertises support via `Accept-Encoding`
- **Static file serving** — reads and serves files from disk, with support for `GET` and `POST` file I/O
- **Modular architecture** — request handling, response building, and connection logic are separated for readability

## Tech Stack

- **C++** (POSIX sockets, `std::thread`)
- **CMake** — build system
- **vcpkg** — dependency management (see `vcpkg.json`)
- **Shell** — build/run scripts (`your_program.sh`)

## Project Structure

```
Http-Server/
├── src/                    # Server source code
├── .codecrafters/          # CodeCrafters challenge config
├── CMakeLists.txt          # Build configuration
├── vcpkg.json               # Dependency manifest
├── vcpkg-configuration.json
└── your_program.sh          # Build & run entrypoint
```

## Building and Running

This project uses CMake with vcpkg for dependencies.

```bash
# Clone the repo
git clone https://github.com/vinaayy06/Http-Server.git
cd Http-Server

# Build and run
./your_program.sh
```

The script handles configuring CMake with the vcpkg toolchain, building the binary, and starting the server.

By default the server listens on `localhost:4221`. Once running, you can test it with:

```bash
curl -v http://localhost:4221/
curl -v http://localhost:4221/echo/hello
curl -v --compressed http://localhost:4221/echo/hello   # test gzip
curl -v -X POST --data "sample content" http://localhost:4221/files/test.txt
curl -v http://localhost:4221/files/test.txt
```

*(Adjust paths/port above if your route handling or config differs — update this section to match your actual endpoints.)*

## How It Works

1. The server opens a listening TCP socket and accepts incoming connections in a loop.
2. Each accepted connection is handed off to a new detached thread (`handle_client`), so the main loop can immediately accept the next connection.
3. Inside each thread, the raw request is read from the socket and parsed into method, path, headers, and body.
4. Based on the path, the server routes the request to the appropriate handler (echo, file read/write, etc.).
5. If the client supports gzip (`Accept-Encoding: gzip`), the response body is compressed before sending.
6. If the client requested `Connection: keep-alive` (default in HTTP/1.1), the socket stays open for subsequent requests instead of closing after one response.

## Learning Goals

This project was built to get hands-on experience with:
- Low-level networking (POSIX sockets, `bind`/`listen`/`accept`)
- The HTTP/1.1 protocol internals (headers, status lines, chunked/keep-alive semantics)
- Concurrency in C++ (threading, resource cleanup)
- Systems programming fundamentals outside of high-level frameworks

## License

This project is for educational purposes as part of the CodeCrafters challenge.
