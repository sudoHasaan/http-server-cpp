# C++ HTTP/1.1 Server

This repository contains my implementation of a multi-threaded HTTP/1.1 server, built from the ground up using C++ and core networking libraries. This project was undertaken as part of the **CodeCrafters "Build Your Own HTTP Server" challenge** to gain a deep, practical understanding of network protocols, concurrency, and system-level programming.

The server is capable of handling multiple concurrent clients, parsing and routing HTTP requests, serving static files, creating new files via POST requests, and negotiating content compression with clients.

[![progress-banner](https://backend.codecrafters.io/progress/http-server/1517dbe9-b93f-42f5-bd6a-7a19913a953d)](https://app.codecrafters.io/users/sudoHasaan?r=2qF)

---

## Key Features Implemented

The server was built incrementally, with each stage adding a new layer of functionality. The final implementation includes:

*   **Concurrent & Persistent Connections:**
    *   **Multi-Threading:** Utilizes C++ threads (`std::thread`) to handle multiple client connections simultaneously. Each incoming connection is dispatched to its own worker thread for processing.
    *   **HTTP/1.1 Keep-Alive:** Implements persistent connections by default, allowing a single TCP connection to be reused for multiple sequential requests from the same client to reduce latency.
    *   **Connection Management:** Correctly handles the `Connection: close` header. If a client requests to close the connection, the server acknowledges by including the header in its response and gracefully closes the connection after the response is sent.

*   **Robust HTTP Parsing:**
    *   **Request Line:** Parses the request line to extract the HTTP method (`GET`/`POST`), URL path, and protocol version.
    *   **Headers:** Scans request headers to extract key-value pairs, specifically handling `User-Agent`, `Accept-Encoding`, `Content-Length`, and `Connection`.
    *   **Request Body:** Correctly extracts the request body payload for `POST` requests based on the `Content-Length` header.

*   **Dynamic Endpoint Routing:** Implements a routing system to handle different paths and methods:
    *   `GET /`: Responds with a `200 OK`.
    *   `GET /echo/{str}`: A dynamic endpoint that echoes the requested string back to the client.
    *   `GET /user-agent`: Returns the client's `User-Agent` string in the response body.
    *   `GET /files/{filename}`: Serves the contents of a specified file from a server-configured directory.
    *   `POST /files/{filename}`: Creates a new file on the server with the contents of the request body.
    *   Responds with `404 Not Found` for any other routes.

*   **Gzip Content Compression:**
    *   **Content Negotiation:** Intelligently parses the `Accept-Encoding` header, tokenizing comma-separated values to accurately detect client support for `gzip`.
    *   **Third-Party Library Integration:** Successfully integrates and links the industry-standard `zlib` library to perform data compression.
    *   **Dynamic Gzip Compression:** If the client supports it, the server compresses the response body in-memory using `zlib` before sending it.
    *   **Correct Header Generation:** When content is compressed, the server correctly adds the `Content-Encoding: gzip` header and calculates the `Content-Length` based on the size of the *compressed* binary data.

*   **Filesystem Interaction:**
    *   The server can be configured with a root directory via command-line arguments (`--directory`).
    *   Uses C++ file streams (`<fstream>`) for both reading existing files and writing new ones.
---

## Technical Details

*   **Language:** C++11 (or newer)
*   **Core APIs & Libraries:**
    *   **POSIX Sockets API** (`<sys/socket.h>`, `<arpa/inet.h>`) for all low-level network I/O.
    *   **C++ Standard Library:**
        *   `<thread>` for concurrency.
        *   `<string>`, `<vector>`, `<sstream>` for robust string parsing and data handling.
        *   `<fstream>` for all filesystem operations.
    *   **zlib (`<zlib.h>`)**: For `gzip` compression.
*   **Build System:** Configured for `g++` compilation with `-pthread` and `-lz` flags.
*   **Development Environment:** Developed and tested within Windows Subsystem for Linux (WSL).

---

## How to Build and Run

#### Prerequisites
*   A C++ compiler (like `g++`)
*   The `zlib` development library (`sudo apt install zlib1g-dev` on Debian/Ubuntu)

#### Steps
1.  Clone the repository:
    ```sh
    git clone <your-repo-url>
    cd <your-repo-folder>
    ```

2.  Compile the server:
    The `-pthread` flag is required for `std::thread` and `-lz` is for linking `zlib`.
    ```sh
    g++ src/server.cpp -o server -pthread -lz
    ```

3.  Run the server:
    The server accepts a `--directory` argument to specify the root for file operations. If omitted, it defaults to the current directory.
    ```sh
    ./server --directory /tmp/files/
    ```

---

## Project Journey & Key Learnings

Building this server from scratch was an incredible learning experience in low-level systems programming. It solidified my understanding of fundamental concepts often abstracted away by modern frameworks, including:
*   The raw, text-based nature of the HTTP protocol.
*   The lifecycle of a TCP socket: from `socket()` to `bind()`, `listen()`, and `accept()`.
*   The challenges and solutions for handling multiple clients concurrently using threads.
*   Meticulous string parsing for routing and extracting data from requests.
*   The practical application of integrating and linking third-party C libraries like `zlib` into a C++ project.

Debugging issues related to concurrency, off-by-one errors in buffer handling, and dynamic `Content-Length` calculation provided invaluable, hands-on experience in building robust network applications.