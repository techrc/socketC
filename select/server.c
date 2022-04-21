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

int serverSocket(int af, int type, int protocol, char *ip, int port);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int server_sock = serverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port);

    fd_set fds, rs;
    FD_ZERO(&rs);
    FD_ZERO(&fds);
    FD_SET(server_sock, &fds);

    char buffer[512];
    while (1) {
        rs = fds;
        printf("selecting...\n");
        int result_select = select(FD_SETSIZE, &rs, NULL, NULL, NULL);
        for (int fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &rs)) {
                if (fd == server_sock) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_size = sizeof(client_addr);
                    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
                    printf("accept: %d\n", client_sock);
                    FD_SET(client_sock, &fds);
                } else {
                    memset(buffer, 0, sizeof(buffer));
                    result_read = read(fd, buffer, sizeof(buffer));
                    if (result_read <= 0) {
                        FD_CLR(fd, &fds);
                        close(fd);
                        printf("client(%d) closed\n", fd);
                        continue;
                    }
                    printf("client(%d): %s\n", fd, buffer);
                    result_write = write(fd, buffer, sizeof(buffer));
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

