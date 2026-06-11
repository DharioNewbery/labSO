#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"
#include "history.h"
 
int parse_command(char input[], char *args[]) {
    int i = 0;
    int background = 0;
    char *token = strtok(input, " \t");
 
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
 
    args[i] = NULL;
 
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        background = 1;
        args[i - 1] = NULL;
    }
 
    return background;
}
 
void execute_command(char *args[], int background) {
    pid_t pid;
 
    pid = fork();
 
    if (pid < 0) {
        printf("Erro ao criar processo-filho\n");
    }
    else if (pid == 0) {
        printf("Criando processo-filho [PID: %d]...\n", getpid());
        execvp(args[0], args);
        perror("Erro ao executar comando");
        exit(1);
    }
    else {
        printf("processo-filho [PID: %d] criado.\n", pid);
        if (!background) {
            wait(NULL);
        }
    }
}

int main(void) {
    char input[MAX_LINE];
    char input_copy[MAX_LINE];
    char command_to_execute[MAX_LINE];
    char *args[MAX_ARGS];
    int should_run = 1;
    int background;
 
    while (should_run) {
        printf("osh>");
        fflush(stdout);
 
        if (fgets(input, MAX_LINE, stdin) == NULL) {
            break;
        }
 
        input[strcspn(input, "\n")] = '\0';
 
        if (strlen(input) == 0) {
            continue;
        }
 
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }
 
        if (strcmp(input, "!!") == 0) {
            if (!get_last_command(command_to_execute)) {
                printf("Nenhum comando no histórico\n");
                continue;
            }
 
            printf("%s\n", command_to_execute);
            strcpy(input, command_to_execute);
        }
        else if (input[0] == '!' && input[1] != '\0') {
            int number = atoi(&input[1]);
 
            if (!get_command_by_number(number, command_to_execute)) {
                printf("Nenhum comando correspondente no histórico\n");
                continue;
            }
 
            printf("%s\n", command_to_execute);
            strcpy(input, command_to_execute);
        }
 
        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }
        add_history(input);
        strcpy(input_copy, input);
        background = parse_command(input_copy, args);
        if (args[0] != NULL) {
            execute_command(args, background);
        }
    }
    return 0;
}