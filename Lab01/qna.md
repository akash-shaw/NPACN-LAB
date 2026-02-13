# Lab 01 Q&A: Socket Basics (TCP/UDP)

## 1. Using TCP vs UDP in C
**Q: What are the main code-level differences between a TCP and UDP program?**

There are three main differences in the code structure:

1.  **Socket Creation:**
    *   **TCP:** Uses `SOCK_STREAM` (reliable, connection-based).
    *   **UDP:** Uses `SOCK_DGRAM` (datagrams, connectionless).
    ```c
    // TCP
    socket(AF_INET, SOCK_STREAM, 0);
    // UDP
    socket(AF_INET, SOCK_DGRAM, 0);
    ```

2.  **Connection Setup:**
    *   **TCP Client:** Must call `connect` to establish a handshake before sending data.
    *   **TCP Server:** Must call `listen` and then `accept` to create a dedicated socket for each client.
    *   **UDP:** No `connect`, `listen`, or `accept`. The socket is ready to receive/send immediately after binding.

3.  **Data Transmission:**
    *   **TCP:** Uses `send()` and `recv()` (or `write`/`read`). The socket "knows" who the partner is because of the connection.
    *   **UDP:** Uses `sendto()` and `recvfrom()`.
        *   `sendto` requires the destination address every time.
        *   `recvfrom` fills a struct with the sender's address so you know who sent the packet.

## 2. Converting TCP to UDP
**Q: How do I convert a TCP client/server to UDP?**

**For the Server:**
1.  Change `socket()` to use `SOCK_DGRAM`.
2.  Keep `bind()` exactly the same.
3.  **Remove** `listen()` and `accept()`.
4.  Replace `recv()` with `recvfrom()` (you need a `struct sockaddr_in` variable to capture the sender's data).
5.  Replace `send()` with `sendto()` (use the address captured in step 4).

**For the Client:**
1.  Change `socket()` to use `SOCK_DGRAM`.
2.  **Remove** `connect()`.
3.  Replace `send()` with `sendto()` (provide server's IP/Port struct here).
4.  Replace `recv()` with `recvfrom()`.

## 3. Byte Order
**Q: What is `htons` and why do we need it?**

`htons` stands for **Host TO Network Short**.
*   **Problem:** Different computers store numbers differently (Big Endian vs Little Endian).
    *   *Little Endian (Intel/Windows/Linux):* Stores `0x1234` as `34 12`.
    *   *Network Standard (Big Endian):* Expects `0x1234` as `12 34`.
*   **Solution:** `htons(port)` ensures the port number is formatted correctly for the network. If you forget this, Port 80 (0x0050) might be interpreted as Port 20480 (0x5000) by the network stack.

## 4. Socket Descriptors
**Q: What is `sockfd` exactly?**

It is an **integer** (File Descriptor). In Linux, everything is a file.
*   When you call `socket()`, the OS creates a buffer in memory and gives you an ID (like 3, 4, or 5).
*   Anytime you want to send data, you say "Write these bytes to File ID 3", and the OS knows that "File 3" is actually a network cable, not a text file.

## 5. Additional Viva Questions
**Q: What is the loopback address?**
A: `127.0.0.1`. It is a special IP address that refers to "this computer". Packets sent to this address never leave the machine; the OS just loops them back to itself.

**Q: What is a "port"?**
A: An IP address identifies a *machine*. A port identifies a specific *program* on that machine. (e.g., Web Server uses Port 80, our Lab tests use Port 3388).

**Q: What happens if I forget `bind()` in the server?**
A: The server will pick a random ephemeral port (like 49152). Since the client expects the server to be on a specific fixed port (e.g., 3388), the client will fail to connect.

**Q: Why does the client not need `bind()`?**
A: The client initiates the connection. The OS automatically assigns it a random port (e.g., 51234) just for that session. The server learns this port automatically when it receives the first packet.

**Q: What does `listen(sockfd, 5)` mean?**
A: The `5` is the "Backlog". It means "If 5 clients try to connect at the exact same split-second while I am busy handling another one, keep them in a waiting line. If a 6th one comes, reject them."

**Q: What is the difference between `read()` and `recv()`?**
A: `recv()` is specific to sockets and has an extra `flags` parameter. `read()` is a generic file system call. In simple labs, they behave identically.

**Q: Why do we use `struct sockaddr_in`?**
A: It is a C structure designed to hold IPv4 addresses. It groups the Family (AF_INET), Port, and IP Address together so we can pass them to system calls easily.

