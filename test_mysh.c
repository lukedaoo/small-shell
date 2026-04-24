#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* test framework */
typedef struct {
    int passed;
    int failed;
} test_suite;

/* mysh function declarations */
#define FALSE 0
#define TRUE 1
#define COMMAND_LIMIT_LENGTH 256
#define COMMAND_MAX_ARGS 256
#define DIRECTORY_LIMIT_PATH_LENGTH 1024

int io_input_validate(char* buffer);
int cmd_arguments_validate(char** arguments);
void cmd_arguments_extract(char* user_input_buffer, char** arguments);
void cmd_cd_execute(char** arguments);
void cmd_pwd_execute(void);
void cmd_exit_execute(void);

void test_print(const char* name, int passed) {
    if (passed) {
        printf("  PASS: %s\n", name);
    } else {
        printf("  FAIL: %s\n", name);
    }
}

void test_print_str(const char* name, int passed, const char* expected, const char* actual) {
    if (passed) {
        printf("  PASS: %s\n", name);
    } else {
        printf("  FAIL: %s\n", name);
        printf("        expected: %s\n", expected);
        printf("        actual:   %s\n", actual);
    }
}

void test_print_int(const char* name, int passed, int expected, int actual) {
    if (passed) {
        printf("  PASS: %s\n", name);
    } else {
        printf("  FAIL: %s\n", name);
        printf("        expected: %d\n", expected);
        printf("        actual:   %d\n", actual);
    }
}

