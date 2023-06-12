#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>
#include <mutex>
#include <functional>

constexpr int BUF_SIZE = 100;
constexpr int MAX_CLNT = 256;

void handle_clnt(int clnt_sock);
void send_msg(char *msg, int len);
void error_handling(char *msg);

// using handle_clnt_fun = 
std::function<void (int)> handle_clnt_fun = handle_clnt;

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
// pthread_mutex_t mutx;

std::mutex g_mutx;

int main(int argc, char *argv[]) {
    if (2 != argc) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_adr{};
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    
    if (-1 == bind(serv_sock, reinterpret_cast<sockaddr *>(&serv_adr), sizeof(serv_adr))) {
        error_handling("bind() error");
    }
    if (-1 == listen(serv_sock, 5)) {
        error_handling("listen() error");
    }

    sockaddr_in clnt_adr{};
    // pthread_t t_id;
    while (true) {   
        socklen_t clnt_adr_sz = sizeof(clnt_adr);
        int clnt_sock = accept(serv_sock, reinterpret_cast<sockaddr *>(&clnt_adr), &clnt_adr_sz);

        // pthread_mutex_lock(&mutx);
        {    
            std::unique_lock<std::mutex> lock(g_mutx);
            clnt_socks[clnt_cnt++] = clnt_sock;
        }
        // pthread_mutex_unlock(&mutx);

        std::thread handle_clnt_thread(handle_clnt_fun, clnt_sock);
        // pthread_create(&t_id, nullptr, handle_clnt, (void *)&clnt_sock);
        handle_clnt_thread.detach();
        printf("Connected client IP: %s\n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void handle_clnt(int clnt_sock) {
    // int clnt_sock = *((int *)arg);
    char msg[BUF_SIZE] = {};
    int str_len = 0;
    
    while (0 != (str_len = read(clnt_sock, msg, sizeof(msg)))) {
        send_msg(msg, str_len);
    }
    // pthread_mutex_lock(&mutx);
    std::unique_lock<std::mutex> lock(g_mutx);
    for (int i = 0; i < clnt_cnt; ++i) {
        if (clnt_sock == clnt_socks[i]) {
            while (i++ < clnt_cnt - 1) {
                clnt_socks[i] = clnt_socks[i + 1];
            }
            break;
        }
    }
    clnt_cnt--;
    // pthread_mutex_unlock(&mutx);
    lock.unlock();
    close(clnt_sock);
}

void send_msg(char *msg, int len) {
    // pthread_mutex_lock(&mutx);
    std::unique_lock<std::mutex> lock(g_mutx);
    for (int i = 0; i < clnt_cnt; ++i) {
        write(clnt_socks[i], msg, len);
    }
    // pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}