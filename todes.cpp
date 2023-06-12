/**
 * @file todes.cpp
 * @author Chen Youning (chen18249881210@163.com)
 * @brief fileno函数的调用实例
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <fcntl.h>

int main(void) {
    FILE *fp;
    int fd = open("data.dat", O_WRONLY, O_CREAT, O_TRUNC);
    if (fd == -1) {
        fputs("file open error", stdout);
        return -1;
    }
    // 输出打开文件的文件描述符
    printf("First file descriptor : %d\n", fd);
    // 将文件描述符转换为FILE指针
    fp = fdopen(fd, "w");
    fputs("TCP/IP SOCKET PROGRAMMING \n", fp);
    // 将FILE指针转换为文件描述符
    printf("Second file descriptor : %d\n", fileno(fp));
    fclose(fp);
    return 0;
}