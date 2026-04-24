#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1

#define COMMAND_LIMIT_LENGTH 256
#define COMMAND_LIMIT_NUM_OF_ARG_LENGTH 256
#define DIRECTORY_LIMIT_PATH_LENGTH 1024
#define DEFAULT_PROMPT "mysh> "
#define DEFAULT_SEPARATOR "$"

typedef enum {
    COMMAND_TYPE_BUILTIN_CD,
    COMMAND_TYPE_BUILTIN_PWD,
    COMMAND_TYPE_BUILTIN_EXIT,
    COMMAND_TYPE_EXTERNAL
} command_type;

void dir_current_get(char* buffer, int buffer_size);
void io_input_get(char* buffer, int buffer_size);
int io_input_validate(char* buffer);
void io_print_str(const char* str);
void io_print_error(const char* str);
void io_print_prompt(const char* current_dir, const char* separator, const char* prompt);

void cmd_arguments_extract(char* user_input_buffer, char** arguments);
int cmd_arguments_validate(char** arguments);
void cmd_command_execute(char** arguments);
void cmd_command_execute_external(char** arguments);

void cmd_command_execute_bultin(char** arguments, command_type type);
void cmd_exit_execute(void);
void cmd_cd_execute(char** arguments);
void cmd_pwd_execute(void);

void run_shell(void);

int main() {
    run_shell();
    return EXIT_SUCCESS;
}

void run_shell(void) {
    char io_input_value[COMMAND_LIMIT_LENGTH];
    char dir_current_value[DIRECTORY_LIMIT_PATH_LENGTH];

    while (TRUE) {
        // 1. get current directory
        dir_current_get(dir_current_value, DIRECTORY_LIMIT_PATH_LENGTH);
        // 2. print prompt at current directory
        io_print_prompt(dir_current_value, DEFAULT_SEPARATOR, DEFAULT_PROMPT);
        // 3. get user input
        io_input_get(io_input_value, COMMAND_LIMIT_LENGTH);
        // 4. validate user input
        if (!io_input_validate(io_input_value)) {
            io_print_error("Invalid input");
            continue;
        }
        // 5. extract command
        char* arguments[COMMAND_LIMIT_NUM_OF_ARG_LENGTH];
        cmd_arguments_extract(io_input_value, arguments);
        // 6. validate arguments
        if (!cmd_arguments_validate(arguments)) {
            io_print_error("Invalid arguments for command");
            continue;
        }
        // 7. execute command
        cmd_command_execute(arguments);
    }
}

/* helpers implementations */
void dir_current_get(char* buffer, int buffer_size) {
    if (getcwd(buffer, buffer_size) != NULL) {
        return;
    }
    io_print_error("getcwd failed");
}

void io_input_get(char* buffer, int buffer_size) {
    if (fgets(buffer, buffer_size, stdin) != NULL) {
        return;
    }
    if (feof(stdin)) {
        // exit when user pressed ctrl + d
        cmd_exit_execute();
    } else {
        io_print_error("fgets failed");
    }
    buffer[0] = '\0';
}

int io_input_validate(char* buffer) {
    if (buffer == NULL || buffer[0] == '\0') {
        return FALSE;
    }

    char* new_line = strchr(buffer, '\n');  // search for newline

    if (new_line) {
        // strip the newline char by replacing it with end of string character
        *new_line = '\0';
    } else {
        // if input is too long, extra characters stay in stdin,
        // so we clear them to avoid breaking the next input
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return FALSE;
    }

    if (buffer[0] == '\0') {
        return TRUE;
    }

    return TRUE;
}

void io_print_str(const char* str) {
    if (str == NULL) return;
    fprintf(stdout, "%s\n", str);
    fflush(stdout);
}

void io_print_prompt(const char* current_dir, const char* separator, const char* prompt) {
    if (current_dir) {
        fputs(current_dir, stdout);
    } else {
        fputs("unknown", stdout);
    }
    fputs(separator, stdout);
    fputs(prompt, stdout);

    fflush(stdout);
}

void io_print_error(const char* str) {
    if (str == NULL) return;
    fprintf(stderr, "%s\n", str);
    fflush(stderr);
}

/* commands implementations */
void cmd_arguments_extract(char* user_input_buffer, char** arguments) {
    if (user_input_buffer == NULL || arguments == NULL) return;
    char* token;
    char* save_ptr;
    int i = 0;

    token = strtok_r(user_input_buffer, " \t\n\r", &save_ptr);
    while (token != NULL) {
        if (i < COMMAND_LIMIT_NUM_OF_ARG_LENGTH - 1) {
            arguments[i++] = token;
        } else {
            break;
        }
        token = strtok_r(NULL, " \t\n\r", &save_ptr);
    }
    arguments[i] = NULL;
}

int cmd_arguments_validate(char** arguments) {
    if (arguments == NULL || arguments[0] == NULL) {
        return TRUE;
    }
    int i = 0;
    while (arguments[i] != NULL) {
        i++;
    }
    if (i >= COMMAND_LIMIT_NUM_OF_ARG_LENGTH - 1) {
        return FALSE;
    }
    return TRUE;
}

void cmd_command_execute(char** arguments) {
    if (arguments == NULL || arguments[0] == NULL) {
        return;
    }

    command_type type = COMMAND_TYPE_EXTERNAL;
    if (strcmp(arguments[0], "cd") == 0) {
        type = COMMAND_TYPE_BUILTIN_CD;
    } else if (strcmp(arguments[0], "pwd") == 0) {
        type = COMMAND_TYPE_BUILTIN_PWD;
    } else if (strcmp(arguments[0], "exit") == 0) {
        type = COMMAND_TYPE_BUILTIN_EXIT;
    }

    if (type == COMMAND_TYPE_EXTERNAL) {
        cmd_command_execute_external(arguments);
    } else {
        cmd_command_execute_bultin(arguments, type);
    }
}

void cmd_command_execute_external(char** arguments) {
    if (arguments == NULL || arguments[0] == NULL) {
        return;
    }
    const int child_pid = fork();
    if (child_pid < 0) {
        io_print_error("Failed to create process");
    } else if (child_pid == 0) {
        if (execvp(arguments[0], arguments) == -1) {
            io_print_error("Failed to execute command");
        }
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(child_pid, &status, 0);
    }
}

void cmd_command_execute_bultin(char** arguments, command_type type) {
    if (arguments == NULL || arguments[0] == NULL) {
        return;
    }
    if (type == COMMAND_TYPE_BUILTIN_CD) {
        cmd_cd_execute(arguments);
    } else if (type == COMMAND_TYPE_BUILTIN_PWD) {
        cmd_pwd_execute();
    } else if (type == COMMAND_TYPE_BUILTIN_EXIT) {
        cmd_exit_execute();
    }
}

void cmd_exit_execute(void) { exit(EXIT_SUCCESS); }
void cmd_cd_execute(char** arguments) {
    char* target_path = arguments[1];

    // cd without arguments is equivalent to cd $HOME
    if (target_path == NULL) {
        target_path = getenv("HOME");

        if (target_path == NULL) {
            return;
        }
    }

    if (chdir(target_path) != 0) {
        io_print_error("Could not change directory");
    }
}

void cmd_pwd_execute(void) {
    char buffer[DIRECTORY_LIMIT_PATH_LENGTH];
    dir_current_get(buffer, DIRECTORY_LIMIT_PATH_LENGTH);
    io_print_str(buffer);
}
