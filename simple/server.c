#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sockutils.h"

#define BUFFER_SIZE 512
#define BACKLOG 5

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int listen_socket = create_listen_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port, BACKLOG);

    while (1) {
        int client_socket = accept_socket(listen_socket);
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int result_read = read(client_socket, buffer, sizeof(buffer)-1);
            if (result_read <= 0) {
                close(client_socket);
                printf("** Read from client (socket number %d) failed, return code is %d, closed\n",
                       client_socket, result_read);
                break;
            }
            printf("<< Read from client (socket number: %d): %s\n", client_socket, buffer);
            write_msg(client_socket, buffer);
        }
    }
    close(listen_socket);
    return 0;
}
