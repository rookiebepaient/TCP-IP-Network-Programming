/** \brief 利用信号技术消灭僵尸程序
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig) {
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Remove proc id : %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

int main(int argc ,char *argv[]) {
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid = fork();
    if (pid == 0) {
        puts("Hi! Im child process");
        sleep(10);
        return 12;
    } else {
        printf("Child proc id : %d \n", pid);
        pid = fork();
        if (pid == 0) {
            puts("Hi! Im child process");
            sleep(10);
            return 24;
        } else {
            printf("Child proc id : %d \n", pid);
            // 为了等待发生SIGCHILD信号，使父进程共暂停5次，每次间隔5秒。发生信号时，父进程将被唤醒，因此实际暂停时间不到25秒
            for (int i = 0; i < 5; ++i) {
                puts("wait...");
                sleep(5);
            }
        }
    }

    return 0;
}