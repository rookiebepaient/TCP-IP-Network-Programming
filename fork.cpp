#include <stdio.h>
#include <unistd.h>

int global_val = 10;

int main(int argc, char *argv[]) {
    int local_val = 20;
    global_val++;
    local_val += 5;

    pid_t pid = fork();
    if (pid == 0) {
        global_val += 2;
        local_val += 2;
    } else {
        global_val -= 2;
        local_val -= 2;
    }

    if (pid == 0) {
        printf("Child Proc : [%d, %d] \n", global_val, local_val);
    } else {
        printf("Parent Proc : [%d, %d] \n", global_val, local_val);
    }

    return 0;
}