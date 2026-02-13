# Lab 01 Guide: Socket Programming Basics

This guide provides a comprehensive, line-by-line explanation of the socket programming examples in this lab. We cover both TCP (Transmission Control Protocol) and UDP (User Datagram Protocol) implementations.

---

## 1. Basic TCP Client (`client.c`)
This program establishes a connection to a TCP server, sends a message, and receives a reply.

```c
#include <stdio.h>      // Standard Input/Output library (printf, scanf, etc.)
#include <unistd.h>     // Unix Standard library (close, read, write system calls)
#include <sys/socket.h> // Main library for socket definitions and functions
#include <sys/types.h>  // Definitions for system data types
#include <netinet/in.h> // Constants and structures for internet domain addresses
#include <sys/stat.h>   // File status definitions
#include <fcntl.h>      // File control definitions
#include <string.h>     // String manipulation functions
#define MAXSIZE 50      // Defines a constant named MAXSIZE with value 50
```

```c
main() 
{
    // Declare variables
    int sockfd, retval;                 // sockfd: socket file descriptor/ID; retval: return value for error checking
    int recedbytes, sentbytes;          // variables to store number of bytes received and sent
    struct sockaddr_in serveraddr;      // Structure to hold the server's address information
    char buff[MAXSIZE];                 // Character array (buffer) to hold messages, size 50

    // 1. Create a Socket
    // socket(domain, type, protocol)
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: TCP (reliable, connection-oriented)
    // 0: Default protocol (TCP for SOCK_STREAM)
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    
    // Check if socket creation failed
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error");
    }
    printf("%i", sockfd); // Print the socket ID (just for debugging)

    // 2. Configure Server Address
    serveraddr.sin_family = AF_INET;           // usage of IPv4
    serveraddr.sin_port = htons(3388);         // Set port number to 3388. htons() converts mostly integer to network byte order
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set IP address to localhost (127.0.0.1)

    // 3. Connect to Server
    // connect(socket_id, address_pointer, address_size)
    // Tries to establish connection to the server specified in serveraddr
    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    
    // Check if connection failed
    if (retval == -1)
    {
        printf("Connection error");
    }
    
    printf("enter the text\n"); // Prompt user for input

    // 4. Send Data
    scanf("%s", buff); // Read a string from user input into 'buff'

    // send(socket_id, data, size, flags)
    // Sends the data in 'buff' to the server
    sentbytes = send(sockfd, buff, sizeof(buff), 0);

    // Check if send failed
    if (sentbytes == -1)
    {
        printf("!!");
        close(sockfd); // Close socket if error
    }

    // 5. Receive Data
    // recv(socket_id, buffer, size, flags)
    // Waits to receive data from server and stores it in 'buff'
    recedbytes = recv(sockfd, buff, sizeof(buff), 0);
    
    puts(buff);     // Print the received message to screen
    printf("\n");   // Print a new line
    
    // 6. Close Connection
    close(sockfd);  // Close the socket and free resources
}
```

---

## 2. Basic TCP Server (`server.c`)
This program waits for a connection, receives a message, prints it, and sends a reply back.

```c
#include <stdio.h>      // Standard IO
#include <stdlib.h>     // Standard Library (exit, malloc, etc.)
#include <string.h>     // String functions
#include <unistd.h>     // Unix system calls
#include <sys/socket.h> // Socket functions
#include <sys/types.h>  // System types
#include <netinet/in.h> // Internet address structures
#define MAXSIZE 90      // Buffer size constant
```

