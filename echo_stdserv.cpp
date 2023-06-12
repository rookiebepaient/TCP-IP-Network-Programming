/**
 * @file echo_stdserv.cpp
 * @author your name (you@domain.com)
 * @brief 使用标准IO函数改写的服务器
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>      // printf
#include <stdlib.h>     // atoi
#include <string.h>     // c风格字符串
#include <unistd.h>     // write, close, open
#include <arpa/inet.h>  // sockaddr_in
#include <sys/socket.h> // socket

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        error_handling("bind() error");
    } 
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);
    char msg[BUF_SIZE];
    for (int i = 0; i < 5; i++) {
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1) {
            error_handling("accept() error");
        } else {
            printf("Connected client %d \n", i + 1);
        }

        FILE *readfp = fdopen(clnt_sock, "r");
        FILE *writefp = fdopen(clnt_sock, "w");
        while (!feof(readfp)) {
            fgets(msg, BUF_SIZE, readfp);
            fputs(msg, writefp);
            // 不调用fflush函数无法保证立即将数据传输到客户端
            fflush(writefp);
        }
        fclose(readfp);
        fclose(writefp);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}