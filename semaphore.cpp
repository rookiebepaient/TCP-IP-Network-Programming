/**
 * @file semaphore.cpp
 * @author your name (you@domain.com)
 * @brief 线程A从用户输入得到值后存入全局变量num，此时线程B将取走该值并累加，该过程共进行5次，完成后输出总和并退出程序
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void *read(void *arg);
void *accu(void *arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[]) {
    pthread_t id_t1, id_t2;
    /**
     * @brief 创建信号量时传递保存信号量的变量地址，销毁时传递需要销毁的信号量变量地址
     * 第二参数 传递其他值时，创建可由多个进程共享的信号量，传递0时，创建只允许一个进程内部使用的信号量，
     * 第三参数 指定新创建的信号量初始值
     * 成功时返回0，失败时返回其他值
     */
    sem_init(&sem_one, 0 ,0);
    sem_init(&sem_two, 0 ,1);

    pthread_create(&id_t1, nullptr, read, nullptr);
    pthread_create(&id_t2, nullptr, accu, nullptr);

    pthread_join(id_t1, nullptr);
    pthread_join(id_t2, nullptr);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void *read(void *arg) {
    for (int i = 0; i < 5; ++i) {
        fputs("Input num: ", stdout);
        // 传递保存信号量读取值的变量地址值，传递给post时信号量增加1，传递给wait时信号量减1
        sem_wait(&sem_two);
        scanf("%d", &num);
        sem_post(&sem_one);
    }
    return nullptr;
}

void *accu(void *arg) {
    int sum = 0;
    for (int i = 0; i < 5; ++i) {
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result : %d\n", sum);
    return nullptr;
}