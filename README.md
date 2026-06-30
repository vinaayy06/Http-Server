# HTTP Server in C++

A lightweight HTTP/1.1 server built from scratch in C++ using POSIX sockets. This project demonstrates low-level network programming by implementing core HTTP functionality without relying on external web frameworks.

## Features

* HTTP/1.1 request parsing
* Handles `GET` requests
* Serves static files (HTML, CSS, JavaScript, images, etc.)
* Returns appropriate HTTP status codes (200 OK, 404 Not Found)
* MIME type detection based on file extension
* Modular and beginner-friendly code structure
* Built using POSIX socket programming

## Tech Stack

* **Language:** C++17
* **Networking:** POSIX Sockets
* **Protocols:** TCP/IP, HTTP/1.1
* **Build Tool:** g++

## Project Structure

```text
.
├── src/
│   ├── main.cpp
│   ├── server.cpp
│   ├── request.cpp
│   ├── response.cpp
│   └── ...
├── public/
│   ├── index.html
│   ├── styles.css
│   └── images/
├── include/
├── README.md
└── Makefile
```

> Your project structure may differ. Update this section to match your repository.

## How It Works

1. Creates a TCP socket.
2. Binds the socket to a specified port.
3. Listens for incoming client connections.
4. Accepts HTTP requests from web browsers or tools like `curl`.
5. Parses the HTTP request.
6. Locates the requested resource.
7. Generates and sends a valid HTTP response.
8. Closes the client connection.

## Getting Started

### Clone the Repository

```bash
git clone https://github.com/<your-username>/http-server-cpp.git
cd http-server-cpp
```

### Build

```bash
g++ -std=c++17 src/*.cpp -o server
```

Or, if you're using a Makefile:

```bash
make
```

### Run

```bash
./server
```

The server starts listening on the configured port (for example, `4221`).

Open your browser and visit:

```text
http://localhost:4221
```

## Example Request

```http
GET / HTTP/1.1
Host: localhost:4221
```

## Example Response

```http
HTTP/1.1 200 OK
Content-Type: text/html

<html>
  ...
</html>
```

## Concepts Learned

* Socket programming
* TCP/IP networking
* HTTP request and response format
* File handling in C++
* Parsing text-based protocols
* MIME types
* Modular software design

## Future Improvements

* Multi-threaded client handling
* Persistent (Keep-Alive) connections
* Support for POST requests
* File uploads
* Routing system
* Logging
* HTTPS support
* Configuration file support

