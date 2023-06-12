#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

// struct operation {
//     int op_cnt = 0;

// };

int main (int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_adrr;
    char msg[BUF_SIZE];
    int res;
    memset(msg, 0, sizeof(msg));
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

    int op_cnt = 0;
    while (1) {
        fputs("Operand count : ", stdout);
        // fgets(msg, BUF_SIZE, stdin);
        scanf("%d", &op_cnt);
        fflush(stdin);
        msg[0] = (char)op_cnt;
        if (op_cnt < 1) {
            printf("请输入大于1的数字\n");
            continue;
        }
        
        for (int i = 0; i < op_cnt; i++) {
            printf("Operand %d :", i + 1);
            scanf("%d", (int *) &(msg[i * 4 + 1]));
        }
        printf("Operator : ");
        fgetc(stdin);
        scanf("%c", &msg[op_cnt * 4 + 1]);
        
        int len = strlen(msg);
        int len2 = op_cnt * 4 + 2; 
        printf("strlen(msg) = %d", len);
        printf("op_cnt * 4 + 2 = %d", len2);
        printf("客户端开始 write\n");
        write(sock, msg, len2);
        printf("客户端结束 write\n");
        printf("客户端开始 read\n");
        read(sock, &res, 4);
        printf("Operation result : %d \n", res);
    }
    close(sock);

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}