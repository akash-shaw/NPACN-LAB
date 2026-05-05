#include <stdio.h>
#include <unistd.h>

int main() {
    // Only these variables will exist in the child
    char *env[] = {
        "DATABASE_URL=postgres://user:pass@192.168.1.50:5432/mydb",
        "LOG_LEVEL=DEBUG",
        NULL 
    };

    printf("Parent: Launching the app with custom DB settings...\n");

    // Syntax: path, arg0, NULL, environment_array
    if (execle("./child_app", "child_app", NULL, env) == -1) {
        perror("Execution failed");
    }

    return 0;
}