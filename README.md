# Webserv - HTTP Server Project - README

This project involves creating an HTTP server in C++98 that adheres to the HTTP 1.1 standard. The server must handle multiple client connections concurrently without blocking, and it should be able to handle various HTTP methods like GET, POST, and DELETE. The server configuration is defined in a configuration file, where you can set server parameters, routes, error pages, and more.

## Features:

- Implements a non-blocking HTTP server in C++98.
- Handles multiple client connections using a single `poll()` (or equivalent) instance.
- Supports various HTTP methods: GET, POST, and DELETE.
- Allows configuration through a configuration file similar to NGINX.
- Handles routes, directory listings, file serving, and CGI execution.
- Supports uploading files from clients.
- Provides default error pages.
- Ensures accurate HTTP response status codes.
- Ensures compatibility with web browsers like NGINX.
- Can listen on multiple ports based on the configuration.
- Utilizes C++ features where possible, following C++98 standards.

## Usage:

1. Compile the source files using the provided Makefile.
2. Run the compiled executable: `./webserv [configuration_file]`.

## Configuration:

- The configuration file defines server settings, routes, and behavior.
- Specify the port and host for each server.
- Set up default error pages and limit client body size.
- Configure routes with various rules like accepted HTTP methods, redirection, file serving, and CGI execution.
- Define the default file for a directory request and enable/disable directory listing.

## Bonus Features:

- Support cookies and session management.
- Handle multiple CGI.

## Important:

- Ensure the server never crashes or quits unexpectedly.
- Code must comply with C++98 standards.
- Use only non-blocking I/O and adhere to the specified requirements.
- Test your server's resilience and compatibility with different browsers and tools.
- Consider using provided testers and other languages for thorough testing.
- Aim for perfect implementation of the mandatory requirements before attempting bonus features.

## Contributions:

Contributions are welcome! Feel free to submit issues and pull requests for improvements or bug fixes.

## Disclaimer:

This project is developed based on the C++98 standard and implements a subset of the HTTP 1.1 protocol. It may not cover all edge cases or advanced features. Please refer to the project documentation for more information.
