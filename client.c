#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(1234);

    // 无需bind，connect会自动绑定一个没有使用的本地端口
    int result_connect = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (result_connect < 0) {
        printf("result_connect: %d\n", result_connect);
        return 1;
    }

    char buffer[512];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("message to send: ");
        scanf("%s", buffer);
        write(sock, buffer, sizeof(buffer)-1);
        int result_read = read(sock, buffer, sizeof(buffer));
        if (result_read <= 0) {
            printf("client(%d) closed", sock);
            break;
        }
        printf("response: %s\n", buffer);
    }

    close(sock);

    return 0;
}
