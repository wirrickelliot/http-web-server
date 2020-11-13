#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <arpa/inet.h>
#include <errno.h>
#include <zconf.h>
#include <stdlib.h>
#include <time.h>


void print_addr_info(struct addrinfo * info) {
    struct sockaddr_in * tmpp;
    char ip4[INET_ADDRSTRLEN];

    while(info) {
        if (info->ai_family == PF_INET) {
            tmpp = (struct sockaddr_in *) info->ai_addr;
            inet_ntop(AF_INET, &tmpp->sin_addr, ip4, INET_ADDRSTRLEN);

            printf("IPv4 address: %s\n", ip4);
        }
        info = info->ai_next;
    }
}

char* get_request(int acceptfd) {
    size_t p = 0;
    char c;
    char* s = malloc(1);

    while (c != '\r') {
        recv(acceptfd, &c, 1, 0);
        s[p] = c;
        p += 1;
    }
    s[p - 1] = '\0';

    printf("HTTP VERSION: %s\n", s);
    return s;
}

char* http_request_type(int acceptfd) {
    size_t p = 0;
    char c;
    char* s = malloc(1);

    while (c != ' ') {
        recv(acceptfd, &c, 1, 0);
        s[p] = c;
        p += 1;
    }
    s[p - 1] = '\0';

    printf("REQUEST TYPE: %s\n", s);
    return s;
}

char* http_request_path(int acceptfd) {
    size_t p = 0;
    char c;
    char* s = malloc(1);

    while (c != ' ') {
        recv(acceptfd, &c, 1, 0);
        s[p] = c;
        p += 1;
    }
    s[p - 1] = '\0';
    s[0] = '\0';

    recv(acceptfd, &c, 1, 0);
    s[p] = c;
    p += 1;

    while(c != ' ') {
        recv(acceptfd, &c, 1, 0);
        s[p] = c;
        p += 1;
    }
    s[p - 1] = '\0';

    printf("REQUEST PATH: %s\n", s);
    return s;
}

int parse_request(char* method, char* version) {
    if (((strcmp(method, "GET")) != 0) && ((strcmp(method, "HEAD")) != 0)) {
        return 1;
    }
    if (((strcmp(version, "HTTP/1.1")) != 0) && ((strcmp(version, "HTTP/1.0")) != 0)) {
        return 1;
    }
    return 0;
}

