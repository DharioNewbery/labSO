#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
 
#define MAX_LINE 80
#define MAX_ARGS (MAX_LINE / 2 + 1)
#define HISTORY_SIZE 10
 
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
        execvp(args[0], args);
        perror("Erro ao executar comando");
        exit(1);
    }
    else {
        printf("[Processo-filho criado com PID: %d]\n", pid);
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
 
        if (strcmp(input, "ajuda") == 0) {
            printf("--- Mini-Shell UNIX (Comandos Internos) ---\n");
            printf("exit    - Encerra o mini-shell\n");
            printf("history - Exibe o historico de comandos\n");
            printf("ajuda   - Exibe esta tela de ajuda\n");
            printf("!!      - Executa novamente o ultimo comando\n");
            printf("!N      - Executa o comando de numero N no historico\n");
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
            int idx = 1;
            int valido = 1;
            while (input[idx] != '\0') {
                if (input[idx] < '0' || input[idx] > '9') {
                    valido = 0;
                    break;
                }
                idx++;
            }
 
            if (!valido) {
                printf("Erro: Entrada invalida apos '!'. Use apenas numeros.\n");
                continue;
            }
 
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