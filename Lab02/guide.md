# Lab 02: Advanced Socket Programming

This guide covers the advanced socket programming exercises in Lab 02, including multiple exercises under subfolders `files` (File operations), `sort array` (Array sorting), and others.

---

## 1. File Manipulation Server (`files/`)

**Objective:** A TCP Client-Server system where the client sends a filename, and then can perform operations (Search word, Replace word, Reorder content) on that file stored on the server side.

### Client (`files/client.c`)
Sends the filename first, then enters a loop to send menu choices (1, 2, 3, 4) and associated data (strings to search/replace).

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // close, read, write
#include <string.h>     // strcmp, strcpy, memset
#include <sys/socket.h> // socket, connect, send, recv
#include <netinet/in.h> // sockaddr_in, AF_INET
#include <arpa/inet.h>  // inet_addr

#define MAX 1024        // Max buffer size for filenames and file content blocks

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char filename[MAX], buffer[MAX], str1[MAX], str2[MAX];

    // 1. Create Socket (TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 2. Setup Server Address (Localhost:3388)
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Connect failed"); 
        exit(1); 
    }

    // 4. Send Filename
    printf("Enter filename: ");
    scanf("%s", filename);
    send(sockfd, filename, MAX, 0); // Send filename to server

    // 5. Check if file exists
    recv(sockfd, buffer, MAX, 0);   // Wait for "FOUND" or "NOT_FOUND"
    if (strcmp(buffer, "NOT_FOUND") == 0) {
        printf("Server: File not present. Terminating.\n");
        close(sockfd); 
        return 0;
    }

    // 6. Menu Loop
    int choice;
    while (1) {
        printf("----MENU----");
        printf("\n1. Search\n2. Replace\n3. Reorder\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        
        // Send choice to server
        send(sockfd, &choice, sizeof(int), 0);

        if (choice == 4) break; // Exit loop

        if (choice == 1) { // Search
            printf("Enter string to search: ");
            scanf("%s", str1);
            send(sockfd, str1, MAX, 0); // Send search term
        } else if (choice == 2) { // Replace
            printf("Enter string to replace: "); scanf("%s", str1);
            printf("Enter new string: "); scanf("%s", str2);
            send(sockfd, str1, MAX, 0); // Send old string
            send(sockfd, str2, MAX, 0); // Send new string
        }
        // Case 3 (Reorder) requires no extra input, just waits for response
        
        // 7. Receive Operation Result
        memset(buffer, 0, MAX);
        recv(sockfd, buffer, MAX, 0);
        printf("Server Response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
```

### Server (`files/server.c`)
Receives the filename, checks existence, then processes menu commands endlessly until exit.

```c
#include <stdio.h>
// ... Includes ...
#define MAX 1024

// Helper Function: Sorts the characters inside the file
void sort_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp); // Go back to start
    
    // Read entire file into memory
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp); // Close read mode

    // Bubble Sort Characters (ASCII order)
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (buffer[i] > buffer[j]) {
                char t = buffer[i]; buffer[i] = buffer[j]; buffer[j] = t;
            }
        }
    }

    // Write back sorted content
    fp = fopen(filename, "w");
    fputs(buffer, fp);
    fclose(fp);
    free(buffer);
}

