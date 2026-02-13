# Lab 03: Process Management & Inter-Process Communication

This guide covers socket programming with a focus on process management, using `fork()` for concurrency, and different socket domains (TCP/UDP, Inet/Unix).

---

## 1. Additional Exercise: Advanced Math Server (`additional/`)
**Objective:** A TCP Client-Server system that passes a complex data structure (struct) back and forth. The server performs various mathematical operations (Addition, Linear Equations, Matrix Multiplication) based on the client's choice.

### Client (`additional/client.c`)
Sends a `DataPacket` structure containing inputs, waits for the same structure back with results.

```c
// ... Includes & Struct Definition ...
// DataPacket struct matches the server's definition exactly. 
// This allows sending the entire memory block of the struct over the network.

int main() {
    // ... Socket Creation & Connection (Standard TCP) ...

    while (1) {
        // Menu
        printf("\n--- MENU ---\n...");
        scanf("%d", &choice);
        packet.option = choice; // Store choice in struct

        if (choice == 4) { ... break; }

        // Gather Inputs directly into the packet struct
        if (choice == 1) {
            scanf("%d %d", &packet.num1, &packet.num2);
        } 
        else if (choice == 3) {
            // Read Matrix dimensions and values
            // ... loops to fill packet.matA and packet.matB ...
        }

        // Send the WHOLE structure
        // sizeof(packet) ensures we send exactly the right number of bytes
        send(sock, &packet, sizeof(packet), 0);

        // Receive the WHOLE structure back
        // The server will have filled in the 'Outputs' section of the struct
        read(sock, &packet, sizeof(packet));

        // Display Results from struct
        if (choice == 1) printf("Sum: %d\n", packet.sum);
        else if (choice == 3) {
             // ... print matrix ...
        }
    }
}
```

### Server (`additional/server.c`)
Receives the struct, processes logic based on `option`, fills result fields, and sends it back.

```c
// ... Struct Definition (Must match Client) ...

void handle_client(int new_socket) {
    DataPacket packet;

    while (1) {
        // Read struct from socket
        int valread = read(new_socket, &packet, sizeof(packet));
        if (valread <= 0) break;

        switch (packet.option) {
            case 1: // Math
                packet.sum = packet.num1 + packet.num2;
                break;
            case 2: // Equation
                // Logic for x = -b/a
                if(packet.a == 0) strcpy(packet.error_msg, "Error...");
                else packet.x_val = -packet.b / packet.a;
                break;
            case 3: // Matrix Mult
                // ... Nested loops for Matrix Multiplication ...
                // packet.matRes[i][j] += ...
                break;
        }
        // Send updated struct back
        send(new_socket, &packet, sizeof(packet), 0);
    }
}
```

---

## 2. Process Basics (`process/test.c`)
**Objective:** Demonstrate `fork()` to create a child process.

```c
#include<stdio.h>
#include<unistd.h> // for fork(), getpid(), getppid()

int main(){
    int mychild;
    printf("parent process\n");
    
    // fork() creates a duplicate of the current process
    // Returns: 
    //   0 to the Child process
    //   PID of child to the Parent process
    mychild = fork();
    
    if(mychild){ 
        // Non-zero result means we are in the Parent
        printf("Inside Parent\n");
    }
    else{ 
        // Zero result means we are in the Child
        // getpid() = My ID
        // getppid() = My Parent's ID
        printf("Inside child %d of parent %d\n", getpid(), getppid());
    }
    return 0;
}
```

---

## 3. Exercise 1: Full-Duplex Chat using `fork()` (`quo1/`)
**Objective:** Create a chat system where both client and server can send and receive messages *simultaneously* (Full Duplex). We use `fork()` so one process handles sending (keyboard -> socket) and the other handles receiving (socket -> screen).
**Note:** These files use `AF_UNIX` (Unix Domain Sockets) which communicate via a file path (`unix_socket_chat`) instead of IP/Port.

### Client (`quo1/client.c`)
```c
// ...
#define SOCKET_PATH "unix_socket_chat"

int main() {
    // ... Create AF_UNIX Socket ...
    // ... Connect to SOCKET_PATH file ...

    // Split process into two!
    pid = fork();

    if (pid == 0) {
        // --- CHILD (Receiver) ---
        // Loops forever waiting for messages FROM server
        while (1) {
            read(sock_fd, buffer, ...);
            printf("Server: %s", buffer);
        }
    } else {
        // --- PARENT (Sender) ---
        // Loops forever waiting for user input TO server
        while (1) {
            fgets(buffer, ...); // Get input
            write(sock_fd, buffer, ...); // Send
        }
    }
}
```

