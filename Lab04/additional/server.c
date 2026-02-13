/*
 * TCP Book Server
 * Implements an in-memory database for books.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9090
#define MAX_BOOKS 100
#define BUFFER_SIZE 4096

// Book Structure
typedef struct {
    char title[100];
    char author[100];
    int accession_number;
    int total_pages;
    char publisher[100];
    int is_deleted; // Flag to mark deletion (0 = active, 1 = deleted)
} Book;

Book database[MAX_BOOKS];
int book_count = 0;

// Function to add a dummy book for testing
void add_dummy_data() {
    strcpy(database[0].title, "The C Programming Language");
    strcpy(database[0].author, "Kernighan");
    database[0].accession_number = 1001;
    database[0].total_pages = 272;
    strcpy(database[0].publisher, "Prentice Hall");
    database[0].is_deleted = 0;

    strcpy(database[1].title, "Clean Code");
    strcpy(database[1].author, "Robert Martin");
    database[1].accession_number = 1002;
    database[1].total_pages = 464;
    strcpy(database[1].publisher, "Pearson");
    database[1].is_deleted = 0;
    
    book_count = 2;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int choice;

    add_dummy_data();

    // 1. Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Book Server listening on port %d...\n", PORT);

    // 4. Accept
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read Operation Choice
        int valread = read(new_socket, &choice, sizeof(int));
        if (valread <= 0) break;

        char response[BUFFER_SIZE] = {0};

        switch (choice) {
            case 1: // Insert
                {
                    Book new_book;
                    read(new_socket, &new_book, sizeof(Book));
                    if (book_count < MAX_BOOKS) {
                        new_book.is_deleted = 0;
                        database[book_count++] = new_book;
                        strcpy(response, "Book inserted successfully.");
                    } else {
                        strcpy(response, "Database full. Cannot insert.");
                    }
                    write(new_socket, response, strlen(response));
                }
                break;

            case 2: // Delete (by Accession Number)
                {
                    int acc_no;
                    read(new_socket, &acc_no, sizeof(int));
                    int found = 0;
                    for (int i = 0; i < book_count; i++) {
                        if (!database[i].is_deleted && database[i].accession_number == acc_no) {
                            database[i].is_deleted = 1; // Soft delete
                            found = 1;
                            break;
                        }
                    }
                    if (found) strcpy(response, "Book deleted successfully.");
                    else strcpy(response, "Book not found.");
                    write(new_socket, response, strlen(response));
                }
                break;

            case 3: // Display All
                {
                    char entry[500];
                    int count = 0;
                    strcpy(response, "--- Book List ---\n");
                    for (int i = 0; i < book_count; i++) {
                        if (!database[i].is_deleted) {
                            sprintf(entry, "Acc: %d | Title: %s | Author: %s | Pages: %d | Pub: %s\n",
                                    database[i].accession_number, database[i].title, 
                                    database[i].author, database[i].total_pages, database[i].publisher);
                            strcat(response, entry);
                            count++;
                        }
                    }
                    if (count == 0) strcpy(response, "No books available.");
                    write(new_socket, response, strlen(response));
                }
                break;

            case 4: // Search (Title or Author)
                {
                    int search_type; // 1=Title, 2=Author
                    char search_term[100];
                    read(new_socket, &search_type, sizeof(int));
                    read(new_socket, search_term, 100);

                    char entry[500];
                    int found_count = 0;
                    strcpy(response, "--- Search Results ---\n");
                    
                    for (int i = 0; i < book_count; i++) {
                        if (!database[i].is_deleted) {
                            int match = 0;
                            if (search_type == 1 && strstr(database[i].title, search_term) != NULL) match = 1;
                            if (search_type == 2 && strstr(database[i].author, search_term) != NULL) match = 1;

                            if (match) {
                                sprintf(entry, "Acc: %d | Title: %s | Author: %s\n",
                                        database[i].accession_number, database[i].title, database[i].author);
                                strcat(response, entry);
                                found_count++;
                            }
                        }
                    }
                    if (found_count == 0) strcpy(response, "No records found.");
                    write(new_socket, response, strlen(response));
                }
                break;

            case 5: // Exit
                printf("Client requested exit.\n");
                close(new_socket);
                close(server_fd);
                exit(0);
        }
    }
    return 0;
}