#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

int serverSocket(int af, int type, int protocol, char *ip, int port);
int acceptSocket(int listen_sock);

int main() {
    char ip[] = "127.0.0.1";
    int port = 1234;
    int server_sock = serverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ip, port);

    int epfd = epoll_create(1);
    int EVENTS_SIZE = 20;
    struct epoll_event event, events[EVENTS_SIZE];
    event.data.fd = server_sock;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);
    int nfds = 0, timeout = -1;

    char buffer[512];
    while (1) {
        printf("epoll waiting...\n");
        nfds = epoll_wait(epfd, events, EVENTS_SIZE, timeout);
        for (int i = 0; i < nfds; i++) {
            if (events[i].events & EPOLLIN) {
                int fd = events[i].data.fd;
                if (fd == server_sock) {
                    int client_sock = acceptSocket(server_sock);

                    event.data.fd = client_sock;
                    event.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                } else {
                    memset(buffer, 0, sizeof(buffer));
                    int result_read = read(fd, buffer, sizeof(buffer));
                    if (result_read <= 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &events[i]);
                        close(fd);
                        printf("client(%d) closed\n", fd);
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

int acceptSocket(int listen_sock) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int conn_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    printf("accept: %d\n", conn_sock);

    return conn_sock;
}

