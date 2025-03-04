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
        printf("** Connect server failed, return code is: %d\n", result_connect);
        return 1;
    }

    int result_write, result_read;
    char buffer[512];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf(">> Type message you want to send to server: ");
        scanf("%s", buffer);

        // write
        result_write = write(sock, buffer, sizeof(buffer)-1);
        if (result_write < 0) {
            printf("** Write to server failed, return code is: %d\n", result_write);
            break;
        }

        // read
        result_read = read(sock, buffer, sizeof(buffer));
        if (result_read <= 0) {
            printf("** Read from server failed, return code is: %d\n", result_read);
            break;
        }
        printf("<< Read from server: %s\n", buffer);
    }

    close(sock);

    return 0;
}
