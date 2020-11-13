# HTTP Web Server

General purpose HTTP web server written in C. It does not support dynamic content generation; it simply transmits files that are requested (via GET) to the HTTP client. 

## Features

- Implements a subset of the HTTP/1.1 specification
	- Handles HTTP request types: GET, HEAD
	- Responds with an error if the request is malformed
- Listens for connections on a socket that is bound to a network port number
	- Port number is specified as a command line argument
- Handles non-persistent connections
- Can handle multiple requests simultaneously using fork
- Determines if requested file exists
	- If yes, transmits file with appropriate HTTP response headers
	- If no, transmits appropriate HTTP error code and message

### Usage

The web server must be started from the command line with two command line arguments:

1. DOCROOT: server path to web server files
2. PORT: what network port to use
	- Note: Use a port number higher than 1024 unless you have administrative/root privileges. Conventionally, port 8080 is a typical unprivileged port number used for HTTP (it can be any port number).

```shell
% ./http-web-server /tmp/www/htdocs 8080
```
