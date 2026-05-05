#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

int main() {
    char buf[1024];
    int n = read(STDIN_FILENO, buf, sizeof(buf));

    for (int i = 0; i < n; i++)
        buf[i] = toupper(buf[i]);

    write(STDOUT_FILENO, buf, n);
    return 0;
}
