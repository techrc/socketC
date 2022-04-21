#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int serverSocket(int af, int type, int protocol, char *ip, int port);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int server_sock = serverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_sock, result_read, result_write;
    char buffer[512];
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
        printf("accept: %d\n", client_sock);

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            result_read = read(client_sock, buffer, sizeof(buffer)-1);
            printf("%d\n", result_read);
            if (result_read <= 0) {
                close(client_sock);
                printf("client(%d) closed\n", client_sock);
                break;
            }
            printf("client(%d): %s\n", client_sock, buffer);
            result_write = write(client_sock, buffer, sizeof(buffer));
            if (result_write < 0) {
                printf("server write failed\n");
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

