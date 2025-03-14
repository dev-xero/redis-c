#include "../include/constants.h"
#include "../include/utils.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
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
#include <sys/poll.h>
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

static Conn *handle_accept(int fd) {
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
        return NULL;
    }
    fd_set_nb(connfd);
    // Create a `struct Conn`
    Conn *conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;

    return conn;
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

        // Wait for readiness
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), -1);
        if (rv < 0 && errno == EINTR) {
            continue;
        }
        if (rv < 0) {
            die("poll()");
        }

        // Handle the listening socket
        if (poll_args[0].revents) {
            if (Conn *conn = handle_accept(fd)) {
                if (fdToConn.size() <= (size_t)conn->fd) {
                    fdToConn.resize(conn->fd + 1);
                }
                fdToConn[conn->fd] = conn;
            }
        }
    }

    return 0;
}
