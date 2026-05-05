#include <stdio.h>
#include <unistd.h>

int main() {
    char buffer[1024];

    read(STDIN_FILENO, buffer, sizeof(buffer));
    write(STDOUT_FILENO, buffer, sizeof(buffer));

    return 0;
}
