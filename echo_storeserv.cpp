/** \brief 保存消息的回声服务器
 * 将回声客户端传输的字符串按序保存到文件中
*/

#include <stdio.h>      // printf
#include <stdlib.h>     //
#include <string.h>     // c风格字符串
#include <unistd.h>     // write, close, open
#include <arpa/inet.h>  // 
#include <sys/socket.h> // socket
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE 100

void read_childproc(int sig);
void error_handling (char *msg);

int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;
    char buf[BUF_SIZE];
    int str_len;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }
    // 防止僵尸进程
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    int state= sigaction(SIGCHLD, &act, 0);

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

    int fds[2];
    pipe(fds);
    pid_t pid = fork();
    if (pid == 0) {
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        for (int i = 0; i < 10; ++i) {
            int len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void *)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }
    
    while (1) {
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        // 生成客户端套接字
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            continue;
        } else {
            printf("new client connected...\n");
        }
        pid = fork(); // 执行完此语句后 父子进程分别带有1个受理客户端连接请求时创建的套接字文件描述符
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        if (pid == 0) { /* 子进程运行区域 */
            // 关闭的时服务器套接字，这是因为服务器套接字的文件描述符同样也传递到子进程
            close(serv_sock);
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
                // printf("服务器端开始 write\n");
                write(clnt_sock, buf, str_len);
                write(fds[1], buf ,str_len);
                // printf("服务器端结束 write\n");
            }
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        } else {
            // 通过accept函数创建的套接字文件描述符已复制给子进程，因此服务器端需要销毁自己拥有的文件描述符
            close(clnt_sock);
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

void read_childproc(int sig) {
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Remove proc id : %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}