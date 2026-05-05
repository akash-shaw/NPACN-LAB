#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } 
    else if (pid == 0) {
        // --- Inside the Child Process ---
        printf("Child: My PID is %d. Replacing my image with 'ls'...\n", getpid());
        
        char *args[] = {"ls", "-l", "/usr/bin", NULL};
        
        // execvp searches PATH for "ls" and takes the args vector
        if (execvp("ls", args) == -1) {
            perror("execvp failed");
        }
        
        // This line will NEVER execute if execvp is successful
        printf("This will not print.\n");
    } 
    else {
        // --- Inside the Parent Process ---
        printf("Parent: My PID is %d. Waiting for child %d to finish...\n", getpid(), pid);
        wait(NULL); // Wait for the child to finish before exiting
        printf("Parent: Child is done. Exiting.\n");
    }

    return 0;
}