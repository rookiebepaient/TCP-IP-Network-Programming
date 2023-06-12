#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
    // 通过客户端文件描述符创建读写模式的FILE指针
    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(dup(clnt_sock), "w");
    // 向客户端发送字符串，调用fflush函数结束发送过程
    fputs("FROM SERVER: Hi~ client?\n", writefp);
    fflush(writefp);

    char buf[BUF_SIZE];
    shutdown(fileno(writefp), SHUT_WR);
    // 此处如果没有经过复制文件描述符和半关闭写操作是完全关闭了套接字，而不是半关闭
    fclose(writefp);
    fgets(buf, sizeof(buf), writefp);
    fputs(buf, stdout);
    fclose(readfp);
    return 0;
}