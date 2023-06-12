#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *msg);

int main (int argc, char *argv[]) {
    int sock;
    /// @brief IPv4 地址的di结构体
    /// sin_familt  采用何种地址族，IP地址是多少，端口号是多少
    /// sin_port    保存16位端口号，以网络字节序保存
    /// sin_addr    保存32位IP地址信息，以网络字节序保存是一个32位整数
    /// sin_zero    无特殊含义，使结构体 sockaddr_in 和 sockaddr 结构体保持一致。必须填充为0
    struct sockaddr_in serv_adrr;
    char msg[30];
    int str_len;
    int idx = 0, read_len = 0;

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
    }

    // 反复调用read函数，每次读取一个字节
    while (read_len = read(sock, &msg[idx++], 1)) {
        if (str_len == -1) {
            error_handling("read() error");
        }
        str_len += read_len;
    }

    printf("MSG from server : %s \n", msg);
    printf("Function read call count : %d \n", str_len);
    close(sock);

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}