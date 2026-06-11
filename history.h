#include <stdio.h>
#include <string.h>
#include "config.h"

char history[HISTORY_SIZE][MAX_LINE];
int history_count = 0;
 
void add_history(char command[]) {
    int index = history_count % HISTORY_SIZE;
 
    strcpy(history[index], command);
    history_count++;
}
 
void show_history() {
    int start;
    int i;
    int index;
 
    if (history_count == 0) {
        return;
    }
 
    if (history_count < HISTORY_SIZE) {
        start = 1;
    } else {
        start = history_count - HISTORY_SIZE + 1;
    }
 
    for (i = history_count; i >= start; i--) {
        index = (i - 1) % HISTORY_SIZE;
        printf("%d %s\n", i, history[index]);
    }
}
 
int get_last_command(char command[]) {
    int index;
 
    if (history_count == 0) {
        return 0;
    }
 
    index = (history_count - 1) % HISTORY_SIZE;
    strcpy(command, history[index]);
 
    return 1;
}
 
int get_command_by_number(int number, char command[]) {
    int start;
    int index;
 
    if (history_count < HISTORY_SIZE) {
        start = 1;
    } else {
        start = history_count - HISTORY_SIZE + 1;
    }
 
    if (number < start || number > history_count) {
        return 0;
    }
 
    index = (number - 1) % HISTORY_SIZE;
    strcpy(command, history[index]);
 
    return 1;
}