int send_headers(int acceptfd, char* type, char* request_type, char* file, char* version) {
    char time_buffer[128];
    char read_buffer[1024];
    char send_buffer[1024];
    FILE *fp;
    int status;
    long len;

    if (parse_request(request_type, version) == 1) {
        sprintf(send_buffer, "HTTP/1.1 400 Bad Request\r\n");
        send(acceptfd, send_buffer, strlen(send_buffer), 0);
    } else if (fopen(file, "rb") == NULL) {
        sprintf(send_buffer, "HTTP/1.1 404 Not Found\r\n");
        send(acceptfd, send_buffer, strlen(send_buffer), 0);
    } else if (strcmp(request_type, "GET") == 0) {
        sprintf(send_buffer, "HTTP/1.1 200 OK\r\n");
        send(acceptfd, send_buffer, strlen(send_buffer), 0);
    } else if (strcmp(request_type, "HEAD") == 0) {
        sprintf(send_buffer, "HTTP/1.1 200 OK\r\n");
        send(acceptfd, send_buffer, strlen(send_buffer), 0);
    } else {
        fprintf(stderr, "Error: not implemented\n");
        return 1;
    }

    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(time_buffer, sizeof time_buffer, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    printf("TIME: %s\n", time_buffer);

    sprintf(send_buffer, "Date: %s\r\n", time_buffer);
    send(acceptfd, send_buffer, strlen(send_buffer), 0);

    if (((fopen(file,"rb")) == NULL) || ((parse_request(request_type, version)) == 1)) {
        sprintf(send_buffer, "Content-Type: text/html\r\n");
        send(acceptfd, send_buffer, strlen(send_buffer), 0);
    } else {
        if (strcmp(type, ".jpeg") == 0) {
            sprintf(send_buffer, "Content-Type: image/jpeg\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".jpg") == 0) {
            sprintf(send_buffer, "Content-Type: image/jpeg\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".png") == 0) {
            sprintf(send_buffer, "Content-Type: image/png\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".gif") == 0) {
            sprintf(send_buffer, "Content-Type: image/gif\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".pdf") == 0) {
            sprintf(send_buffer, "Content-Type: application/pdf\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".js") == 0) {
            sprintf(send_buffer, "Content-Type: application/javascript\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".html") == 0) {
            sprintf(send_buffer, "Content-Type: text/html\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".txt") == 0) {
            sprintf(send_buffer, "Content-Type: text/plain\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else if (strcmp(type, ".css") == 0) {
            sprintf(send_buffer, "Content-Type: text/css\r\n");
            send(acceptfd, send_buffer, strlen(send_buffer), 0);
        } else {
            fprintf(stderr, "Error: file type not supported\n");
            return 1;
        }
    }

        if ((fp = fopen(file, "rb")) != NULL) {
            fseek(fp, 0L, SEEK_END);
            len = ftell(fp);
            printf("%ld\n", len);
            rewind(fp);
            fclose(fp);
        } else {
            len = 0;
        }
        if ((parse_request(request_type, version)) == 1) {
            len = 0;
        }

        sprintf(send_buffer, "Content-Length: %ld\r\n", len);
        send(acceptfd, send_buffer, strlen(send_buffer), 0);

    sprintf(send_buffer, "\r\n");
    send(acceptfd, send_buffer, strlen(send_buffer), 0);

    do {
        status = recv(acceptfd, read_buffer, sizeof(read_buffer), MSG_DONTWAIT);
    } while (status > 0);


    return 0;
}

int send_body(int acceptfd, char* file, char* type, char* request_type, char* version) {
    size_t s = 0;
    char* buffer = malloc(1);
    FILE* fp  = NULL;
    int c;

    if (send_headers(acceptfd, type, request_type, file, version) == 1) return 1;

    if ((parse_request(request_type, version)) == 1) {
        sprintf(buffer, "<html><body>400 Bad Request</body></html>\r\n");
        send(acceptfd, buffer, strlen(buffer), 0);
        return 1;
    }

    if ((fp = fopen(file, "rb")) != NULL) {
        while ((c = fgetc(fp)) != EOF) {
            buffer[s] = (char) c;
            send(acceptfd, buffer, 1, 0);
        }
        fclose(fp);
    } else {
        sprintf(buffer, "<html><body>404 Not Found</body></html>\r\n");
        send(acceptfd, buffer, strlen(buffer), 0);
        return 1;
    }

    free(buffer);
    return 0;
}

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo * address_resource;

    if (argc != 3) {
        fprintf(stdout, "Usage: server [docroot] [port]\n");
        return 1;
    }

    char *docroot = argv[1];
    printf("DOCROOT: %s\n", docroot);

    char *port = argv[2];
    printf("PORT: %s\n", port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &address_resource);

    print_addr_info(address_resource);

    int socket_desc = socket(address_resource->ai_family,
                             address_resource->ai_socktype,
                             address_resource->ai_protocol);

    int enable = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        fprintf(stderr, "Error: setsockopt failed\n");
    }

    if (bind(socket_desc, address_resource->ai_addr, address_resource->ai_addrlen) != 0) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }

    freeaddrinfo(address_resource);
    listen(socket_desc, 1);

    struct sockaddr_storage remote_addr;
    socklen_t remote_addr_s = sizeof remote_addr;
    int accept_desc;

    while(1) {
        accept_desc = accept(socket_desc, (struct sockaddr *) &remote_addr, &remote_addr_s);

        if (!fork()) {
            close(socket_desc);

            char* request_type = http_request_type(accept_desc);
            char* request_path = http_request_path(accept_desc);
            char* http_type = get_request(accept_desc);

            if (strcmp(request_path, "/") == 0) {
                request_path = "/index.html";
            }

            char* cat_path = strcat(docroot, request_path);
            printf("%s\n", cat_path);
            char* p = strrchr(request_path, '.');

            send_body(accept_desc, cat_path, p, request_type, http_type);

            close(accept_desc);
            exit(0);
        }
        close(accept_desc);
    }
}
