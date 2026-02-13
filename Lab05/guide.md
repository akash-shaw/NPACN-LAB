# Lab 05: Concurrent vs Iterative Servers

This lab explores the difference between **Iterative Servers** (handling one client at a time) and **Concurrent Servers** (handling multiple clients simultaneously using `fork()`). It also introduces connection management (limiting the number of clients) and file operations within server processes.

## Exercise 1: File Appending Server (Concurrent with Limit)

This system involves a server that maintains a text file (`result.txt`). Clients send strings to be appended to this file. The server has a unique constraint: it only accepts 2 clients before determining the session is "full" and terminating itself.

### 1. Server Code (`quo1/server.c`)

**Initialization:**
```c
// Open file in 'w' mode to clear it and write initial text
fp = fopen("result.txt", "w");
fprintf(fp, "Manipal ");
fclose(fp);
client_count = 0;
```

**Client Limiting Logic:**
The server acts normally for the first 2 clients. For the 3rd, it rejects the connection and shuts down.
```c
while (1) {
    newsockfd = accept(sockfd, ...);
    client_count++;

    // Check if limit exceeded
    if (client_count > 2) {
        strcpy(buffer, "terminate session");
        write(newsockfd, buffer, ...);
        
        // Close everything and exit
        close(newsockfd);
        close(sockfd); 
        exit(0); 
    }

    // Normal handling for clients 1 and 2
    if (fork() == 0) {
        // Child Process
        read(newsockfd, buffer, ...);
        
        // Critical Section: Writing to file
        // In a real scenario, you might need file locking (flock) here
        // to prevent race conditions between children.
        fp = fopen("result.txt", "a"); // 'a' = append mode
        fprintf(fp, "%s ", buffer);
        fclose(fp);
        
        // ... Send response ...
        exit(0);
    }
}
```

### 2. Client Code (`quo1/client.c`)
Standard TCP client. It includes a check for the server's specific termination message.
```c
write(sockfd, input, ...);
read(sockfd, buffer, ...);

if (strstr(buffer, "terminate session") != NULL) {
    printf("Server sent: %s. Exiting.\n", buffer);
}
```

---

## Exercise 2: Anagram Checker (Iterative vs Concurrent)

The goal is to verify if two strings are anagrams (contain the same characters in different orders). You implemented both an Iterative Server (one client at a time) and a Concurrent Server (all clients at once).

### 1. The Anagram Logic (Shared)
Both servers use the same logic:
1.  Receive String 1 and String 2.
2.  Sort both strings alphabetically.
3.  Compare sorted strings using `strcmp`.

```c
void sort_string(char *str) {
    // Bubble sort or similar to arrange chars: "hello" -> "ehllo"
}
```

### 2. Iterative Server (`quo2/iterative_server.c`)
This server **blocks** on `accept`, handles one client's **entire conversation**, and only then goes back to `accept` the next one. If Client A is connected, Client B must wait in the queue.

```c
while (1) {
    // 1. Wait for a client
    new_socket = accept(...); 

    // 2. Process ALL requests for this client
    // The server is stuck inside this loop until the client disconnects!
    while (1) {
        valread = read(new_socket, buffer, ...); // Read Str1
        if (valread <= 0) break; // Client disconnected
        
        // ... Read Str2, Check Anagram, Send Result ...
    }
    
    // 3. Client finished, close socket
    close(new_socket);
    
    // 4. Loop back to accept the next client
}
```

### 3. Concurrent Server (`quo2/concurrent_server.c`)
This server **forks** immediately after `accept`. The parent goes back to listening, while the child handles the conversation. Client B can connect even while Client A is still sending data.

```c
while (1) {
    // 1. Wait for a client
    new_socket = accept(...);

    // 2. Fork immediately
    if (fork() == 0) {
        // --- Child Process ---
        close(server_fd); // Don't need the listener
        
        // Handle client's entire conversation here
        while (1) {
            // ... Read Str1, Read Str2, Check Anagram ...
        }
    }
    
    // --- Parent Process ---
    close(new_socket); // Handed off to child, close my copy
    // Loop back immediately to accept next client
}
```

### 4. Timestamp Logging
Both servers use `time.h` to log when a client connects.
```c
#include <time.h>

time_t rawtime;
struct tm *timeinfo;

time(&rawtime);             // Get current time
timeinfo = localtime(&rawtime); // Convert to struct tm
printf("Connected at: %s", asctime(timeinfo)); // Print readable string
```

## 4. Glossary: New Variables & Functions

### **Variables**
- **`FILE *`**
  - **Type**: Pointer to a `FILE` structure.
  - **Purpose**: A "handle" used to identify a file stream for reading/writing.
- **`time_t`**
  - **Type**: Arithmetic type (usually long integer).
  - **Purpose**: Stores system time as seconds since Epoch (Jan 1, 1970).
- **`struct tm`**
  - **Type**: Structure.
  - **Purpose**: Holds broken-down time components (tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, etc.).

### **Functions**
- **`fopen(filename, mode)`**
  - **Library**: `<stdio.h>`.
  - **Purpose**: Opens a file. Modes: `"r"` (read), `"w"` (write/truncate), `"a"` (append). Returns `NULL` on failure.
- **`fprintf(file_ptr, format, ...)`**
  - **Library**: `<stdio.h>`.
  - **Purpose**: Writes formatted output to a file stream (like `printf` but to a file).
- **`fclose(file_ptr)`**
  - **Library**: `<stdio.h>`.
  - **Purpose**: Closes a file stream and flushes any buffered data.
- **`time(&t)`**
  - **Library**: `<time.h>`.
  - **Purpose**: Stores the current calendar time in `t`.
- **`localtime(&t)`**
  - **Library**: `<time.h>`.
  - **Purpose**: Converts a `time_t` value to a `struct tm` in local time.
- **`asctime(tm_struct)`**
  - **Library**: `<time.h>`.
  - **Purpose**: Converts a `struct tm` to a human-readable string (e.g., "Mon Jan 01...").

