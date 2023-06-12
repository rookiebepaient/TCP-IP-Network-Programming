/** \brief 3.4 网络地址的初始化与分配
 * 
*/

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    char *addr1 = "1.2.3.4";
    // 一个字节能表示的最大整数为255，这是一个错误的IP地址
    char *addr2 = "1.2.3.256";
    
    unsigned long conv_addr = inet_addr(addr1);
    if (conv_addr == INADDR_NONE) {
        printf("error occured! \n");
    } else {
        printf("Network ordered interger addr: %#lx \n", conv_addr);
    }

    conv_addr = inet_addr(addr2);
    if (conv_addr == INADDR_NONE) {
        printf("error occured! \n");
    } else {
        printf("Network ordered interger addr: %#lx \n\n", conv_addr);
    }
    // unsigned short
    return 0;
}