```c
main()
{
    // distinct variables for server socket and the new connection socket
    int sockfd, newsockfd, retval;
    socklen_t actuallen;        // Variable to store the length of the client address
    int recedbytes, sentbytes;  // Bytes transfer counters
    struct sockaddr_in serveraddr, clientaddr; // Structures for server and client info

    char buff[MAXSIZE]; // Buffer for messages
    int a = 0;          // Unused variable
    
    // 1. Create Socket (TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error check
    if (sockfd == -1)
    {
        printf("\nSocket creation error");
    }

    // 2. Configure Server Address
    serveraddr.sin_family = AF_INET;           // IPv4
    serveraddr.sin_port = htons(3388);         // Port 3388
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY); // Bind to ANY available network interface on this machine

    // 3. Bind
    // bind(socket, address, size)
    // Assigns the address (IP and Port) to the socket
    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    
    // Error check (Note: usually returns -1 on error, code checks for 1 which is unusual but c'est la vie)
    if (retval == 1) 
    {
        printf("Binding error");
        close(sockfd);
    }

    // 4. Listen
    // listen(socket, backlog)
    // Marks the socket as passive, ready to accept incoming connections
    // '1' is the backlog (queue size for pending connections)
    retval = listen(sockfd, 1);
    
    if (retval == -1)
    {
        close(sockfd);
    }

    // 5. Accept
    // accept(socket, client_addr, addr_len)
    // Blocks/waits until a client connects. Returns a *new* socket descriptor for this specific connection
    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

    if (newsockfd == -1)
    {
        close(sockfd);
    }

    // 6. Receive Data using the NEW socket
    recedbytes = recv(newsockfd, buff, sizeof(buff), 0);

    if (recedbytes == -1)
    {
        close(sockfd);
        close(newsockfd);
    }
    
    puts(buff);     // Print received message
    printf("\n");   // New line
    
    // 7. Send Reply
    scanf("%s", buff); // Read server user input
    sentbytes = send(newsockfd, buff, sizeof(buff), 0); // Send back to client

    if (sentbytes == -1)
    {
        close(sockfd);
        close(newsockfd);
    }

    // 8. Close Sockets
    close(sockfd);     // Close the main listener
    close(newsockfd);  // Close the client connection
}
```

---

## 3. Conversational TCP Client (`Conversation_Client_TCP.c`)
A chat client that stays in a loop to send and receive multiple messages until "stop" is typed.

```c
// Includes are similar to previous files...
main()
{
    // ... Variable declarations ...
    
    // Create TCP Socket
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    // Error handling...

    // Configure Target Server Address (Port 4567, IP 172.18.175.156)
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(4567);
    serveraddr.sin_addr.s_addr=inet_addr("172.18.175.156"); 
    // connect...
    
    // Infinite Loop for Chat
    for (i = 0; ; i+=1)
    {
        // Clear buffer with null bytes to prevent garbage data
        memset(buff, '\0', sizeof(buff)); 
        
        printf("enter the text\n");
        scanf("%s",buff); // Get user input
        
        // Ensure string is null-terminated and calculate real size
        buff[strlen(buff)] = '\0'; 
        int s = strlen(buff) * sizeof(char); 
        
        // Send message
        sentbytes=send(sockfd,buff,s,0);
        
        // Check for exit command "stop"
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
        {
            break; // Exit loop
        }
        
        // Clear buffer before receiving
        memset(buff, '\0', sizeof(buff));
        
        // Receive reply from server
        recedbytes=recv(sockfd,buff,sizeof(buff),0);
        printf ("%s \n", buff);
        
        // Check if server said "stop"
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
        {
            break; // Exit loop
        }
    }
    close(sockfd); // Clean up
}
```

---

## 4. Conversational TCP Server (`Conversation_Server_TCP.c`)
The counterpart to the chat client. It first receives a message, then allows the user to reply.

```c
// ... Includes ...

// Helper function to print a message
void printsomething(char msg[]){
    printf("%s\n", msg);
};

main()
{
    printsomething("abcd"); // Test print

    // Setup Socket, Bind, Listen, Accept (standard TCP boilerplate as seen before)
    // ...
    // Note: Binds to INADDR_ANY on Port 4567
    
    // Chat Loop
    for (i = 0; ; i+=1)
    {
        // 1. Receive First
        memset(buff, '\0', sizeof(buff)); // Clear buffer
        recedbytes=recv(newsockfd,buff,sizeof(buff),0); // Blocking wait for data
        
        // ... Error checking ...

        // Check for "stop" from client
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
        {
            break;
        }
        printf("%s \n",buff); // Display client message

        // 2. Send Reply
        memset(buff, '\0', sizeof(buff)); // Clear
        scanf("%s",buff); // Input reply
        
        // Send reply
        buff[strlen(buff)] = '\0';
        int s = strlen(buff) * sizeof(char);
        sentbytes=send(newsockfd,buff,s,0);
        
        // Check if server user typed "stop"
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
        {
            break;
        }
    }
    close(sockfd);
    close(newsockfd);
}
```

---