/* ===== TEST GROUP 1: Newline stripping (io_input_validate) ===== */
void test_group_newline_stripping(test_suite* suite) {
    printf("\n=== TEST GROUP 1: Newline Stripping (io_input_validate) ===\n");

    char buffer[COMMAND_LIMIT_LENGTH];
    int result;
    int passed;

    strcpy(buffer, "ls\n");
    result = io_input_validate(buffer);
    passed = (result == TRUE && strcmp(buffer, "ls") == 0);
    test_print("'ls\\n' strips to 'ls', returns TRUE", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "ls -la\n");
    result = io_input_validate(buffer);
    passed = (result == TRUE && strcmp(buffer, "ls -la") == 0);
    test_print("'ls -la\\n' strips to 'ls -la', returns TRUE", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "\n");
    result = io_input_validate(buffer);
    passed = (result == TRUE && buffer[0] == '\0');
    test_print("'\\n' (only newline) strips to '', returns TRUE", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    buffer[0] = '\0';
    result = io_input_validate(buffer);
    passed = (result == FALSE);
    test_print_int("'' (empty string) returns FALSE", passed, 0, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    result = io_input_validate(NULL);
    passed = (result == FALSE);
    test_print_int("NULL buffer returns FALSE", passed, 0, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

/* ===== TEST GROUP 2: Argument parsing (cmd_arguments_extract) ===== */
void test_group_argument_parsing(test_suite* suite) {
    printf("\n=== TEST GROUP 2: Argument Parsing (cmd_arguments_extract) ===\n");

    char buffer[COMMAND_LIMIT_LENGTH];
    char* args[COMMAND_MAX_ARGS];
    int passed;

    strcpy(buffer, "ls");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] != NULL && strcmp(args[0], "ls") == 0 && args[1] == NULL);
    test_print("'ls' parses to args[0]='ls', args[1]=NULL", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "ls -la /tmp");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] != NULL && strcmp(args[0], "ls") == 0 && args[1] != NULL &&
              strcmp(args[1], "-la") == 0 && args[2] != NULL && strcmp(args[2], "/tmp") == 0 &&
              args[3] == NULL);
    test_print("'ls -la /tmp' parses correctly", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "ls  -la");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] != NULL && strcmp(args[0], "ls") == 0 && args[1] != NULL &&
              strcmp(args[1], "-la") == 0 && args[2] == NULL);
    test_print("'ls  -la' (multiple spaces) parses as single space", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "ls\t-la");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] != NULL && strcmp(args[0], "ls") == 0 && args[1] != NULL &&
              strcmp(args[1], "-la") == 0 && args[2] == NULL);
    test_print("'ls\\t-la' (tabs) parsed correctly", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "  ls  ");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] != NULL && strcmp(args[0], "ls") == 0 && args[1] == NULL);
    test_print("'  ls  ' (leading/trailing spaces) parses to 'ls'", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    strcpy(buffer, "");
    cmd_arguments_extract(buffer, args);
    passed = (args[0] == NULL);
    test_print("'' (empty string) -> args[0]=NULL", passed);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

/* ===== TEST GROUP 3: Argument validation (cmd_arguments_validate) ===== */
void test_group_argument_validation(test_suite* suite) {
    printf("\n=== TEST GROUP 3: Argument Validation (cmd_arguments_validate) ===\n");

    char* args[COMMAND_MAX_ARGS];
    int result;
    int passed;

    args[0] = "ls";
    args[1] = "-la";
    args[2] = NULL;
    result = cmd_arguments_validate(args);
    passed = (result == TRUE);
    test_print_int("valid args array returns TRUE", passed, 1, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    result = cmd_arguments_validate(NULL);
    passed = (result == TRUE);
    test_print_int("NULL returns TRUE", passed, 1, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    args[0] = NULL;
    result = cmd_arguments_validate(args);
    passed = (result == TRUE);
    test_print_int("args[0]=NULL returns TRUE", passed, 1, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    for (int i = 0; i < 256 - 2; i++) {
        args[i] = "arg";
    }
    args[256 - 2] = NULL;
    result = cmd_arguments_validate(args);
    passed = (result == TRUE);
    test_print_int("254 args returns TRUE", passed, 1, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    for (int i = 0; i < 256 - 1; i++) {
        args[i] = "arg";
    }
    args[256 - 1] = NULL;
    result = cmd_arguments_validate(args);
    passed = (result == FALSE);
    test_print_int("255 args returns FALSE", passed, 0, result);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

/* ===== TEST GROUP 4: Built-in: cd ===== */
void test_group_builtin_cd(test_suite* suite) {
    printf("\n=== TEST GROUP 4: Built-in: cd ===\n");

    char* args[COMMAND_MAX_ARGS];
    char cwd[DIRECTORY_LIMIT_PATH_LENGTH];
    char original_cwd[DIRECTORY_LIMIT_PATH_LENGTH];
    int passed;

    getcwd(original_cwd, sizeof(original_cwd));

    args[0] = "cd";
    args[1] = "/tmp";
    args[2] = NULL;
    cmd_cd_execute(args);
    getcwd(cwd, sizeof(cwd));
    passed = (strcmp(cwd, "/tmp") == 0);
    test_print_str("cd /tmp -> cwd is /tmp", passed, "/tmp", cwd);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    args[0] = "cd";
    args[1] = NULL;
    cmd_cd_execute(args);
    getcwd(cwd, sizeof(cwd));
    const char* home = getenv("HOME");
    if (home) {
        passed = (strcmp(cwd, home) == 0);
        test_print_str("cd (no arg) -> changes to $HOME", passed, home, cwd);
        if (passed) {
            suite->passed++;
        } else {
            suite->failed++;
        }
    }

    chdir("/tmp");
    args[0] = "cd";
    args[1] = "/nonexistent_path_12345";
    args[2] = NULL;
    cmd_cd_execute(args);
    getcwd(cwd, sizeof(cwd));
    passed = (strcmp(cwd, "/tmp") == 0);
    test_print_str("cd /nonexistent -> error, cwd unchanged", passed, "/tmp", cwd);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    chdir(original_cwd);
}

/* ===== TEST GROUP 5: Built-in: pwd ===== */
void test_group_builtin_pwd(test_suite* suite) {
    printf("\n=== TEST GROUP 5: Built-in: pwd ===\n");
    fflush(stdout);

    char expected[DIRECTORY_LIMIT_PATH_LENGTH];
    char actual[DIRECTORY_LIMIT_PATH_LENGTH];
    int passed;

    getcwd(expected, sizeof(expected));

    int pipefd[2];
    pipe(pipefd);
    const int saved_stdout = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    cmd_pwd_execute();
    fflush(stdout);

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    ssize_t n = read(pipefd[0], actual, sizeof(actual) - 1);
    close(pipefd[0]);
    if (n > 0 && actual[n - 1] == '\n') n--;
    actual[n] = '\0';

    passed = (strcmp(actual, expected) == 0);
    test_print_str("pwd output matches getcwd()", passed, expected, actual);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

/* ===== TEST GROUP 6: Error messages (stderr exact strings) ===== */
void io_print_error(const char* str);

void test_group_error_messages(test_suite* suite) {
    printf("\n=== TEST GROUP 6: Error Messages (stderr exact strings) ===\n");
    fflush(stderr);

    char actual[256];
    int passed;

    int pipefd[2];
    pipe(pipefd);
    const int saved_stderr = dup(STDERR_FILENO);

    /* test "Failed to create process\n" */
    dup2(pipefd[1], STDERR_FILENO);
    io_print_error("Failed to create process");
    fflush(stderr);
    dup2(saved_stderr, STDERR_FILENO);

    ssize_t n = read(pipefd[0], actual, sizeof(actual) - 1);
    actual[n] = '\0';

    passed = (strcmp(actual, "Failed to create process\n") == 0);
    test_print_str("fork fail msg exact: \"Failed to create process\\n\"", passed,
                   "Failed to create process\n", actual);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }

    /* test "Failed to execute command\n" */
    pipe(pipefd);
    dup2(pipefd[1], STDERR_FILENO);
    io_print_error("Failed to execute command");
    fflush(stderr);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);

    n = read(pipefd[0], actual, sizeof(actual) - 1);
    close(pipefd[0]);
    actual[n] = '\0';

    passed = (strcmp(actual, "Failed to execute command\n") == 0);
    test_print_str("execvp fail msg exact: \"Failed to execute command\\n\"", passed,
                   "Failed to execute command\n", actual);
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

/* ===== MAIN TEST RUNNER ===== */
int main() {
    test_suite suite = {0, 0};

    printf("\n========================================\n");
    printf("      mysh Unit Test Suite\n");
    printf("========================================\n");

    test_group_newline_stripping(&suite);
    test_group_argument_parsing(&suite);
    test_group_argument_validation(&suite);
    test_group_builtin_cd(&suite);
    test_group_builtin_pwd(&suite);
    test_group_error_messages(&suite);

    printf("\n========================================\n");
    printf("Test Results: %d passed, %d failed\n", suite.passed, suite.failed);
    printf("========================================\n\n");

    return (suite.failed > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
