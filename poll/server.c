#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include "sockutils.h"

#define BUFFER_SIZE 512
#define BACKLOG 20
#define MAX_CLIENTS 10
#define POLLFD_EMPTY -1

void init_pollfds(struct pollfd *pollfds, int size);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int listen_socket = create_listen_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port, BACKLOG);

    int nfds = 0, poll_timeout = -1;

    const int POLLFDS_SIZE = MAX_CLIENTS + 1;
    struct pollfd pollfds[POLLFDS_SIZE];
    init_pollfds(pollfds, POLLFDS_SIZE);
    pollfds[0].fd = listen_socket;
    pollfds[0].events = POLLIN;
    nfds++;

    while (1) {
        printf("-- Polling...\n");
        int result_poll = poll(pollfds, nfds, poll_timeout);
        if (result_poll <= 0) {
            printf("** Poll failed, return code is: %d\n", result_poll);
            return 1;
        }

        char buffer[BUFFER_SIZE];
        for (int i = 0; i < nfds; i++) {
            if (pollfds[i].revents == 0) {
                continue;
            }
            struct pollfd *ready_pollfd = &pollfds[i];

            if (ready_pollfd->revents & (POLLERR | POLLHUP | POLLNVAL)) {
                close(ready_pollfd->fd);
                printf("** Poll failed on client(socket number: %d), closed\n", ready_pollfd->fd);

                pollfds[i] = pollfds[nfds - 1];
                nfds--;
                i--;
                continue;
            }

            if (ready_pollfd->fd == listen_socket) {
                int new_client_socket = accept_socket(listen_socket);

                // 检查连接数边界
                if (nfds >= POLLFDS_SIZE) {
                    printf("** Exceed max connection. refused.\n");
                    close(new_client_socket);
                    continue;
                }

                pollfds[nfds].fd = new_client_socket;
                pollfds[nfds].events = POLLIN;
                nfds++;
            } else {
                memset(buffer, 0, sizeof(buffer));
                int result_read = read(ready_pollfd->fd, buffer, sizeof(buffer));
                if (result_read <= 0) {
                    close(ready_pollfd->fd);
                    printf("** Read from client(socket number: %d) failed, return code is: %d, closed\n",
                           ready_pollfd->fd, result_read);
                    pollfds[i] = pollfds[nfds - 1];
                    nfds--;
                    i--;
                    continue;
                }
                printf("<< Read from client(socket number: %d): %s\n", ready_pollfd->fd, buffer);
                write_msg(ready_pollfd->fd, buffer);
            }
        }
    }
    close(listen_socket);
    return 0;
}

void init_pollfds(struct pollfd *pollfds, int size) {
    for (int i = 0; i < size; i++) {
        pollfds[i].fd = POLLFD_EMPTY;
        pollfds[i].events = 0;
        pollfds[i].revents = 0;
    }
}
