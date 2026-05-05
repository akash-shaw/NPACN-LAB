#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
    time_t now = time(NULL);
    char *t = ctime(&now);

    write(STDOUT_FILENO, t, strlen(t));
    return 0;
}
