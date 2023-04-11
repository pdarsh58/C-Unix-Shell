// Darshil Patel
// CIS 3207: Systems Programming and Operating System
// Project 2: Shell

Psuedocode for Shell:

    1) Start by creating the skeleton code for your shell with a launcher/dispatcher for built-in commands.
    
    2) Implement the help and exit built-in commands. The help command should display a list of built-in commands and their usage, and the exit command should exit the shell.
    
    3) Add a built-in pwd command that prints the current working directory to standard output using the getcwd syscall.
    
    4) Implement the cd command as a built-in command that takes one argument, a directory path, and changes the current working directory to that directory using the chdir syscall.
    
    5) If the user types a command that isn't a built-in command, display a message that the shell doesn't know how to execute programs.
    
    6) Modify the shell to execute external programs when they are entered into the shell.
    
    7) When the user enters a command, fork a child process using the fork syscall.
    
    8) In the child process, use one of the functions from the exec family (e.g. execlp, execv, etc.) to run the external program.
    
    9) In the parent process, wait until the child process completes using the waitpid syscall.
    
    10)Implement redirection of stdout and stdin to/from files using the open and dup2 syscalls. If the user enters a command with output redirection (e.g. [process] > [file]), open the file using the open syscall and use dup2 to redirect the stdout of the process to the file descriptor of the opened file. If the user enters a command with input redirection (e.g. [process] < [file]), open the file using the open syscall and use dup2 to redirect the stdin of the process to the file descriptor of the opened file.
    
    11) Implement piping between programs using the pipe and dup2 syscalls. If the user enters a command with piping (e.g. [process A] | [process B]), create n-1 pipes using the pipe syscall, where n is the number of processes in the pipeline. Then, use a for loop to fork n child processes, where each process i (aside from the first and last, which are edge cases) changes its stdin FD (0) to point to the read end of pipe i-1 and changes its stdout FD (1) to point to the write end of pipe i. Use the dup2 syscall to set these file descriptors accordingly. After changing its stdin and stdout, process i must close all of its pipe FDs. If these FDs are not closed, the process will hang. The shell process also needs to close all of the pipe FDs since it doesn't make use of them at all.
    Repeat steps 5-11 until the user exits the shell using the exit command.
