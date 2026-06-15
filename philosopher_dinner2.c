#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define N 5

union semun { int val; };

void take(int sem, int f) {
    struct sembuf op = { f, -1, 0 };
    semop(sem, &op, 1);
}

void put(int sem, int f) {
    struct sembuf op = { f, +1, 0 };
    semop(sem, &op, 1);
}

void philosopher(int id, int sem) {
    int left = id, right = (id + 1) % N;
    int a = left < right ? left : right;
    int b = left < right ? right : left;

    for (int i = 0; i < 3; i++) {
        printf("Philosopher %d (pid %d): thinking\n", id, getpid());
        sleep(1);

        printf("Philosopher %d (pid %d): trying to pick up forks\n", id, getpid());
        take(sem, a);
        take(sem, b);

        printf("Philosopher %d (pid %d): eating\n", id, getpid());
        sleep(1);

        put(sem, a);
        put(sem, b);
        printf("Philosopher %d (pid %d): released forks\n", id, getpid());
    }
}

int main(void) {
    int sem = semget(IPC_PRIVATE, N, IPC_CREAT | 0666);
    union semun val = { .val = 1 };
    for (int i = 0; i < N; i++)
        semctl(sem, i, SETVAL, val);

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            philosopher(i, sem);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++)
        wait(NULL);

    semctl(sem, 0, IPC_RMID);
    return 0;
}