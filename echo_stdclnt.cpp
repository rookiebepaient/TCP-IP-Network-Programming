/**
 * @file echo_stdclnt.cpp
 * @author your name (you@domain.com)
 * @brief 使用标准IO函数的客户端
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        error_handling("connect() error");
    } else {
        puts("Conneced ......");
    }

    FILE *readfp = fdopen(sock, "r");
    FILE *writefp = fdopen(sock, "w");
    char msg[BUF_SIZE];
    while (1) {
        fputs("Input msg (Q to quit) : ", stdout);
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            break;
        }
        fputs(msg, writefp);
        fflush(writefp);
        // echo_client.cpp 中的客户端需要将接受的数据转换为字符串(数据的尾部插入0)
        // 但此处并没有这一过程。因为使用标准IO函数可以按字符串单位进行数据交换
        fgets(msg, BUF_SIZE, readfp);
        printf("Message from server : %s", msg);
    }
    fclose(writefp);
    fclose(readfp);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}