#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[]) {
    fd_set reads;
    // 初始化reads变量
    FD_ZERO(&reads);
    // 将文件描述符0对应的位设置为1.需要监视标准输入的变化
    FD_SET(0, &reads);

    fd_set temps;
    struct timeval timeout;
    // timeout.tv_sec = 5;
    // timeout.tv_usec = 5000;

    int result;
    int str_len;
    char buf[BUF_SIZE];
    while (1) {
        // 将reads的内容复制到temps变量，因为之前讲过，调用select函数后，除发生变化的文件描述符对应位外，剩下的所有位将初始化为0
        // 为了记住初始值，必须经过这种复制过程
        temps = reads;
        // 初始化timeval的值
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // 调用select函数。如果有控制台输入数据，则返回大于0的整数，如果没有输入数据而引发超时，则返回0
        result = select(1, &temps, 0 , 0, &timeout);
        if (result == -1) {
            puts("select() error!");
            break;
        } else if (result == 0) {
            puts("Time-out!");
        } else {
            if (FD_ISSET(0, &temps)) {
                // 验证发生变化的文件描述符是为标准输入，若是，则从标准输入读取数据并向控制台输出
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("msg from console : %s", buf);
            }
        }
    }

    return 0;
}