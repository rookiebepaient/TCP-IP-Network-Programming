#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[]) {
    int fds[2];
    // 调用pipe函数创建管道，fds数组中保存用于I/O的文件描述符
    pipe(fds);
    // 调用fork函数，子进程将同时拥有通过pipe(fds)调用获取的2个文件描述符
    // 复制的并非是管道，而是用于管道I/O的文件描述符
    pid_t pid = fork();
    char buf[BUF_SIZE];
    if (pid == 0) {
        // 子进程向管道传递字符串
        char str[] = "Who are you?";
        write(fds[1], str, sizeof(str));
        // sleep(2);
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output : %s \n", buf);
    } else {
        // 父进程从管道接受字符串
        read(fds[0], buf ,BUF_SIZE);
        printf("Parent proc output : %s \n", buf);
        char str2[] = "Thk u 4 ur msg";
        write(fds[1], str2, sizeof(str2));
        sleep(3);
    }

    return 0;
}