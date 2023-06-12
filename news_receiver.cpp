/** \brief 多播接收方
 *  为了接受传向任意多播地址的数据，需要经过加入多播组的过程
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 5

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in adr;
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));

    if (bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1) {
        error_handling("bind() error");
    }

    struct ip_mreq join_adr;
    // 初始化结构体变量，初始化多播组地址，
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    // 初始化待加入主机的IP地址
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);
    // 利用套接字选项加入多播组，至此完成指定的多播组数据的所有准备
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr));

    int str_len;
    char buf[BUF_SIZE];
    while (1) {
        // 利用recefrom函数接受多播数据。如果不需要知道传输数据的主机地址信息，可以向recvfrom函数的第五个和第六个参数分别传递NULL和0
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0) {
            break;
        }
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(recv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}