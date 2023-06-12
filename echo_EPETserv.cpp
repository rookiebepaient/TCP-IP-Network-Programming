/**
 * @file echo_EPETserv.cpp
 * @author your name (you@domain.com)
 * @brief 边缘触发的优势在于 可以分离接受数据和处理数据的时间点
 * @version 0.1
 * @date 2023-04-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50

void setNonblockingMode(int fd);

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_adr = {};
    // memset(&serv_adr, 0 , sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = INADDR_ANY;
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr))) {
        error_handling("bind() error");
    }
    if (-1 == listen(serv_sock, 5)) {
        error_handling("listen() error");
    }

    int epfd = epoll_create1(0);
    // epoll_event *ep_events = (epoll_event *)malloc(sizeof(epoll_event)*EPOLL_SIZE);
    epoll_event *ep_events = new epoll_event;
    // std::unique_ptr<epoll_event> ep_evev

    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    sockaddr_in clnt_adr = {};
    char buf[BUF_SIZE] = {};
    while (1) {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (-1 == event_cnt) {
            puts("epoll_wait() error");
            break;
        }
        puts("return epoll_wait");
        for (int i = 0; i < event_cnt; ++i) {
            if (ep_events[i].data.fd == serv_sock) {
                socklen_t adr_sz = sizeof(clnt_adr);
                int clnt_sock = accept(serv_sock, (sockaddr *)&clnt_adr, &adr_sz);
                setNonblockingMode(clnt_sock);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d\n", clnt_sock);
            } else {
                while (1) {
                    int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if (0 == str_len) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, nullptr);
                        close(ep_events[i].data.fd);
                        printf("closed client: %d\n", ep_events[i].data.fd);
                        break;
                    } else if (0 > str_len) {
                        // read函数发现输入缓冲没有数据可读时返回-1，同时在errno中保存EAGAIN常量
                        // read函数返回-1，变量errno中的值为EAGAIN时，说明没有数据可读
                        if (EAGAIN == errno) {
                            break;
                        }
                    } else {
                        write(ep_events[i].data.fd, buf, str_len);
                    }
                }
                
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void setNonblockingMode(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}