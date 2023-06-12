/** \brief  通过select函数实I/O复用回声服务器
*/

#include <stdio.h>      // printf
#include <stdlib.h>     //
#include <string.h>     // c风格字符串
#include <unistd.h>     // write, close, open
#include <arpa/inet.h>  // 
#include <sys/socket.h> // socket
#include <sys/time.h>
#include <sys/select.h>

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

    fd_set reads;
    FD_ZERO(&reads);
    // 传入select第二个参数的fd_set变量reads注册服器端套接字
    // 接受数据情况的监视对iang就包含了服器端套接字
    // 客户端的连接请求同样通过传输数据操作。服器端套接字中有接受的数据，就说明有连接请求
    FD_SET(serv_sock, &reads);
    int fd_mas = serv_sock;

    fd_set cpy_reads;
    timeval timeout;
    int fd_num;
    while (1) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        // 根据监视目的传递必要的参数
        if (-1 == (fd_num = select(fd_mas + 1, &cpy_reads, 0, 0, &timeout))) {
            break;
        } 
        if (0 == fd_num) {
            continue;
        }
        // select函数返回大于等于1的值时有后面的步骤
        // 调用accept函数受理连接请求。如果在没有连接请求的情况下调用该函数，则不会返回，直到有连接请求为止
        for (int i = 0; i < fd_mas + 1; i++) {
            // 查找有变化的（有接收数据的套接字）的件描述符
            if (FD_ISSET(i, &cpy_reads)) {
                // 发生状态变化时，验证服器端套接字是否有变化。如果是，受理连接请求
                if (i == serv_sock) {   // 连接请求
                    clnt_addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                    // 注册与客户端连接的套接字件描述符
                    FD_SET(clnt_sock, &reads);
                    if (fd_mas < clnt_sock) {
                        fd_mas = clnt_sock;
                    }
                    printf("Connected client %d \n", i + 1);
                } else {    // 读取数据
                    // 发生变化的套接字并非服器端套接字时，即有要接受的数据
                    // 此时还要确认接受的数据是字符串还是断开连接的EOF
                    str_len = read(i, msg, BUF_SIZE);
                    if (str_len == 0) {
                        // 接受的数据是EOF时要关闭套接字，并从reads中删除该套接字
                        FD_CLR(i ,&reads);
                        close(i);
                        printf("closed client : %d \n", i);
                    } else {
                        // 接受的数据是字符串时，回声
                        write(i, msg, str_len); // 回声数据
                    }
                }
            }
        }
    }

    
    close(serv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}