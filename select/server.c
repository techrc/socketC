#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "sockutils.h"

#define BUFFER_SIZE 512
#define BACKLOG 20
#define MAX_CLIENTS 10
#define CLIENT_SOCKET_EMPTY -1

int regen_readfds(fd_set *readfds, int listen_socket, int client_sockets[]);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int listen_socket = create_listen_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port, BACKLOG);

    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = CLIENT_SOCKET_EMPTY;
    }

    fd_set readfds;
    while (1) {
        int max_fd = regen_readfds(&readfds, listen_socket, client_sockets);
        int nfds = max_fd + 1;

        printf("-- Selecting...\n");
        int nready = select(nfds, &readfds, NULL, NULL, NULL);
        if (nready <= 0) {
            printf("** Select failed, return code is %d\n", nready);
            return 1;
        }

        if (FD_ISSET(listen_socket, &readfds)) {
            int new_client_socket = accept_socket(listen_socket);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == CLIENT_SOCKET_EMPTY) {
                    client_sockets[i] = new_client_socket;
                    break;
                }
            }
        }

        char buffer[BUFFER_SIZE];
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_socket = client_sockets[i];
            if (client_socket == CLIENT_SOCKET_EMPTY) {
                continue;
            }
            if (FD_ISSET(client_socket, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int result_read = read(client_socket, buffer, BUFFER_SIZE);
                if (result_read <= 0) {
                    close(client_socket);
                    printf("** Read from client (socket number %d) failed, return code is %d, closed\n",
                           client_socket, result_read);
                    client_sockets[i] = CLIENT_SOCKET_EMPTY;
                    continue;
                }
                printf("<< Read from client (socket number: %d): %s\n", client_socket, buffer);
                write_msg(client_socket, buffer);
            }
        }
    }
    close(listen_socket);
    return 0;
}

int regen_readfds(fd_set *p_readfds, int listen_socket, int client_sockets[]) {
    FD_ZERO(p_readfds);
    FD_SET(listen_socket, p_readfds);
    int max_fd = listen_socket;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != CLIENT_SOCKET_EMPTY) {
            FD_SET(client_sockets[i], p_readfds);
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }
    }
    return max_fd;
}
