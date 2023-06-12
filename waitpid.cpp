#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        sleep(15);
        return 24;
    } else {
        int status;
        while (!waitpid(-1, &status, WNOHANG)) {
            sleep(3);
            puts("sleep 3 sec.");
        }
        if (WIFEXITED(status)) {    //是否正常退出
                printf("Child send one: %d \n", WEXITSTATUS(status));  // 返回值是多少
            }
    }

    return 0;
}