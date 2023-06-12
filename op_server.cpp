/** \brief  回声服务器
*/

#include <stdio.h>      // printf
#include <stdlib.h>     //
#include <string.h>     // c风格字符串
#include <unistd.h>     // write, close, open
#include <arpa/inet.h>  // 
#include <sys/socket.h> // socket

#define BUF_SIZE 1024

void error_handling (char *msg);

int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;
    char msg[BUF_SIZE];
    int str_len;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // 调用bind函数分配IP地址和端口号
    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    // 调用listen函数将套接字转为可接受连接的状态
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    // 调用accept函数受理连接请求。如果在没有连接请求的情况下调用该函数，则不会返回，直到有连接请求为止
    for (int i = 0; i < 5; i++) {
        printf("服务器端开始 accept\n");
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
        printf("服务器端结束 accept\n");
        if (clnt_sock == -1) {
            error_handling("accept() error");
        } else {
            printf("Connected client %d \n", i + 1);
        }
        int op_cnt = 0;
        read(clnt_sock, &op_cnt, 1);
        printf("op_cnt = %d\n", op_cnt);
        printf("服务器端开始 read\n");
        // read函数只有收到EOF标志的时候才会返回0
        int recv_len = 0;
        while ((op_cnt * 4 + 1) > recv_len) {
            int recv_cnt = read(clnt_sock, &msg[recv_len], 4);
            printf("本次读取 recv_cnt = %d  , 读取的是 : %d \n", recv_cnt, msg[recv_len]);
            recv_len += recv_cnt;
        }
        printf("服务器端结束 read\n");
        char opt = msg[recv_len - 1];
        printf("opt = %c\n", msg[recv_len - 1]);
        for (int i = 0; i < op_cnt * 4 + 2; ++i) {
            printf("msg[%d] = %c", i, msg[i]);
        }
        int res = 0;
        switch (opt)
        {
        case '+':
            for (int i = 0; i < op_cnt; ++i) {
                res += (int)msg[i * 4];
            }
            break;
        case '-':
            for (int i = 0; i < op_cnt; ++i) {
                res -= (int)msg[i * 4];
            }
            break;
        case '*':
            for (int i = 0; i < op_cnt; ++i) {
                res *= (int)msg[i * 4];
            }
            break;
        default:
            break;
        }
        printf("服务器端开始 write\n");
        write(clnt_sock, (char *)&res, sizeof res);
        printf("服务器端结束 write\n");
        
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}