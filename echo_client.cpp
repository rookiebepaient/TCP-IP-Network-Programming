#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main (int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_adrr;
    char msg[BUF_SIZE];
    int str_len;
    int recv_len = 0, recv_cnt = 0;

    if (argc != 3) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }
    // 创建套接字，但此时套接字并不马上分为服务器端和客户端。如果紧接着调用bind、listen函数
    // 将成为服务器端套接字，如果调用connect函数，将成为客户端套接字
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_adrr, 0, sizeof(serv_adrr));
    serv_adrr.sin_family = AF_INET;
    serv_adrr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adrr.sin_port = htons(atoi(argv[2]));

    // 调用connect函数向服务器端发送连接请求
    if (connect(sock, (struct sockaddr *) &serv_adrr, sizeof(serv_adrr)) == -1) {
        error_handling("connect() error");
    } else {
        puts("Conneced ......");
    }

    while (1) {
        fputs("Input msg(Q to quit) : ", stdout);
        fgets(msg, BUF_SIZE, stdin);

        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            break;
        }
        printf("客户端开始 write\n");
        write(sock, msg, strlen(msg));
        printf("客户端结束 write\n");
        printf("客户端开始 read\n");
        while (recv_len < str_len) {
            recv_cnt = read(sock, msg, BUF_SIZE - 1);
            if (recv_cnt == - 1) {
                error_handling("read() error!");
            } else {
                printf("客户端结束 read\n");
                recv_len += recv_cnt;
            }
        }
        msg[str_len] = 0;
        printf("Message from server : %s", msg);
    }
    close(sock);

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}