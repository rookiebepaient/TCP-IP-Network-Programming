#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    FILE *readfp = fdopen(sock, "r");
    FILE *writefp = fdopen(sock, "w");

    char buf[BUF_SIZE] = {0};
    while (1) {
        // 收到EOF后，fgets函数将返回NULL指针
        if (fgets(buf, sizeof(buf), readfp) == nullptr) {
            break;
        }
        fputs(buf, stdout);
        fflush(stdout);
    }

    fputs("FROM CLIENT: Thank you\n", writefp);
    fflush(writefp);
    fclose(writefp);
    fclose(readfp);
    return 0;
}