## 5. Conversational UDP Client (`Conversation_Client_UDP.c`)
UDP is connectionless. This client sends packets to a specific address without "connecting" first.

```c
// ... Includes ...
main()
{
    // ... Variables ...
    
    // Create Socket: SOCK_DGRAM specifies UDP
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    
    // Define TARGET (Server) Address: Port 3388
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(3388);
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    // Define LOCAL (Client) Address: Port 3389
    // In UDP, it's common to bind the client too if we want a specific return port
    clientaddr.sin_family=AF_INET;
    clientaddr.sin_port=htons(3389);
    clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    // Bind this socket to port 3389
    retval=bind(sockfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr));

    // Chat Loop
    for (i = 0; ; i+= 1)
    {
        printf("enter the text\n");
        scanf("%s",buff);

        // sendto(socket, data, len, flags, destination_addr, addr_len)
        // Must specify destination every time because there is no permanent connection
        sentbytes=sendto(sockfd,buff,sizeof(buff),0,
        (struct sockaddr*)&serveraddr,sizeof(serveraddr));

        // ... Check "stop" ...
    
        int size=sizeof(serveraddr);
        // recvfrom(socket, buffer, len, flags, sender_addr_holder, addr_len_pointer)
        // Receives data and fills 'serveraddr' with who sent it
        recedbytes=recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,&size);
        
        puts(buff);
        
        // ... Check "stop" ...
    }
    
    close(sockfd);
}
```

---

## 6. Conversational UDP Server (`Conversation_Server_UDP.c`)
Recieves UDP packets and replies.

```c
// ... Includes ...
main()
{
    // ... 
    // Create UDP Socket
    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    // Define CLIENT info (for reference, though recvfrom will overwrite this mostly)
    clientaddr.sin_family=AF_INET;
    clientaddr.sin_port=htons(3389);
    clientaddr.sin_addr.s_addr=htons(INADDR_ANY);

    // Define MY (Server) Address: Port 3388
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(3388);
    serveraddr.sin_addr.s_addr=htons(INADDR_ANY);

    // Bind socket to Port 3388
    retval=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    
    // Chat Loop
    for (i = 0; ; i+=1)
    {
        // Receive
        actuallen=sizeof(clientaddr);
        // Blocks until a packet arrives. Stores sender info in 'clientaddr'
        recedbytes=recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,&actuallen);

        // Print message
        puts(buff);
        
        // ... Check stop ...

        // Input reply
        scanf("%s", buff);
        
        // Send Reply
        // Uses 'clientaddr' which was filled by recvfrom to know where to send back
        sentbytes=sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));

        // ... Check stop ...
    }
    
    close(sockfd); // Note: close(newsockfd) logic in original code is redundant for UDP as there is no 'newsockfd' created by accept()
}
```

---

## 7. UDP Character Client (`udp_client.c`)
This seemingly experimental program attempts to send a string character-by-character or string-by-string.

```c
// ... Includes ...
int main()
{
    // ... Variables ...
    // Create UDP socket
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    
    // Address Setup (Client: 3201, Server: 3200)
    // ...
    // Bind Client to 3201
    retval=bind(sockfd,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
    
    gets(buff); // Read full line of text from user (unsafe function, use fgets in modern code)
    
    actuallen=sizeof(serveraddr);
    
    // Loop through the input string
    for(k=0;k<=strlen(buff);k++)
    {
        // Take character at k
        temp[0]=buff[k];
        temp[1]='\0'; // null terminate it
        
        strcpy(temp1,temp); // ensure copy
        
        // Send just this one character/string to server
        retval=sendto(sockfd,temp1,sizeof(temp),0,(struct sockaddr *)&serveraddr,actuallen);
        
        if(retval==-1) { /* exit */ }
        
        // Wait for Echo/Reply immediately
        actuallen=sizeof(serveraddr);
        retval=recvfrom(sockfd,temp1,sizeof(temp1),0,(struct sockaddr *)&serveraddr,&actuallen);
        puts(temp1); // Print reply
    }
    
    close(sockfd);
}
```

---

## 8. UDP Character Server (`udp_server.c`)
Receives packets and echoes them back.

