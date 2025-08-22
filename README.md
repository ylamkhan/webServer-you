# Webserver

A custom HTTP/1.1 web server implementation written in C++ as part of the 1337 School curriculum.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [HTTP Methods](#http-methods)
- [Error Handling](#error-handling)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Resources](#resources)
- [Authors](#authors)

## Overview

This project implements a non-blocking HTTP/1.1 web server from scratch using C++98. The server can handle multiple simultaneous connections, serve static files, execute CGI scripts, and process various HTTP methods. It's designed to be compliant with HTTP/1.1 standards and handle real-world scenarios.

## Features

- **HTTP/1.1 Protocol Support**: Full implementation of HTTP/1.1 specification
- **Multiple Port Listening**: Can listen on multiple ports simultaneously
- **Virtual Hosting**: Support for multiple server blocks with different configurations
- **Static File Serving**: Efficient serving of HTML, CSS, JS, images, and other static content
- **CGI Support**: Execute CGI scripts (PHP, Python, etc.)
- **File Upload**: Handle file uploads via POST requests
- **Directory Listing**: Automatic directory index generation
- **Custom Error Pages**: Configurable error pages for different HTTP status codes
- **Non-blocking I/O**: Uses select() for handling multiple connections
- **Request Size Limits**: Configurable maximum request body size
- **HTTP Methods**: Support for GET, POST, DELETE methods
- **Redirection**: HTTP redirects support

## Requirements

- **Compiler**: C++98 compliant compiler (g++, clang++)
- **Operating System**: Unix-like systems (Linux, macOS)
- **Dependencies**: Standard C++ library only
- **Memory Management**: No memory leaks allowed
- **External Libraries**: Forbidden (except standard library)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/your-username/webserver.git
cd webserver
```

2. Compile the project:
```bash
make
```

3. Clean build files (optional):
```bash
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Clean rebuild
```

## Usage

### Basic Usage

```bash
./webserv [configuration_file]
```

If no configuration file is provided, the server will use a default configuration.

### Examples

```bash
# Run with default configuration
./webserv

# Run with custom configuration
./webserv config/server.conf

# Run with multiple server blocks
./webserv config/multi_server.conf
```

## Configuration

The server uses a configuration file similar to NGINX syntax. Here's an example configuration:

```nginx
server {
    listen 8080;
    server_name localhost;
    root ./www;
    index index.html index.htm;
    client_max_body_size 10M;

    location / {
        allowed_methods GET POST;
        autoindex on;
    }

    location /uploads {
        allowed_methods GET POST DELETE;
        upload_path ./uploads;
        autoindex on;
    }

    location /cgi-bin {
        allowed_methods GET POST;
        cgi_extension .php;
        cgi_path /usr/bin/php;
    }

    error_page 404 ./error_pages/404.html;
    error_page 500 502 503 504 ./error_pages/50x.html;
}
```

### Configuration Directives

- `listen`: Port number to listen on
- `server_name`: Server name (virtual host)
- `root`: Document root directory
- `index`: Default index files
- `client_max_body_size`: Maximum request body size
- `error_page`: Custom error pages
- `location`: Location-specific configuration
- `allowed_methods`: Allowed HTTP methods for location
- `autoindex`: Enable/disable directory listing
- `upload_path`: Directory for file uploads
- `cgi_extension`: CGI script file extension
- `cgi_path`: Path to CGI interpreter

## HTTP Methods

### GET
- Retrieve resources from the server
- Serve static files
- Execute CGI scripts
- Directory listing (if autoindex is on)

### POST
- Upload files to the server
- Send data to CGI scripts
- Form submissions

### DELETE
- Delete files from the server
- Only allowed in configured locations

## Error Handling

The server handles various HTTP status codes:

- **200 OK**: Successful request
- **201 Created**: Resource created successfully
- **204 No Content**: Successful deletion
- **301 Moved Permanently**: Redirection
- **400 Bad Request**: Malformed request
- **403 Forbidden**: Access denied
- **404 Not Found**: Resource not found
- **405 Method Not Allowed**: HTTP method not allowed
- **413 Payload Too Large**: Request body too large
- **500 Internal Server Error**: Server error
- **501 Not Implemented**: Method not implemented
- **505 HTTP Version Not Supported**: Unsupported HTTP version

## Testing

### Manual Testing

```bash
# Test with curl
curl -X GET http://localhost:8080/
curl -X POST -F "file=@test.txt" http://localhost:8080/uploads/
curl -X DELETE http://localhost:8080/uploads/test.txt

# Test with browser
open http://localhost:8080
```

### Siege Testing

```bash
# Install siege
sudo apt-get install siege  # Ubuntu/Debian
brew install siege          # macOS

# Run stress test
siege -c 10 -t 30s http://localhost:8080/
```

### CGI Testing

Create a simple PHP script in your CGI directory:

```php
#!/usr/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<h1>Hello from CGI!</h1>";
echo "<p>Current time: " . date('Y-m-d H:i:s') . "</p>";
?>
```

## Project Structure

```
webserver/
├── Makefile
├── README.md
├── config/
│   ├── default.conf
│   └── test.conf
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── Server.hpp
│   ├── Request.cpp
│   ├── Request.hpp
│   ├── Response.cpp
│   ├── Response.hpp
│   ├── Config.cpp
│   ├── Config.hpp
│   ├── CGI.cpp
│   ├── CGI.hpp
│   └── Utils.cpp
│   └── Utils.hpp
├── www/
│   ├── index.html
│   ├── css/
│   ├── js/
│   └── images/
├── cgi-bin/
│   └── hello.php
├── uploads/
├── error_pages/
│   ├── 404.html
│   └── 50x.html
└── tests/
    ├── unit_tests/
    └── integration_tests/
```

## Resources

- [HTTP/1.1 Specification (RFC 7230-7237)](https://tools.ietf.org/html/rfc7230)
- [CGI Specification (RFC 3875)](https://tools.ietf.org/html/rfc3875)
- [NGINX Configuration Guide](https://nginx.org/en/docs/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
