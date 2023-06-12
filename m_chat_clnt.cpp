#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <thread>

constexpr int BUF_SIZE = 100;
constexpr int NAME_SIZE = 20;

void send_msg(int sock);
int  recv_msg(int sock);
void error_handling(char *msg);

char name[NAME_SIZE] = {};
char msg[BUF_SIZE] = {};

int main(int arg, char *argv[]) {
    if (4 != arg) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    snprintf(name, sizeof(name),"[%s]", argv[3]);
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_adr{};
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (-1 == connect(sock, reinterpret_cast<sockaddr *>(&serv_adr), sizeof(serv_adr))) {
        error_handling("connect() error");
    }

    // pthread_t snd_thread, rcv_thread;
    // void *thread_return;
    std::thread send_thread(send_msg, sock);
    std::thread recv_thread(recv_msg, sock);
    // pthread_create(&snd_thread, nullptr, send_msg, (void *)&sock);
    // pthread_create(&rcv_thread, nullptr, recv_msg, (void *)&sock);
    // pthread_join(snd_thread, &thread_return);
    // pthread_join(rcv_thread, &thread_return);
    send_thread.join();
    recv_thread.join();
    close(sock);
    return 0;
}

void send_msg(int sock) {
    char name_msg[NAME_SIZE + BUF_SIZE] = {};
    while (true) {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            close(sock);
            exit(0);
        }
        snprintf(name_msg, sizeof(name_msg),"%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
}

int recv_msg(int sock) {
    char name_msg[NAME_SIZE + BUF_SIZE] = {};
    while (true) {
        int str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if (-1 == str_len) {
            return -1;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}