```c
// ... Includes ...
int main()
{
    // ...
    // UDP Socket creation
    // Bind to Port 3200(Server)
    retval=bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    
    // PROBLEM AREA in Logic:
    // The loop condition k < strlen(buff) uses 'buff' which is currently uninitialized/empty.
    // This loop might not run at all or behave unpredictably depending on memory garbage.
    for(k=0;k<strlen(buff);k++) 
    {
        // Receive packet
        retval=recvfrom(sockfd,temp,sizeof(temp),0,(struct sockaddr *)&clientaddr,&actuallen);
        
        puts(temp); // Print it
        
        // Echo it back
        retval=sendto(sockfd,temp,sizeof(temp),0,(struct sockaddr *)&clientaddr,actuallen);
    }
    
    // Final receive attempt meant to handle a full string (outside the loop)
    gets(buff); // User input on server side?
    retval=sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr *)&clientaddr,actuallen);

    close(sockfd);
}
```

---

# Detailed Component Reference

This section breaks down the specific variables and functions used across the lab programs, explaining their purpose, how they work, and how you can modify them.

## 1. Variables & Data Types

### `int sockfd` (Socket File Descriptor)
*   **What it is:** An integer representing the "handle" or ID of the socket.
*   **Why used:** The operating system uses this number to track the open network connection. Every function (send, recv, close) needs this ID to know *which* connection to talk to.
*   **How to modify:** You don't modify the value directly. It is assigned by the `socket()` function. If the value is `-1`, it means an error occurred.

### `int newsockfd` (New Socket File Descriptor)
*   **What it is:** Another integer socket ID.
*   **Why used:** In TCP Servers, `sockfd` is used *only* for listening for new people trying to join. When someone actually joins, the OS creates a *brand new separate line* for that specific person. That new line is `newsockfd`.
*   **Key Distinction:** 
    *   `sockfd`: The "receptionist" waiting at the door.
    *   `newsockfd`: The "private room" assigned to the guest. 

### `struct sockaddr_in` (Socket Address Structure)
*   **What it is:** A predefined C structure holding internet address details.
*   **Why used:** Sockets need to know "Who, Where, and How". This struct packages IP family, Port, and IP Address together.
*   **Fields:**
    *   `sin_family`: Always `AF_INET` for IPv4.
    *   `sin_port`: The port number (e.g., 3388). Must be converted using `htons()`.
    *   `sin_addr.s_addr`: The IP address.
*   **How to modify:**
    *   To change the Server IP: change `inet_addr("127.0.0.1")` to `"192.168.1.X"`.
    *   To change the Port: change `htons(3388)` to `htons(8080)`.

### `socklen_t` (Socket Length Type)
*   **What it is:** An integer type specifically for the length of socket structures (at least 32 bits).
*   **Why used:** Functions like `accept()` and `recvfrom()` need a pointer to the *size* of the address structure so they don't overwrite memory.

### `char buff[MAXSIZE]` (Data Buffer)
*   **What it is:** A simple array of characters.
*   **Why used:** This is the "bucket" that holds data being sent or received.
*   **How to modify:** Change `MAXSIZE` at the top of the file to send larger messages (e.g., `#define MAXSIZE 1024`).

### `retval`, `sentbytes`, `recedbytes` (Status checks)
*   **What they are:** Integers storing results of functions.
*   **Why used:** Almost every network function returns `-1` if it fails. We store the result here to check `if (retval == -1)`. `sentbytes` and `recedbytes` specifically tell you how much data was actually transferred.

---

## 2. Functions

### `socket(domain, type, protocol)`
*   **What it does:** Creates the endpoint for communication.
*   **Arguments:**
    *   `AF_INET`: Standard IPv4 Internet.
    *   `SOCK_STREAM`: TCP (Reliable, sequential). **Modify to `SOCK_DGRAM` for UDP.**
    *   `0`: Auto-select protocol (IP).
*   **Returns:** File descriptor (int) or -1 on error.

### `bind(sockfd, address, addrlen)`
*   **What it does:** "Glues" a socket ID to a specific Port and IP on your local machine.
*   **Why used:** Servers need a fixed port (like 3388) so clients know where to knock.
*   **Used by:** Servers mostly. Sometimes UDP clients.

### `listen(sockfd, backlog)`
*   **What it does:** Tells the OS "I am a server, start accepting connection requests".
*   **Arguments:** `backlog` is the number of people allowed to wait in line. If you set this to `5`, the 6th person trying to connect simultaneously will be rejected.
*   **Used by:** TCP Servers only.

