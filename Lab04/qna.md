# Lab 04 Q&A: Advanced Data & Concurrency

## 1. Concurrent Servers
**Q: Why do we need a `fork()` inside the server's `while(1)` loop?**

In an **Iterative Server** (Lab 01/02), if Client A connects and wants a 10-second task, Client B waits in the queue until A finishes.

In a **Concurrent Server**:
1.  Server accepts Client A.
2.  Server forks a **Child A**.
3.  **Child A** handles Client A (takes 10 seconds).
4.  **Parent** immediately loops back to `accept()`.
5.  Parent accepts Client B instantly, even while Child A is still working.

## 2. Struct Padding & Serialization
**Q: Is it safe to send `structs` over the network?**

```c
struct Student {
    char initial; // 1 byte
    int age;      // 4 bytes
};
```
Technically, no (for the internet).
*   **Padding:** Compilers might add 3 bytes of "padding" after `initial` so `age` starts at a 4-byte alignment. Total size = 8 bytes. Another computer might pack it differently (Total size = 5 bytes).
*   **Endianness:** Integers might be flipped.

**However**, for this Lab (where Client and Server are on the same machine/OS), it is perfectly safe and very efficient. Code like `send(sock, &student, sizeof(Student), ...)` creates a direct memory dump.

## 3. Connectionless Concurrency (UDP)
**Q: Can UDP servers be concurrent?**

Yes, but it's different.
*   **TCP:** One new socket per client. Child handles that specific socket.
*   **UDP:** Only ONE socket exists.
    *   A child can answer a request, but it must use the same socket (`sockfd`) and use `sendto` with the specific client's address.
    *   Since UDP is usually quick (single packet request/response), concurrency is used less often than in TCP.

## 4. DNS Logic
**Q: How does the DNS program work?**

It simulates a real DNS lookup.
1.  **Database:** An array of structs matching Hostnames -> IPs.
    ```c
    {"google.com", "142.250.183.68"}
    ```
2.  **Lookup:** Standard string search (`strcmp`).
3.  **Real World:** A real DNS server would forward the query to another DNS server (Recursive Query) if it didn't know the answer. Ours is just an "Iterative" authoritative server simulation.

## 5. Additional Viva Questions

### **Concurrency & Processes**
**Q: What happens if the parent process terminates before the child process?**
The child becomes an **Orphan Process**. It is adopted by the `init` process (PID 1) (or `systemd`), which cleans it up when it finishes.

**Q: What happens if the child terminates but the parent doesn't call `wait()`?**
The child becomes a **Zombie Process**. It has finished execution, but its entry in the process table remains (showing `<defunct>`). This wastes system resources. A concurrent server should use a signal handler (SIGCHLD) to clean up zombies asynchronously.

**Q: How does `fork()` distinguish between Parent and Child?**
By the return value:
-   `> 0`: Pending in the **Parent**. The value is the Child's PID.
-   `== 0`: Pending in the **Child**.
-   `-1`: Fork failed (process limit reached).

### **C & Networking**
**Q: What is the difference between `read()` and `recv()`?**
 they are almost identical for sockets.
-   `read(fd, buf, len)` is a standard file system call.
-   `recv(fd, buf, len, flags)` is specific to sockets. It allows extra flags (like `MSG_PEEK` to look at data without removing it from the buffer).

**Q: Why do we use `while(1)` in the server?**
To keep the server running indefinitely.
-   **Outer `while(1)`**: Accepts new connections repeatedly.
-   **Inner `while(1)` (in child)**: Handles multiple requests from *one* connected client (if the protocol is persistent).

**Q: What is `struct padding`?**
Processors align data in memory for speed (e.g., integers at addresses divisible by 4). If you have a `char` (1 byte) followed by an `int` (4 bytes), the compiler adds 3 bytes of "padding" after the char. This changes the size of the struct, which can cause issues if the client and server have different architectures (e.g., 32-bit vs 64-bit).

