Shell---similar-to-tsch-shell

Designing a shell similar to the tcsh shell requires implementing a variety of features, including basic command execution, built-in commands, parsing, path resolution, input/output redirection, pipes, and job control. My flow of doing things was same as it was given to us in assignment. Here is the design plan for my shell:


1) I first started by creating a raw skeleton of shell, Implement a set of built-in commands, such as cd, pwd, exit, and help.
Take input using darsh's shell > $$, then check if the command entered by the user is a built-in command. If so, execute it directly in the shell process. If not, give error.


2) Command execution:

For external commands, fork a new child process.
In the child process, search for the executable in the PATH environment variable.
Execute the program using the execv system call, passing the command-line arguments and environment variables.
I first execute by taking in Full Path of program, then modified to execute command without full paths


3) Input/output redirection:

When parsing input, look for input/output redirection symbols (> and <).
If found, use the redirect_input and redirect_output functions to change the standard input and output file descriptors to the specified files before executing the command.


4) Pipes
Kind of followed Recommended approach given in Readme.
When parsing input, look for pipe symbols (|).
If found, create an array of commands to be executed in sequence.
Use the pipe system call to create pipes for communication between the processes.
For each command, fork a new child process, connect the appropriate pipe ends to the standard input and output, close unused pipe ends, and execute the command.


5) Background jobs and job control

When parsing input, look for the background job symbol (&).
If found, execute the command in the background, add its process ID to a list of background processes, and return immediately to the shell prompt.
Implement built-in commands for job control, such as jobs, fg, and bg.
Periodically check for and clean up terminated background processes.


Implement proper error handling for system calls, such as fork, execv, and waitpid, and report errors
