### `accept(sockfd, address, len_ptr)`
*   **What it does:** **BLOCKS** (pauses program) until a client connects. Handshakes and creates a new socket.
*   **Used by:** TCP Servers only.

### `connect(sockfd, address, len)`
*   **What it does:** The Client's way of dialing the phone number. Starts the 3-way handshake.
*   **Used by:** TCP Clients only.

### `send(sockfd, buf, len, flags)` / `recv(...)`
*   **What it does:** Sends/Receives data over an *established* connection.
*   **Used by:** TCP only (because they assume a connection exists).

### `sendto(sockfd, buf, len, flags, dest_addr, addrlen)`
*   **What it does:** Sends a packet to a *specific address*.
*   **Why different?** Since there is no "connection" in UDP, you have to write the address on the envelope every single time you send a message.
*   **Used by:** UDP.

### `recvfrom(sockfd, buf, len, flags, src_addr, len_ptr)`
*   **What it does:** Receives a packet and *writes down who sent it* into `src_addr`.
*   **Why different?** In UDP, anyone can send you a packet. This function tells you who it was so you can reply.
*   **Used by:** UDP.

### `htons(short_int)` (Host to Network Short)
*   **What it does:** Flips numbers around in memory.
*   **Why used:** Your computer (likely Intel/AMD) stores numbers "Little Endian" (backwards). The Internet uses "Big Endian" (forwards). If you send port 3388 without this, the network might read it as port 52441.

### `inet_addr("X.X.X.X")`
*   **What it does:** Converts a human readable string IP ("127.0.0.1") into the raw binary format the network card understands.

---

# Lab 01 Exercises

This section covers the additional exercises found in the `Lab01 Excercise` folder.

## 1. Additional Exercise: Client Info Display (`additional/`)
**Objective:** A TCP server that accepts a connection and prints the client's IP address, port number, and socket family.

### Client (`additional/client.c`)
Standard TCP client connecting to port 8080.
```c
// ... Includes ...
#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    
    // 1. Create Socket (TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    // ... Error checking ...
    
    // 2. Connect to Localhost:8080
    // bzero is a legacy function similar to memset, zeros out the struct
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_addr.sin_port = htons(PORT);
    
    // connect() initiates the handshake
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting"); // Helper to print error + reason
        exit(1);
    }
    
    printf("Connected to TCP Server successfully.\n");
    close(sockfd); // Connect, print success, then close immediately
    return 0;
}
```

### Server (`additional/server.c`)
Accepts the connection and extracts client details.
```c
// ... Includes ...
int main() {
    // ...
    // Standard Socket -> Bind -> Listen sequence on Port 8080
    // ...
    
    clilen = sizeof(cli_addr);
    
    // 4. Accept
    // vital: accept fills 'cli_addr' with the connecting client's details!
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    // 5. Display Client Information using 'cli_addr'
    printf("\n--- Client Connected ---\n");
    
    // inet_ntoa converts binary IP (struct in_addr) to string "X.X.X.X"
    printf("Client IP Address: %s\n", inet_ntoa(cli_addr.sin_addr));
    
    // ntohs converts Network Byte Order (Big Endian) to Host Short (Little Endian)
    printf("Client Port Number: %d\n", ntohs(cli_addr.sin_port));
    
    // Family is usually 2 (AF_INET)
    printf("Client Socket Family: %d (AF_INET)\n", cli_addr.sin_family);
    
    close(newsockfd);
    close(sockfd);
}
```

---

## 2. Exercise 1: Number Processing Service (`quo1/`)
**Objective:** A TCP Client-Server menu-driven program where the server performs operations on a set of integers (Search, Sort, Split Odd/Even).

