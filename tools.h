#ifndef _TOOLS_H_
#define _TOOLS_H_
#define LINE_MAX 4096 

void prep_to_parse(char *get_line);
char **parse(char *line, char *delim);
void program_execution(char **args);
char *search_executable(const char *command);
int process_redirection(char **args, int *input_fd, int *output_fd);
void execute_pipes(char **args, int num_pipes);
int count_pipes(char **args);

#endif
