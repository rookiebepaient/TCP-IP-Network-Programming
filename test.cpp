#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024

int main() {
    char msg[BUF_SIZE];

    
    memset(msg, 0, sizeof(msg));
    msg[0] = 't';
    msg[1] = 'e';
    msg[2] = 's';
    msg[3] = 't';
    int size = sizeof(msg);
    int len = strlen(msg);
    printf("sizeof msg = %d \n", size);
    printf("strlen msg = %d ", len);
    printf("\n");
}