### Client (`quo1/client.c`)
Sends commands and data, then waits for results.
```c
// ...
int main() {
    // ... Setup socket & Connect ...

    while (1) {
        // Display Menu
        printf("1. Search\n2. Sort\n3. Split Odd/Even\n4. Exit\n");
        scanf("%d", &choice);

        // 1. Send Command Choice
        write(sockfd, &choice, sizeof(int)); // 'write' is synonymous with 'send' in sockets

        if (choice == 4) break; // Exit

        // 2. Send Data Set
        // First send SIZE
        scanf("%d", &size);
        write(sockfd, &size, sizeof(int));

        // Then send ARRAY
        for (i = 0; i < size; i++) scanf("%d", &arr[i]);
        write(sockfd, arr, size * sizeof(int)); // Send entire array block

        // 3. Command Logic
        switch (choice) {
            case 1: // Search
                scanf("%d", &search_key);
                write(sockfd, &search_key, sizeof(int)); // Send key
                read(sockfd, &found, sizeof(int));       // Get result
                // ... Print result ...
                break;

            case 2: // Sort
                scanf("%d", &order);
                write(sockfd, &order, sizeof(int));      // Send order (asc/desc)
                read(sockfd, arr, size * sizeof(int));   // Receive SORTED array back
                // ... Print array ...
                break;

            case 3: // Split
                // Server sends counts first
                read(sockfd, &odd_count, sizeof(int));
                read(sockfd, &even_count, sizeof(int));
                // Then server sends the arrays
                read(sockfd, odd_arr, odd_count * sizeof(int));
                read(sockfd, even_arr, even_count * sizeof(int));
                // ... Print arrays ...
                break;
        }
    }
}
```

### Server (`quo1/server.c`)
Processes the data based on the command.
```c
// ...
int main() {
    // ... Socket -> Bind -> Listen -> Accept ...
    
    while (1) {
        // 1. Read Command
        n = read(newsockfd, &choice, sizeof(int));
        if (n <= 0) break; // Client disconnected

        // 2. Read Data Set
        read(newsockfd, &size, sizeof(int));          // Get Size
        read(newsockfd, buffer, size * sizeof(int));  // Get Array
        
        // 3. Process
        switch (choice) {
            case 1: // Search
                read(newsockfd, &search_key, sizeof(int)); // Get Key
                // ... Linear Search Logic ...
                // write result back
                break;
                
            case 2: // Sort
                 read(newsockfd, &order, sizeof(int)); // Get Order
                 // ... Bubble Sort Logic ...
                 // write sorted array back
                 break;
                 
            case 3: // Split
                 // ... Separate integers into odd/even arrays ...
                 // Write counts
                 write(newsockfd, &odd_cnt, sizeof(int));
                 write(newsockfd, &even_cnt, sizeof(int));
                 // Write arrays
                 write(newsockfd, odd_arr, ...);
                 write(newsockfd, even_arr, ...);
                 break;
        }
    }
}
```

---

## 3. Exercise 2: String Analysis Service (`quo2/`)
**Objective:** UDP Client sends a string, Server analyzes it (Length, Palindrome check, Vowel count) and sends back a formatted report.

### Client (`quo2/client.c`)
Example of UDP client sending strings.
```c
// ...
int main() {
    // ... Create UDP Socket ...
    // ... Fill server address struct (Port 8080) ...

    while (1) {
        // Get Input
        scanf("%s", message);
        
        // 1. Send TO Server
        // UDP requires 'sendto' with destination address every time
        sendto(sockfd, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        
        if (strcmp(message, "Halt") == 0) break;
        
        // 2. Recv FROM Server
        // 'len' must be initialized to sizeof(servaddr) before recvfrom
        len = sizeof(servaddr);
        n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0'; // Null terminate string
        
        printf("%s", buffer); // Print report
    }
    close(sockfd);
}
```

### Server (`quo2/server.c`)
UDP Server receiving strings and replying.
```c
// ...
int main() {
    // ... Create UDP Socket -> Bind to Port 8080 ...
    
    while (1) {
        len = sizeof(cliaddr);
        
        // 1. Recv FROM Client
        // 'cliaddr' is filled with the sender's info. needed to reply.
        n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        if (strcmp(buffer, "Halt") == 0) break;
        
        // 2. Analyze String
        int length = strlen(buffer);
        int checkPalindrome(char *str); // Helper function
        int isPal = checkPalindrome(buffer); 
        // ... Count vowels loop ...
        
        // 3. Format Reply
        // sprintf prints to a string buffer instead of screen
        sprintf(response, 
                "Length: %d\nPalindrome: %s\nVowels: %d ...", 
                length, ...);
        
        // 4. Send TO Client
        // sendto uses 'cliaddr' captured from recvfrom
        sendto(sockfd, response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
    }
}
```

