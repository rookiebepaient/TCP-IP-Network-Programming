/** \brief 实现多播sender和receiver
 *  多播中使用发送者sender和接收者receiver替代服务器端和客户端
 *  sender是多播数据的发送主体，receiver是需要多播组加入过程的数据接受主体
 *  sender：向AAA组广播文件中保存的新闻信息
 *  receiver：接受传递到AAA组的新闻信息
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 5

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }
    // 多播通信通过UDP完成的，创建UDP套接字
    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in mul_adr;
    memset(&mul_adr, 0, sizeof(mul_adr));
    // 设置传输数据的目标地址信息，必须将IP地址设置为多播地址
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]);   // 多播IP
    mul_adr.sin_port = htons(atoi(argv[2]));        // 多播端口

    int time_live = TTL;
    // 指定套接字TTL信息，这是sender中的必要过程
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));
    FILE *fp = NULL;
    if ((fp = fopen("news.txt", "r")) == NULL) {
        error_handling("fopen() error");
    }
    sleep(5);
    char buf[BUF_SIZE];
    // 基于UDP套接字传输数据，需要利用sendto函数
    while (!feof(fp)) { /* 广播 */
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));
        // 给传输数据提供一定的时间间隔，没有其他-特殊意义
        sleep(2);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}