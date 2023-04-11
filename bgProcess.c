//Darshil Patel
//3-22-23
//CIS 3207: Systems Programming and Operating System
//Project 2 : SHELL similar to tsch

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "tools.h"

void program_execution(char **args);
int wait_pid(pid_t pid, int *status, int options);

//Struct represents a background process. 
//Contains a process ID and a pointer to the next bg_process
typedef struct bg_process {
    pid_t pid;
    struct bg_process *next;

} bg_process;


bg_process *bg_processes = NULL;           //Global pointer to the head of the linked list

void add_bg_process(pid_t pid) {            //Adds new background process with given  PID to linked list.
    bg_process *new_process = malloc(sizeof(bg_process));
    new_process->pid = pid;
    new_process->next = bg_processes;
    bg_processes = new_process;
}

void remove_bg_process(pid_t pid) {         //Removes background process 
    bg_process **current = &bg_processes;
    while (*current) {
        bg_process *entry = *current;
        if (entry->pid == pid) {
            *current = entry->next;
            free(entry);
        }
        else {
            current = &entry->next;
        }
    }
}


bool is_background_command(char **args) {  // Checks if the last element in the args array is the "&" symbol, 
    int count = 0;                         // indicating a background command.
    while (args[count]) {
        count++;
    }

    if (count > 0 && strcmp(args[count - 1], "&") == 0) {
        args[count - 1] = NULL; // Remove '&' from the arguments
        return true;
    }

    return false;
}

//Executes given command in the background by forking a new child process, 
//running the program_execution function in the child process,
//and adding the child process ID to the linked list of background processes in the parent process.

void execute_command_bg(char **args) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        program_execution(args);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0) {
        // Parent process
        add_bg_process(pid);
        printf("Started background process with PID %d\n", pid);
    }
    else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

//Iterates through the linked list of background processes and checks if any of them have terminated
void clean_up_zombie_processes() {
    int status;
    while (bg_processes) {
        pid_t pid = waitpid(bg_processes->pid, &status, WNOHANG);
        if (pid > 0) {
            printf("Background process %d terminated\n", pid);
            remove_bg_process(pid);
        }
        else {
            break;
        }
    }
}


void clean_up_zombie_processes();

