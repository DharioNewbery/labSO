// 3.20
#include <stdio.h>
#include <stdlib.h>

#define MIN_PID 300
#define MAX_PID 5000
#define NUM_PIDS (MAX_PID - MIN_PID + 1)

#define BITMAP_SIZE ((NUM_PIDS + 7) / 8)

unsigned char *pid_map = NULL;

int allocate_map() {
    pid_map = (unsigned char *)calloc(BITMAP_SIZE, sizeof(unsigned char));
    
    if (pid_map == NULL) {
        return -1;
    }
    
    return 1;
}

int allocate_pid() {
    if (pid_map == NULL) {
        return -1;
    }

    for (int i = 0; i < NUM_PIDS; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;

        if (!(pid_map[byte_index] & (1 << bit_index))) {
            pid_map[byte_index] |= (1 << bit_index);
            
            return MIN_PID + i;
        }
    }

    return -1;
}

void release_pid(int pid) {

    if (pid_map == NULL || pid < MIN_PID || pid > MAX_PID) {
        return;
    }

    int i = pid - MIN_PID;
    int byte_index = i / 8;
    int bit_index = i % 8;

    pid_map[byte_index] &= ~(1 << bit_index);
}

