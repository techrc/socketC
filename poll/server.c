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

int serverSocket(int af, int type, int protocol, char *ip, int port);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int server_sock = serverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port);

    int POLLFDS_SIZE = 10;
    int nfds = 0, timeout = -1;
    struct pollfd pollfds[POLLFDS_SIZE];
    pollfds[0].fd = server_sock;
    pollfds[0].events = POLLIN;
    nfds++;
    for (int i = 1; i < POLLFDS_SIZE; i++) {
        pollfds[i].fd = -1;
        pollfds[i].revents = 0;
    }

    char buffer[512];
    while (1) {
        printf("\n--------polling...\n");
        int result_poll = poll(pollfds, nfds, timeout);
        if (result_poll <= 0) {
            printf("result_poll: %d\n", result_poll);
            return 1;
        }
        for (int j = 0; j < POLLFDS_SIZE; j++) {
            if (pollfds[j].revents & POLLIN) {
                pollfds[j].revents = 0;
                if (pollfds[j].fd == server_sock) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_size = sizeof(client_addr);
                    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
                    if (nfds == POLLFDS_SIZE) {
                        printf("max connection. refused.\n");
                        close(client_sock);
                        continue;
                    }
                    printf("accept: %d\n", client_sock);
                    for (int k = 0; k < POLLFDS_SIZE; k++) {
                        if (pollfds[k].fd == -1) {
                            pollfds[k].fd = client_sock;
                            pollfds[k].events = POLLIN;
                            nfds++;
                            break;
                        }
                    }
                } else {
                    int fd = pollfds[j].fd;
                    memset(buffer, 0, sizeof(buffer));
                    int result_read = read(fd, buffer, sizeof(buffer));
                    if (result_read <= 0) {
                        close(fd);
                        printf("client(%d) closed\n", fd);
                        pollfds[j].fd = -1;
                        nfds--;
                        continue;
                    }
                    printf("client(%d): %s\n", fd, buffer);
                    int result_write = write(fd, buffer, sizeof(buffer));
                    if (result_write <= 0) {
                        printf("server write failed\n");
                    }
                }
            }
        }
    }
    close(server_sock);

    return 0;
}

int serverSocket(int af, int type, int protocol, char *ip, int port) {
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 20);
    printf("listen: %s, %d\n", ip, port);
    return server_sock;
}

