#include "../include/constants.h"
#include "../include/utils.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

struct Conn {
    int fd = -1;
    // application's intention, for the event loop
    bool want_read = false;
    bool want_write = false;
    bool want_close = false;
    // bufferred input and output
    std::vector<uint8_t> incoming;
    std::vector<uint8_t> outgoing;
};

static int32_t one_request(int connfd) {
    // 4 bytes header
    char rbuf[4 + K_MAX_MSG];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    uint32_t len = 0;
    // Assuming Little-Endian archs
    memcpy(&len, rbuf, 4);
    if (len > K_MAX_MSG) {
        msg("Message is too long.");
        return -1;
    }

    // Request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    fprintf(stderr, "client says: %.*s\n", len, &rbuf[4]);

    // Reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);

    return write_all(connfd, wbuf, 4 + len);
}

int main() {
    // Obtain socket file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // Bind socket to address
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0); // wildcard 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    // Listen for connections
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    // Server Event Loop
    std::vector<Conn *> fdToConn;
    std::vector<struct pollfd> poll_args;
    while (true) {
        poll_args.clear();
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);
        for (Conn *conn : fdToConn) {
            if (!conn) {
                continue;
            }
            struct pollfd pfd = {conn->fd, POLLERR, 0};
            // poll() flags from the application's intent
            if (conn->want_read) {
                pfd.events |= POLLIN;
            }
            if (conn->want_write) {
                pfd.events |= POLLOUT;
            }
            poll_args.push_back(pfd);
        }
    }

    return 0;
}
