#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handling(char *msg);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main (int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_adrr;
    

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

    char msg[BUF_SIZE];
    pid_t pid = fork();
    if (pid == 0) {
        // 只有数据输出相关代码
        write_routine(sock, msg);
    } else {
        // 只有数据输入相关代码
        read_routine(sock, msg);
    }

    close(sock);

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_routine(int sock, char *buf) {
    while (1) {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0) {
            return;
        }
        buf[str_len] = 0;
        printf("MSG from server : %s", buf);
    }
}

void write_routine(int sock, char *buf) {
    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            // 利用shutdown函数向服务器传递EOF。
            // 因为通过fork函数已经复制了一个文件描述符，此时一次close函数的调用不能传递EOF
            // 需要shutdown函数调用才能传递EOF
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}