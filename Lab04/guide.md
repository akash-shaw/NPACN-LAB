# Lab 04: Advanced TCP/UDP & Concurrent Servers

This lab focuses on building more complex client-server applications. You'll move beyond simple message echoing to simulating real-world scenarios like database operations, student information systems, and DNS lookups. Key concepts include managing data structures over sockets and handling multiple clients using concurrent processes (`fork()`).

## Additional Exercise: Book Database (TCP)

This program simulates a library system where a client can add, delete, and search for books stored on the server. The server maintains an in-memory "database" (an array of structs).

### 1. The Book Structure (Header/Common)

Both client and server must agree on the format of the data being sent.
```c
typedef struct {
    char title[100];
    char author[100];
    int accession_number;
    int total_pages;
    char publisher[100];
    int is_deleted; // 0 = Active, 1 = Deleted
} Book;
```
*   **Struct Padding:** Sending structs directly over sockets (`write(sock, &myBook, sizeof(Book))`) is convenient but relies on both machines having the same architecture (padding/endianness). For this lab (localhost), it works fine.

### 2. Client Code (`additional/client.c`)

The client presents a menu to the user and sends operation codes and data to the server.

**Menu Loop:**
```c
while (1) {
    // Display Menu (Insert, Delete, Display, Search)
    scanf("%d", &choice);
    write(sock, &choice, sizeof(int)); // Tell server what operation to perform
```

**Sending a Structure (Insert):**
```c
    case 1: // Insert
        Book b;
        // ... capture user input into 'b' ...
        write(sock, &b, sizeof(Book)); // Send the entire struct in one go
        break;
```
*   **`write(..., &b, sizeof(Book))`**: This sends the raw memory bytes of the structure. It's efficient for C-to-C communication on the same machine.

### 3. Server Code (`additional/server.c`)

The server keeps an array of `Book` structs and performs operations based on the client's choice.

**Database Initialization:**
```c
Book database[MAX_BOOKS]; // Global array acting as database
// add_dummy_data() fills this with some starter books
```

**Handling Requests:**
```c
while (1) {
    int choice;
    read(new_socket, &choice, sizeof(int)); // Wait for command

    switch (choice) {
        case 1: // Insert
            Book new_book;
            read(new_socket, &new_book, sizeof(Book)); // Read the full struct
            database[book_count] = new_book;           // Save to array
            book_count++;
            break;
            
        case 2: // Delete
             // ... read accession number, find in array, set is_deleted = 1 ...
             break;
    }
}
```

---

## Exercise 1: Student Info System (TCP Concurrent)

This exercise requires a server that can handle different types of queries (Registration No, Name, Subject Code). The server must be **concurrent**, meaning it uses `fork()` to handle the actual processing, allowing the parent process to keep listening for new connections.

### 1. Client Code (`quo1/tcp/client.c`)

The client connects, sends a request, waits for a reply, and then **closes** the connection. It re-connects for the next request. This is a "Short-lived Connection" model.

```c
while (1) {
    // 1. Create & Connect inside the loop
    sockfd = socket(...);
    connect(...);

    // 2. Send Option & Data
    write(sockfd, &option, sizeof(int));
    write(sockfd, input_data, ...);

    // 3. Receive & Print
    read(sockfd, response, ...);
    
    // 4. Close immediately
    close(sockfd); 
}
```

### 2. Server Code (`quo1/tcp/server.c`)

The server reads the request details first, then forks a child to process it.

**The Concurrent Logic:**
```c
while (1) {
    // 1. Accept Connection
    newsockfd = accept(sockfd, ...);

    // 2. Read Request (Option + Data)
    read(newsockfd, &option, sizeof(int));
    read(newsockfd, buffer, ...);

    // 3. Fork
    int pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS ---
        close(sockfd); // Child doesn't need the main listener
        
        // Process logic based on 'option'
        if (option == 1) handle_reg_no(newsockfd, buffer);
        else if (option == 2) handle_student_name(newsockfd, buffer);
        
        close(newsockfd); // Finish conversation
        exit(0);          // Child dies
    }
    else {
        // --- PARENT PROCESS ---
        close(newsockfd); // Parent doesn't need this specific client connection
        // Loops back to 'accept' immediately
    }
}
```
*   **Why fork?** If `handle_reg_no` took 10 seconds to run, a single-process server would be blocked. With `fork`, the child waits within `handle_reg_no`, while the parent immediately goes back to `accept` to serve other students.

---

## Exercise 1: Student Info System (UDP)