int main() {
    // ... Socket Variables ...
    int opt = 1;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Important: Allow port reuse so you can restart server immediately
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind & Listen (Port 3388)
    // ...
    listen(sockfd, 1);
    
    // 3. Accept Connection
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

    // 4. Receive Filename
    memset(filename, 0, MAX);
    recv(newsockfd, filename, MAX, 0);

    // 5. Check Existence
    if (access(filename, F_OK) == -1) { // F_OK tests existence
        send(newsockfd, "NOT_FOUND", MAX, 0);
        // ... close & exit ...
    }
    send(newsockfd, "FOUND", MAX, 0);

    // 6. Command Loop
    int choice;
    while (recv(newsockfd, &choice, sizeof(int), 0) > 0) {
        if (choice == 4) break;

        if (choice == 1) { // Search
            recv(newsockfd, str1, MAX, 0); // Get search term
            FILE *fp = fopen(filename, "r");
            int count = 0;
            // Scan file word by word
            while (fscanf(fp, "%s", buffer) != EOF) {
                if (strcmp(buffer, str1) == 0) count++;
            }
            fclose(fp);
            
            if (count > 0) sprintf(buffer, "Occurrences: %d", count);
            else strcpy(buffer, "String not found");
            
            send(newsockfd, buffer, MAX, 0); // Send result
        } 
        else if (choice == 2) { // Replace
            recv(newsockfd, str1, MAX, 0); // Old
            recv(newsockfd, str2, MAX, 0); // New
            
            FILE *fp = fopen(filename, "r");
            FILE *temp = fopen("temp.txt", "w"); // Temporary write file
            
            int found = 0;
            // Read word by word
            while (fscanf(fp, "%s", buffer) != EOF) {
                if (strcmp(buffer, str1) == 0) {
                    fprintf(temp, "%s ", str2); // Write NEW word
                    found = 1;
                } else {
                    fprintf(temp, "%s ", buffer); // Write ORIGINAL word
                }
            }
            fclose(fp); fclose(temp);
            
            // Replace original file with temp file logic usually goes here
            // (Note: Provide code doesn't explicitly rename, but usually: remove(filename); rename("temp.txt", filename);)
            if(found) strcpy(buffer, "Replaced successfully"); 
            else strcpy(buffer, "String not found");
            
            send(newsockfd, buffer, MAX, 0);
        }
        else if (choice == 3) { // Reorder
            sort_file(filename);
            strcpy(buffer, "File content reordered (ASCII sort)");
            send(newsockfd, buffer, MAX, 0);
        }
    }
    // ... Close ...
}
```

---

## 2. Array Sort Server (`sort array/`)

**Objective:** A simple TCP service where the client sends an array of integers, and the server returns the sorted array.

### Client (`sort array/client.c`)
Sends the size `n`, then sends the raw array `arr`, and waits to receive the sorted array back.

```c
// ... Includes ...
#define MAX 100

int main()
{
    // ... Socket Setup ...
    int n, arr[MAX];

    // ... Connect to 3388 ...

    // Get input
    printf("Enter number of elements: ");
    scanf("%d",&n);

    printf("Enter elements:\n");
    for(int i=0;i<n;i++) scanf("%d",&arr[i]);

    // Send Size
    send(sockfd, &n, sizeof(int), 0);
    
    // Send Array
    // Note: We send the entire memory block of the array at once
    // n * sizeof(int) calculates total bytes
    send(sockfd, arr, n * sizeof(int), 0);

    // Receive Sorted Array
    // Note: We overwrite our original array with the received data
    recv(sockfd, arr, n * sizeof(int), 0);

    printf("Sorted array:\n");
    for(int i=0;i<n;i++) printf("%d ",arr[i]);

    // ... Close ...
}
```

### Server (`sort array/server.c`)
Accepts connection, receives array, performs Bubble Sort, and sends it back.

```c
// ... Includes ...

// Helper: Bubble Sort
void sort(int a[], int n)
{
    // Standard bubble sort O(n^2)
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i] > a[j])
            {
                int t=a[i];
                a[i]=a[j];
                a[j]=t;
            }
}

