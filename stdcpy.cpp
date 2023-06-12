/** \brief 标准IO函数和系统函数之间的性能对比
*/

#include <stdio.h>

#define BUF_SIZE 3

int main(int argc, char *argv[]) {
    FILE *fp1 = fopen("./Data300m.txt", "r");
    FILE *fp2 = fopen("./cpy.txt", "w");

    int len;
    char buf[BUF_SIZE];
    while ((fgets(buf, BUF_SIZE, fp1)) > 0) {
        fputs(buf, fp2);
    }
    fclose(fp1);
    fclose(fp2);
    return 0;
}
