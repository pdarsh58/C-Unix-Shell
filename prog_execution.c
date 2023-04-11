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
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include "tools.h"


//Count number of pipes in command-line
int count_pipes(char **args) {  
    int num_pipes = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            num_pipes++;
        }
    }
    return num_pipes;
}

//Prepare command to parse
//Code was provided
void prep_to_parse(char *get_line) {

    char _line[1000];

    strncpy(_line, get_line, sizeof(_line) - 1);

    _line[sizeof(_line) - 1] = '\0';

    char *line = strdup(_line);
    char **array = parse(line, " ");

    int num_pipes = count_pipes(array);
    if (num_pipes > 0) {
        execute_pipes(array, num_pipes);
    }
    else {
        program_execution(array);
    }

    //program_execution(array);

    /*
    if (array == NULL)
    {
      exit(1);
    }*/
    /*
    int i = 0;
    while (array[i] != NULL)
    {
      printf("%s", array[i++]);
    }*/
    free(array);
    free(line);
}

//Code was provided
char **parse(char *line, char *delim) {

    char **array = malloc(sizeof(char *));
    *array = NULL;
    int n = 0;
    char *buf = strtok(line, delim);
    if (buf == NULL) {
        free(array);
        array = NULL;
        return array;
    }
    while (buf != NULL) {
        char **temp = realloc(array, (n + 2) * sizeof(char *));
        if (temp == NULL) {
            free(array);
            array = NULL;
            return array;
        }
        array = temp;
        temp[n++] = buf;
        temp[n] = NULL;
        buf = strtok(NULL, delim);
    }
    return array;
}

//Search for an executable file with the given command name in the directories 
//listed in the PATH Environment Variable
char *search_executable(const char *command) {

    char *path = getenv("PATH"); //get value of the PATH environment variable using the getenv
    if (path == NULL) { //check if PATH is not NULL
        return NULL;
    }

    //Create copy of path string for later use needed for parsing the directories.
    char *path_copy = strdup(path); 
    char *saveptr;
    char *dir = strtok_r(path_copy, ":", &saveptr);

    //Loop keeps calling strtok_r to tokenize the path_copy string, and it looks 
    //for an executable file with the given command name
    while (dir)
    {
        char *full_path = malloc(strlen(dir) + strlen(command) + 2);
        strcpy(full_path, dir); //copy to full_path
        strcat(full_path, "/");
        strcat(full_path, command);

        //Checks if resulting full_path corresponds to existing executable file for execution
        //Access function is used to do this, and the X_OK argument specifies that the file must be executable. 
        //If an executable file is found, the function returns the full_path string.
        if (access(full_path, X_OK) == 0) {
            free(path_copy);   
            return full_path;
        }
        free(full_path);

        dir = strtok_r(NULL, ":", &saveptr);  //Restart while loop by calling strtok_r again
    }

    free(path_copy);
    return NULL;
}


//Executes external commands by taking in list of args
void program_execution(char **args) {

    //for echo
    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++) {               //checks if its echo command
            if (args[i][0] == '$') {                          //If so, it handles it separately by printing the arguments to the console. 
                char *environment_var = getenv(args[i] + 1);  //If an argument starts with $, it checks for an environment variable 
                if (environment_var) {                        //with the corresponding name and prints its value.
                    printf("%s ", environment_var);
                }
            }
            else {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
        return;
    }

    pid_t pid = fork();
    int status;


    //block of code runs in the child process. It handles input/output redirection by calling 
    //process_redirection function, which sets up file descriptors for input and output based 
    //on any < or > characters found in the argument list. 
    //If an executable path is found by calling search_executable function, it is executed using execv function. 
    //execv replaces the current process with a new process image, and if it succeeds, it never returns. 
    //If execv fails, an error message is printed to stderr, and the child process exits with failure status.
    if (pid == 0) {
        // Child process

        // Handle input/output redirection
        int input_fd = -1;
        int output_fd = -1;
        if (process_redirection(args, &input_fd, &output_fd) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (input_fd != -1)
        {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_fd != -1)
        {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        char *executable_path = NULL;

        // If the command starts with '/', use the provided full path
        if (args[0][0] == '/')
        {
            executable_path = args[0];
        }
        else
        {
            executable_path = search_executable(args[0]);
        }

        if (executable_path)
        {
            if (execv(executable_path, args) == -1)
            {
                fprintf(stderr, "Error 404 in executing '%s': %s\n", args[0], strerror(errno));
                exit(EXIT_FAILURE);
            }
            if (args[0][0] != '/')
            {
                free(executable_path);
            }
        }
        else
        {
            fprintf(stderr, "Error 404 '%s' cannot be found\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }

    else if (pid < 0)
    {
        perror("Error 404");
        exit(EXIT_FAILURE);
    }

    //Runs in the parent process. 
    //It waits for the child process to terminate using waitpid function.
    {
        // Parent process
        do
        {
            pid_t w = waitpid(pid, &status, WUNTRACED);
            if (w == -1)
            {
                perror("Error to get waid PID");
                exit(EXIT_FAILURE);
            } 
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

