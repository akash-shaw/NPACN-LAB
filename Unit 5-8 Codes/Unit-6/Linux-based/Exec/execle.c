#include <stdio.h>
#include <unistd.h>

int main() {
    // Define a custom environment
    // Each string must be in "KEY=VALUE" format
    char *env[] = {
        "USER_ROLE=admin",
        "APP_DEBUG=true",
        "PATH=/usr/bin:/bin",
        NULL // Environment array must also end in NULL
    };

    printf("Starting a new program with a custom environment...\n");

    /* Syntax: execle(path, arg0, arg1, ..., NULL, env_array);
       We'll use '/usr/bin/env' to print the environment and prove it worked.
    */
    if (execle("/usr/bin/env", "env", NULL, env) == -1) {
        perror("execle failed");
    }

    return 0;
}