### Server (`quo1/server.c`)
Structure is identical to Client.
1. Creates Socket.
2. `bind` to `unix_socket_chat` (and `unlink` first to remove old file).
3. `listen`, `accept`.
4. `fork()`:
    *   Child: Reads from socket, prints to screen.
    *   Parent: Reads from keyboard, writes to socket.

---

## 4. Exercise 2: UDP Permutations (`quo2/`)
**Objective:** UDP format. Client sends a string, Server calculates all permutations of that string and prints them on the *server's console*. The server sends a simple acknowledgment back.

### Client (`quo2/client.c`)
Standard UDP Loop.
```c
// ...
    // Send Input
    sendto(sockfd, input, ... server_addr ...);

    // Wait for Ack (Half-Duplex: we wait here, blocking)
    recvfrom(sockfd, buffer, ... server_addr ...);
    printf("Server replied: %s\n", buffer);
// ...
```

### Server (`quo2/server.c`)
Receives string, calls recursive `permute` function.
```c
// ...
// Recursive Permutation Function
// Swaps characters to find all arrangements
void permute(char *a, int l, int r) {
    if (l == r) printf("%s ", a); // Base case: print permutation
    else {
        for (i = l; i <= r; i++) {
            swap((a + l), (a + i));
            permute(a, l + 1, r);
            swap((a + l), (a + i)); // Backtrack
        }
    }
}

int main() {
    // ... UDP Bind ...
    while(1) {
        // Receive string
        recvfrom(sockfd, buffer, ... client_addr ...);
        
        // Calculate Permutations (Output goes to Server Console)
        permute(buffer, 0, strlen(buffer) - 1);
        
        // Send Ack
        sendto(sockfd, "Server processed...", ... client_addr ...);
    }
}
```

---

## 5. Exercise 3: Sorting with Processes (`quo3/`)
**Objective:** Client sends alphanumeric string (e.g., "7a2b9"). Server forks:
*   Child Process: Extracts numbers ("729") and sorts ascending ("279").
*   Parent Process: Extracts characters ("ab") and sorts descending ("ba").

### Client (`quo3/client.c`)
Sends string, then reads *twice* (or in a loop) because it expects two messages back (one from child, one from parent).
```c
// ...
    send(sock, input_string, ...);

    // Read loop
    while ((valread = read(sock, buffer, ...)) > 0) {
       printf("%s", buffer); // Prints both responses
    }
// ...
```

### Server (`quo3/server.c`)
Splits work between Parent and Child process.
```c
// ...
    // Read string
    read(new_socket, buffer, ...);
    
    pid_t pid = fork();

    if (pid == 0) {
        // --- CHILD (Numbers) ---
        // 1. Filter digits
        // 2. qsort Ascending
        // 3. Send result
        send(new_socket, msg, ...);
        exit(0);
    } else {
        // --- PARENT (Characters) ---
        wait(NULL); // Optional: Wait for child to finish first for ordered output
        
        // 1. Filter alpha characters
        // ... logic similar to child but checks isalpha() ...
        
        // 2. qsort Descending
        
        // 3. Send result
        send(new_socket, msg, ...);
    }
// ...
```

## 4. Glossary: New Variables & Functions

### **Variables**
- **`pid_t`**
  - **Type**: Data type (integer-like) used for process IDs.
  - **Purpose**: Stores the return value of `fork()`.
  - **Values**: positive (child PID), 0 (inside child), -1 (error).
- **`int fd[2]`**
  - **Type**: Integer array of size 2.
  - **Purpose**: Used by `pipe()` to store file descriptors. `fd[0]` is for reading, `fd[1]` is for writing.

### **Functions**
- **`fork()`**
  - **Library**: `<unistd.h>` (Linux/Unix).
  - **Purpose**: Creates a new process (Child) by duplicating the calling process (Parent).
  - **Key Behavior**: Returns twice! Once in parent, once in child.
- **`getpid()` / `getppid()`**
  - **Library**: `<unistd.h>`.
  - **Purpose**: Returns the Process ID of the current process / Parent Process ID.
- **`wait(NULL)`**
  - **Library**: `<sys/wait.h>`.
  - **Purpose**: Makes the parent process pause execution until one of its child processes terminates. Prevents "zombie" processes.
- **`exit(0)`**
  - **Library**: `<stdlib.h>`.
  - **Purpose**: Terminates the current process immediately. Essential in child processes to prevent them from running parent code.
- **`isalpha()`, `isdigit()`**
  - **Library**: `<ctype.h>`.
  - **Purpose**: Checks if a character is a letter or number. Helpful for sorting tasks.
- **`qsort()`**
  - **Library**: `<stdlib.h>`.
  - **Purpose**: Standard library function to sort arrays. Requires a comparator function.

