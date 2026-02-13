/*
 * TCP Book Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 4096

// Book Structure (Must match server)
typedef struct {
    char title[100];
    char author[100];
    int accession_number;
    int total_pages;
    char publisher[100];
    int is_deleted;
} Book;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    int choice;

    // 1. Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to Book Server.\n");

    while (1) {
        printf("\n--- LIBRARY MENU ---\n");
        printf("1. Insert New Book\n");
        printf("2. Delete Book (by Accession No)\n");
        printf("3. Display All Books\n");
        printf("4. Search Book\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        // Send Choice
        write(sock, &choice, sizeof(int));

        if (choice == 5) {
            printf("Exiting application.\n");
            break;
        }

        switch (choice) {
            case 1: // Insert
                {
                    Book b;
                    printf("Enter Title: ");
                    scanf(" %[^\n]", b.title);
                    printf("Enter Author: ");
                    scanf(" %[^\n]", b.author);
                    printf("Enter Accession No: ");
                    scanf("%d", &b.accession_number);
                    printf("Enter Total Pages: ");
                    scanf("%d", &b.total_pages);
                    printf("Enter Publisher: ");
                    scanf(" %[^\n]", b.publisher);
                    
                    write(sock, &b, sizeof(Book));
                }
                break;

            case 2: // Delete
                {
                    int acc;
                    printf("Enter Accession Number to delete: ");
                    scanf("%d", &acc);
                    write(sock, &acc, sizeof(int));
                }
                break;

            case 3: // Display
                // No extra data needed, just wait for response
                break;

            case 4: // Search
                {
                    int type;
                    char term[100];
                    printf("Search by: 1. Title  2. Author: ");
                    scanf("%d", &type);
                    getchar(); // consume newline
                    printf("Enter Search Term: ");
                    scanf(" %[^\n]", term);

                    write(sock, &type, sizeof(int));
                    write(sock, term, 100);
                }
                break;
        }

        // Receive Response
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("\nServer Reply:\n%s\n", buffer);
    }

    close(sock);
    return 0;
}