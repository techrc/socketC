#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sockutils.h"

int create_listen_socket(int af, int type, int protocol, char *ip, int port, int backlog) {
    int listen_sock = socket(af, type, protocol);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_sock, backlog);
    printf("-- Listen: %s:%d\n", ip, port);
    return listen_sock;
}

int accept_socket(int listen_sock) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    printf("-- Accept a client (socket number: %d)\n", client_sock);
    return client_sock;
}

void write_msg(int client_sock, const void *buffer) {
    int result_write = write(client_sock, buffer, sizeof(buffer));
    if (result_write < 0) {
        printf("** Write to client (socket number: %d) failed, return code is: %d\n", client_sock, result_write);
    }
    printf(">> Answer client (socket number: %d) done\n", client_sock);
}
