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
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include "tools.h"

#define MAX_ARGS 1024

//Handles input/output redirection in a shell program
int process_redirection(char **args, int *input_fd, int *output_fd) {

    //printf("process redirection works\n");

    //Function loops through the array of command arguments, looking for < and > characters 
    //which indicate input and output redirection respectively. 
    for (int i = 0; args[i] != NULL; i++) {

        //If < is found, the function attempts to open the specified input file and store the resulting 
        //file descriptor in input_fd.
        if (strcmp(args[i], "<") == 0) {

            if (args[i + 1] != NULL) {

                *input_fd = open(args[i + 1], O_RDONLY);

                if (*input_fd < 0) {
                    perror("Error 404 in opening input file");
                    return -1;
                }
                args[i] = NULL;
                args[i + 1] = NULL;
                i++;
            }
            else {
                fprintf(stderr, "Error 404 b/c no input file specified\n");
                return -1;
            }
        }

        //If > is found, the function attempts to open the specified output file and store the resulting 
        //file descriptor in output_fd.
        else if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] != NULL) {

                *output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (*output_fd < 0) {
                    perror("Error 404 in opening output file");
                    return -1;
                }
                args[i] = NULL;
                args[i + 1] = NULL;
                i++;
            }
            else {
                fprintf(stderr, "Error 404 b/c no output file specified\n");
                return -1;
            }
        }
    }
    return 0;
}


char **append_arg(char **args, char *arg) {

    int count = 0;
    while (args[count] != NULL) {  //Count # elements in array
        count++;
    }

    args = realloc(args, sizeof(char *) * (count + 2)); //allocate memory
    if (!args) {
        fprintf(stderr, "Error 404: couldnt allocate memory for arguments\n");
        exit(EXIT_FAILURE);
    }

    args[count] = arg;    //appends the string arg to the end of the args array 
    args[count + 1] = NULL; //Then, sets the next element to null.

    return args;
}

// I have tried to use from Recommended approaches to implement Pipes from Readme
// void execute_pipes(char **args, int num_pipes);
//implements the execution of multiple commands connected by pipes
void execute_pipes(char **args, int num_pipes) {

    int i, pipe_fds[2 * num_pipes]; //Array - file descriptors for the pipes.

    for (i = 0; i < num_pipes; i++) {    
                                                            //creates the required number of pipes 
        if (pipe(pipe_fds + i * 2) < 0) {                   //and increases the pipe_fds array.
            perror("Error 404; coudnt create pipe");
            exit(EXIT_FAILURE);
        }
    }

    //Create child processes and connect pipes
    int pid, status;
    int args_idx = 0;
    for (i = 0; i <= num_pipes; i++) {

        pid = fork();
        if (pid == 0) {
            if (i != 0) {  //current process is not first, redirect its standard input to read end of previous pipe using dup2()

                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);
            
            }
            if (i != num_pipes) {  //current process is not last, redirect its standard output to write end of current pipe using dup2()
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);
            
            }

            for (int j = 0; j < 2 * num_pipes; j++) {
                close(pipe_fds[j]);  //Close all pipe file descriptors
            
            }

            //Parse the command arguments and build a new cmd_args array.
            char **cmd_args = parse(args[args_idx], " ");
            for (int j = args_idx + 1; args[j] != NULL && strcmp(args[j], "|") != 0; j++) {
                cmd_args = append_arg(cmd_args, args[j]);
            }
            
            //Look for the executable using search_executable function.
            char *executable_path = search_executable(cmd_args[0]);

            if (executable_path) {
                extern char **environ;
                //Execute the command using execve() if the executable was found, otherwise print an error message and exit.
                if (execve(executable_path, cmd_args, environ) == -1) {
                    fprintf(stderr, "Error 404; couldnt execute '%s': %s\n", cmd_args[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                free(executable_path);
            }
            else {
                fprintf(stderr, "Error 404; not found command: %s\n", cmd_args[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0) {
            perror("Error 404: couldnt fork");
            exit(EXIT_FAILURE);
        }

        while (args[args_idx] != NULL && strcmp(args[args_idx], "|") != 0) {
            args_idx++;
        }               //Update the args_idx variable to point to the next command after the current pipe
        args_idx++;
    }

    for (i = 0; i < 2 * num_pipes; i++) {
        close(pipe_fds[i]);
    }

    for (i = 0; i <= num_pipes; i++) {
        wait(&status);
    }
}






