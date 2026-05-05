#include <stdio.h>
#include <stdlib.h>

int main() {
    // Look for the custom environment variable
    char *db_url = getenv("DATABASE_URL");

    if(db_url == NULL) {
        printf("Child: DATABASE_URL not found! Defaulting to local.\n");
    } else {
        printf("Child: Connected to %s\n", db_url);
    }
    return 0;
}