int main()
{
    // ... Socket Setup ...
    // ... Bind to 3388 ...
    // ... Listen & Accept ...

    // 1. Receive Size
    recv(newsockfd, &n, sizeof(int), 0);
    
    // 2. Receive Array
    recv(newsockfd, arr, n * sizeof(int), 0);

    // 3. Process
    sort(arr, n);

    // 4. Send Back
    send(newsockfd, arr, n * sizeof(int), 0);

    close(newsockfd);
    close(sockfd);
    return 0;
}
```

---

## 3. Additional Exercises

These exercises deal with file statistics and advanced strings processing.

### Additional: TCP File Statistics (`additional/`)
**Objective:** Client sends a filename, Server returns file content **plus** stats like number of lines, spaces, digits, alphabets.

#### Client (`additional/tcp_client.c`)
Standard TCP Loop: Send Filename -> Receive Content -> Print.

#### Server (`additional/tcp_server.c`)
Key Logic Block:
1.  Open file `fopen(filename, "r")`.
2.  Read character by character `fgetc(fp)`.
3.  Use `ctype.h` functions:
    *   `isalpha(ch)`: Is it a letter?
    *   `isdigit(ch)`: Is it a number?
    *   `ch == ' '`: Space?
    *   `ch == '\n'`: New line?
4.  Construct a large string with stats + content using `sprintf` (or manual concat).
5.  Send back to client.

### Additional: UDP File Statistics (`additional/udp_server.c`)
Same logic as above, but using UDP.
*   Uses `recvfrom` to get filename.
*   Uses `sendto` to send back the large statistics string.
*   Note: UDP has a size limit (usually ~64KB max, realistically ~1400 bytes for safety over internet, but ~4KB locally is fine). The buffer here is 4096 bytes.

---

## 2. Exercise 1: Advanced File Menu (`quo1/`)

**Objective:** Similar to the `files/` example but with a specific menu protocol for string operations within files.

### Client (`quo1/tcp_client.c`)
Menu Driven Client:
1.  **Search:** Sends "1 [search_term]" - Search for a word in the file.
2.  **Replace:** Sends "2 [find_term] [replace_term]" - Replace specific words.
3.  **Reorder:** Sends "3" - Sort the file content alphabetically.
4.  **Exit:** Sends "4"

The client code uses `scanf` to gather these inputs and `send` to transmit filenames and operation codes.

---

## 3. Sort Array (`sort array/`)

**Objective:** A dedicated TCP program to sort an integer array.

### Client (`sort array/client.c`)
1.  Sends the number of elements `n`.
2.  Sends the raw array `arr` (`send(sockfd, arr, n * sizeof(int), 0)`).
3.  Waits to receive the sorted array back.
4.  Prints the result.

This demonstrates sending **binary data blocks** (arrays) rather than just text strings.

---

## 4. Additional: File Statistics (`additional/`)

**Objective:** Client sends a filename, Server returns file content **plus** stats like number of lines, spaces, digits, alphabets.

### Client (`additional/tcp_client.c`)
Standard TCP Loop: Send Filename -> Receive Content -> Print.

### Server (`additional/tcp_server.c`)
Key Logic Block:
1.  Open file `fopen(filename, "r")`.
2.  Read character by character `fgetc(fp)`.
3.  Use `ctype.h` functions:
    *   `isalpha(ch)`: Is it a letter?
    *   `isdigit(ch)`: Is it a number?
    *   `ch == ' '`: Space?
    *   `ch == '\n'`: New line?
4.  Construct a large string with stats + content using `sprintf` (or manual concat).
5.  Send back to client.

### UDP Server (`additional/udp_server.c`)
Same logic as above, but using UDP.
*   Uses `recvfrom` to get filename.
*   Uses `sendto` to send back the large statistics string.
*   Note: UDP has a size limit (usually ~64KB max, realistically ~1400 bytes for safety over internet, but ~4KB locally is fine). The buffer here is 4096 bytes.

---

## Detailed Variable & Function Explanation

### Variables
*   **`FILE *fp`**: A pointer to a file structure used by C to handle file I/O.
*   **`filename`**: A character array holding the name of the file to open (e.g., "data.txt").
*   **`access()`**: Determines if the calling process can access the file pathname. `F_OK` tests for existence.

### Functions
*   **`fopen(filename, mode)`**: Opens a file. Modes: `"r"` (read), `"w"` (write - erases existing!), `"a"` (append). Returns `NULL` on failure.
*   **`fclose(fp)`**: Closes the file. Always close files!
*   **`fseek(fp, offset, whence)`**: Moves the file pointer.
    *   `SEEK_SET`: Start of file.
    *   `SEEK_CUR`: Current position.
    *   `SEEK_END`: End of file.
*   **`ftell(fp)`**: Returns the current position in bytes from the start.
*   **`rewind(fp)`**: Sets the position to the beginning (same as `fseek(..., 0, SEEK_SET)`).
*   **`setsockopt(sock, level, optname, optval, optlen)`**: Configures socket behaviors. `SO_REUSEADDR` is the most common one used here.

### `send` vs `write` / `recv` vs `read`
*   **Difference:** On Linux/Unix, sockets are files, so `read`/`write` work fine. `send`/`recv` are specific to sockets and take an extra `flags` argument (usually 0).
*   **In these labs:** They are used interchangeably. `send(sock, buf, len, 0)` is strictly correct for sockets, but `write(sock, buf, len)` works too.

