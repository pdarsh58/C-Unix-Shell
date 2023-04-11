//Darshil Patel
//3-22-23
//CIS 3207: Systems Programming and Operating System
//Project 2 : SHELL similar to tsch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#include "tools.h"

#define MAX_PATH 1024

void help();
void lets_exit();
void pwd();
void cd(char *path);
int wait_pid(pid_t pid, int *status, int options);

bool is_background_command(char **args);
void execute_command_bg(char **args);

int main() {

    printf("\nWelcome to Darsh's shell\nShell is similar to tsch shell\n\nFollowing are built-in commands\n");
    printf("pwd : prints current working directory\nexit : exits the shell\n");
    printf("cd : changes current working directory\nwait : waits for all background processes\n");
    printf("help : prints the manual\n\n");

    char command_line[MAX_PATH];  //array to hold input
    char *args[100];  //array of strings
   
    for (int i = 0; i < 100; i++) {

        printf("darsh_shell > $$ "); 
        fgets(command_line, MAX_PATH, stdin);   //read line of input, store it in command_line
        char *command_line_copy = strdup(command_line); //create copy of the command_line

        //printf("you comand_line before %s", command_line); 

        char *token = strtok(command_line, " \n"); //tokenize command input
        int arg_count = 0;

        //printf("you comand_line after %s", command_line); 

        //continue calling of strtok to tokenize the input command
        while (token != NULL) {
            args[arg_count++] = token;
            token = strtok(NULL, " \n");
        }
        args[arg_count] = NULL;

        if (args[0] == NULL) {
            continue;
        } 
        if (is_background_command(args))  { //for background process
            execute_command_bg(args);
        }
        else if (strcmp(args[0], "help") == 0) {
            help();
        }
        else if ((strcmp(args[0], "exit")) == 0) {
            lets_exit();
        }
        else if (strcmp(args[0], "pwd") == 0) {
            pwd();
        }
        else if (strcmp(args[0], "cd") == 0) {

            if (args[1] != NULL) {
                cd(args[1]);
            }
            else {
                printf("Error 404\n");
            }
        }

        //forks a child process to start processing and then wait for the child to finish.
        else if (strcmp(args[0], "wait") == 0) {  //for wait 

            pid_t child_pid = fork();

            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (child_pid == 0) {
                // Child process
                printf("Child   PID: %d   processing\n", getpid());
                sleep(2); // Simulate some work
                printf("Child   PID: %d   finishing\n", getpid());
            }
            else {
                // Parent process
                int status;
                if (waitpid(child_pid, &status, 0) == 0) {
                    printf("Parent PID: %d gets to know Child is done PID: %d).\n", getpid(), child_pid);
                }
                else {
                    printf("Error 404 PID: %d) to terminate.\n", child_pid);
                }
            }
        }
        else {
            command_line_copy[strcspn(command_line_copy, "\n")] = '\0';
            prep_to_parse(command_line_copy);            
        }
        free(command_line_copy);
    }

    return 0;
}

// Following are built-in commands

void help() {
    printf("\nYou can also use external commands if you need");
    printf("Following are built-in commands\n");
    printf("pwd : prints current working directory\nexit : exits the shell\n");
    printf("cd : changes current working directory\nwait : wait for all background processes to finish\n\n");
}

void lets_exit() {
    printf("Thank you for using my shell.\n");
    exit(0);
}

void pwd() {
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    }
    else {
        perror("getcwd() error");
    }
}

void cd(char *path) {
    if (chdir(path) != 0) {
        perror("chdir() error");
    }
}

//I have tried to follow TA slides guildlines to write wait_pid
//Waits for specific child to finish
//Checking its status every 1ms using waitpid() with the WNOHANG 
//It returns 0 when the child process has terminated and -1 if an error occurs.
int wait_pid(pid_t pid, int *status, int options) {

    int wait_status;
    pid_t child_pid;

    while (1) {
        child_pid = waitpid(pid, &wait_status, options | WNOHANG);

        if (child_pid == -1) {
            perror("waitpid");
            return -1;
        }

        if (child_pid == 0) {
            // The child process is still running
            usleep(1000); // Sleep for 1ms before checking again
        }
        else {
            // The child process has terminated
            if (status) {
                *status = wait_status;
            }
            return 0;
        }
    }
}