The same logic as above, but using UDP. UDP is connectionless, so there is no `connect` or `accept`.

### 1. Client Code (`quo1/udp/client.c`)
```c
// Define server address
serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

// Send Request
struct Request req = {option, "Sort Data"};
sendto(sockfd, &req, sizeof(req), ..., (struct sockaddr *)&serv_addr, ...);

// Receive Response
recvfrom(sockfd, response, ...);
```

### 2. Server Code (`quo1/udp/server.c`)
Even in UDP, we can use `fork` to offload processing, though it's less common than in TCP since individual UDP packets are usually self-contained tasks.

```c
while (1) {
    // 1. Receive Packet (Blocking)
    recvfrom(sockfd, &req, sizeof(req), ..., &cli_addr, &len);

    // 2. Fork to handle response
    if (fork() == 0) {
        // Child calculates response
        if (req.option == 1) ...
        
        // Child sends response back to specific client address
        sendto(sockfd, response, ..., (struct sockaddr *)&cli_addr, len);
        
        exit(0);
    }
}
```
*   **Note:** In this specific code, the parent continues listening immediately. Be careful with UDP concurrency; if multiple children try to write to the same `sockfd` at perfectly the same time, it is generally safe (atomic packet writes), but logical ordering requires care.

---

## Exercise 2: DNS Server (TCP)

A simulation of a Domain Name System (DNS). The client sends a hostname (e.g., `www.google.com`), and the server looks it up in a database and returns the IP address.

### 1. Client Code (`quo2 dns/client.c`)
Standard TCP client.
*   **Connection:** Connects once and keeps the session open (`while(1)` loop over `write` and `read`).
*   **Input:** Uses `fgets` to read a string, and `strcspn` to strip the newline character `\n`.

### 2. Server Code (`quo2 dns/server.c`)

**Database Lookup:**
```c
struct DNS_Record {
    char hostname[50];
    char ip[20];
};

struct DNS_Record database[] = {
    {"www.google.com", "142.250.183.68"},
    ...
};

char* search_dns(char *host) {
    // Linear search through the array
    for (int i = 0; i < db_size; i++) {
        if (strcmp(database[i].hostname, host) == 0) return database[i].ip;
    }
    return "Host not found";
}
```

**Server Loop:**
```c
// Blocks waiting for client to send a hostname
n = read(newsockfd, buffer, ...);

// Removes newline from received buffer to ensure exact match
buffer[strcspn(buffer, "\n")] = 0;

// Calls search wrapper
char *result = search_dns(buffer);

// Sends result back
write(newsockfd, result, strlen(result));
```

## 4. Glossary: New Variables & Functions

### **Variables**
- **`pid_t`**
  - **Type**: Signed integer type.
  - **Purpose**: Represents Process IDs. Used for the return value of `fork()`.
- **`struct sockaddr_in`**
  - **Type**: Structure.
  - **Purpose**: Holds IPv4 address information (family, port, IP). Needed for `bind`, `connect`, etc.
- **`socklen_t`**
  - **Type**: Unsigned integer type (at least 32 bits).
  - **Purpose**: Used to store the size of the address structure. Passed by pointer to `accept` and `recvfrom`.

### **Functions**
- **`fork()`**
  - **Library**: `<unistd.h>`.
  - **Purpose**: Duplicates the current process. Returns `0` to the child and the child's PID to the parent.
- **`exit(status)`**
  - **Library**: `<stdlib.h>`.
  - **Purpose**: Terminates the process. `status` is sent to the parent (retrievable via `wait`).
- **`waitpid(pid, &status, options)`**
  - **Library**: `<sys/wait.h>`.
  - **Purpose**: Waits for a *specific* child process to change state. Useful for preventing zombies.
- **`sendto(sockfd, buf, len, flags, dest_addr, addrlen)`**
  - **Library**: `<sys/socket.h>`.
  - **Purpose**: Sends a datagram to a specific address. Primary sending function for UDP.
- **`recvfrom(sockfd, buf, len, flags, src_addr, &addrlen)`**
  - **Library**: `<sys/socket.h>`.
  - **Purpose**: Receives a datagram and fills `src_addr` with the sender's details. Primary receiving function for UDP.
- **`strcspn(str, reject)`**
  - **Library**: `<string.h>`.
  - **Purpose**: Calculates the length of the initial segment of `str` which consists entirely of characters not in `reject`. Used here to find the index of `\n` to remove it: `buf[strcspn(buf, "\n")] = 0`.

