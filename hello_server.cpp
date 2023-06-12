/** \brief  网络编程中接受连接请求的套接字创建过程
 *  什么是套接字?
 *      第一步：调用socket函数创建套接字
 *      第二步：调用bind函数分配IP地址和端口号
 *      第三步：调用listen函数转为可接受请求状态
 *      第四步：调用accept函数受理连接请求
 * 
 * 在Linux中，socket操作与文件操作没有区别。
 * socket被认为是文件的一种，因此在网络数据传输过程中自然可以使用文件I/O的相关函数
 * 在Windows中，是要区分socket和文件的，因此在Windows中需要调用特殊的数据传输相关函数
 * 
 * 底层文件访问(Low-Level File Access) 和文件描述符(File Descriptor)
 *      底层 意为“与标准无关的操作系统独立提供的”
*/

#include <stdio.h>      // printf
#include <stdlib.h>     //
#include <string.h>     // c风格字符串
#include <unistd.h>     // write, close, open
#include <arpa/inet.h>  // 
#include <sys/socket.h> // socket

void error_handling (char *msg);

int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char msg[] = "Hello World!";

    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // 调用socket函数创建套接字
    /**
     * @param[in] domain 使用的协议族的信息，套接字实际采用的最终协议信息是通过socket函数的第三个参数传递的，
     *              在指定的协议族范围内通过第一个参数决定第三个参数
     * @param[in] type 套接字数据传输类型信息，socket函数第一个参数PF_INET协议族中也存在多种数据传输方式
     * @param[in] protocol 计算机间通信中使用的协议信息
     *              在 同一协议族中存在多个数据传输方式相同的协议。即数据传输方式相同，但协议不同，需要通过第三个参数具体指定协议信息
    */
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
    clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }
    // write函数用传输数据，若程序经过54行执行到本行，则说明有了连接请求
    write(clnt_sock, msg, sizeof(msg));
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}