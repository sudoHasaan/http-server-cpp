# Build Your Own HTTP Server in C++

This repository contains my implementation of a basic HTTP/1.1 server, built from the ground up using C++ and core networking libraries. This project was undertaken as part of the ["Build Your Own HTTP Server" challenge on CodeCrafters](https://app.codecrafters.io/courses/http-server/overview) to gain a deep, practical understanding of network programming and the HTTP protocol.

The server is capable of handling multiple client connections, parsing HTTP requests, routing to different endpoints, and generating dynamic responses.

---

## Key Features Implemented

The server was built incrementally, with each stage adding a new layer of functionality. The final implementation includes:

*   **TCP Server Foundation:** Establishes a listening TCP socket on port 4221, capable of accepting and handling incoming client connections.

*   **HTTP Request/Response Handling:**
    *   **Request Parsing:** Parses the request line to extract the HTTP method (GET/POST) and the URL path for routing.
    *   **Header Parsing:** Reads and interprets request headers, such as `User-Agent` and `Accept-Encoding`.
    *   **Response Generation:** Constructs valid `HTTP/1.1` responses with a status line, appropriate headers, and a message body.

*   **Endpoint Routing:** Implements a routing system to handle different paths:
    *   `GET /`: Responds with a `200 OK`.
    *   `GET /echo/{str}`: A dynamic endpoint that echoes the string from the URL back in the response body.
    *   `GET /user-agent`: Parses the `User-Agent` header from the request and returns its value in the response body.
    *   Any other `GET` path: Responds with a `404 Not Found`.

*   **Filesystem Interaction:**
    *   **File Serving (GET):** Supports serving files from a server-specified directory. Requests to `/files/{filename}` will return the contents of the requested file with an `application/octet-stream` content type.
    *   **File Uploads (POST):** Supports file creation by handling `POST` requests to `/files/{filename}`. It parses the request body and writes its contents to a new file on the server, responding with `201 Created`.

*   **HTTP Content Negotiation (Compression):**
    *   **Robust Header Parsing:** Implements logic to correctly parse comma-separated values from the `Accept-Encoding` header.
    *   **Compression Scheme Selection:** The server intelligently scans the list of encodings supported by the client and determines if `gzip` is a valid option.
    *   **Conditional Encoding:** If the client supports `gzip`, the server correctly includes the `Content-Encoding: gzip` header in its response. (Note: Actual body compression is a future extension).
---

## Technology Stack

*   **Language:** C++
*   **Core APIs:**
    *   **POSIX Sockets API** (`sys/socket.h`, `arpa/inet.h`) for all low-level network I/O.
    *   **C++ Standard Library** (`<iostream>`, `<string>`, `<fstream>`, `<sstream>`) for I/O, string manipulation, and file handling.
*   **Build System:** CMake
*   **Development Environment:** Windows Subsystem for Linux (WSL)

---

## How to Build and Run

#### Prerequisites
*   A C++ compiler (like `g++`)
*   `cmake`

#### Steps
1.  Clone the repository:
    ```sh
    git clone <your-repo-url>
    cd <your-repo-folder>
    ```

2.  Build the project using CMake:
    ```sh
    cmake -B build
    cmake --build build
    ```
    This will create an executable at `build/server`.

3.  Run the server:
    The server accepts a `--directory` argument to specify the root for file operations.
    ```sh
    ./build/server --directory /path/to/your/files
    ```

---

## Project Journey & Learnings

Building this server from scratch was an incredible learning experience. It solidified my understanding of fundamental concepts that are often abstracted away by modern frameworks, including:
*   The raw, text-based nature of the HTTP protocol.
*   The lifecycle of a TCP socket: from creation and binding to listening and accepting connections.
*   The importance of meticulous string parsing for routing and header/body extraction.
*   Low-level system interactions, such as filesystem I/O and command-line argument parsing.
*   The client-server negotiation process for features like content encoding.

Debugging issues like off-by-one errors in buffer handling and incorrect `Content-Length` headers provided invaluable, hands-on experience in network programming.