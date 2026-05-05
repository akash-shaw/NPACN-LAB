#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void run_demo_header(char* name) {
    // 1. Print the header
    printf("\n--- Testing %s ---", name);
    
    // 2. IMPORTANT: Flush stdout! 
    // This empties the buffer so the child doesn't "inherit" the header text.
    fflush(stdout); 
    printf("\n");
}

int main() {
    // --- 1. execl ---
    run_demo_header("execl");
    if (fork() == 0) {               // Child continues to exec
        execl("/bin/ls", "ls", "-lh", NULL);   //, "ad.c"
        exit(0);
    }
    wait(NULL);                     // Parent waits for child to finish

    // --- 2. execv ---
    run_demo_header("execv");
    if (fork() == 0) {
        char *args[] = {"/bin/ls", "-a", NULL};  //"client.c"
        execv(args[0], args);
        exit(0);
    }
    wait(NULL);

    // --- 3. execlp ---
    run_demo_header("execlp");
    if (fork() == 0) {
        execlp("ls", "ls", "-a", NULL);
        exit(0);
    }
    wait(NULL);

    // --- 4. execvp ---
    run_demo_header("execvp");
    if (fork() == 0) {
        char *args[] = {"ls", "-1", NULL};
        execvp("ls", args);
        exit(0);
    }
    wait(NULL);

    // --- 5. execle ---
    run_demo_header("execle");
    if (fork() == 0) {
        char *env[] = {"MY_VAR=HelloWorld", NULL};
        execle("/usr/bin/env", "env", NULL, env);
        exit(0);
    }
    wait(NULL);

    // --- 6. execve ---
    run_demo_header("execve");
    if (fork() == 0) {
        char *args[] = {"/usr/bin/env", NULL};
        char *env[] = {"USER=DemoUser", "SECRET=123", NULL};
        execve("/usr/bin/env", args, env);
        exit(0);
    }
    wait(NULL);

    printf("\n--- All demos complete ---\n");
    return 0;
}