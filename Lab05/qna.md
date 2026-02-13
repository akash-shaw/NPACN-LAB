# Lab 05 Q&A: Connection Management

## 1. Connection Limiting
**Q: How does the server limit clients to 2?**

It uses a counter variable.
```c
int client_count = 0;
while(1) {
    accept(...);
    client_count++;
    if (client_count > 2) {
        // Reject
    }
}
```
*   **Critical Detail:** In a multi-process environment, `client_count` in the *Parent* must be updated. If the Child increments its own copy of the variable, the Parent won't know.
*   In our simple lab Example, the parent increments it right after `accept`, which works. If we needed to decrement it when a child exits (to allow a new slot), we would need **Shared Memory** or Pipes, because the Parent cannot see the Child's variables.

## 2. Iterative vs Concurrent Behavior
**Q: How can I tell if a server is Iterative or Concurrent just by looking at the code?**

*   **Iterative:** The logic processing (reading/calculating/writing) happens *inside* the main `while(1)` loop. The loop cannot restart until the logic finishes.
*   **Concurrent:** The logic processing happens *inside* an `if (fork() == 0)` block. The main loop reaches the end and restarts immediately.

## 3. Persistent vs Non-Persistent Connections
**Q: What is the difference between the Client connecting once vs connecting every time?**

*   **Persistent (Keep-Alive):** Client connects, enters `while(1)`, sends 10 requests, then closes.
    *   *Pros:* Less overhead establishing handshakes.
    *   *Cons:* Ties up a server slot/process even when idle.
*   **Non-Persistent (Short-lived):** Client enters `while(1)`, Connects, Sends 1 Data, Receives, Closes. Repeats.
    *   *Pros:* Frees up server immediately.
    *   *Cons:* Very slow if sending many small items (3-way handshake overhead for every single item).

## 4. File Appending Race Conditions
**Q: If two clients write to `result.txt` at the same time, what happens?**

In the Concurrent File Server exercise:
*   If Child A and Child B try to write to the file at the exact same nanosecond, the output might be garbled ("HellWorldo").
*   However, `fprintf` is usually buffered, and the OS file system handles basic locking for small writes.
*   **Safe Way:** Use `flock()` (File Lock) or Semaphores to ensure only one child writes to the file at a time.

## 5. Additional Viva Questions

### **File Handling & Concurrency**
**Q: Why use `O_APPEND` or "a" mode for log files?**
When multiple processes write to the same file:
-   **Without Append:** Process A writes at byte 100. Process B thinks EOF is at 100 and writes there. They overwrite each other.
-   **With Append (`O_APPEND`):** The OS atomically moves the file pointer to the *current* end of file before every write. This prevents overwriting even without explicit locks (for small writes).

**Q: What is a Race Condition?**
A situation where the outcome of a program depends on the timing of uncontrollable events (like which process the OS schedules first).
*   **Example:** Two clients try to book the last seat on a flight. Both read "Seats = 1". Both book it. Result: "Seats = -1".

### **Advanced Process Management**
**Q: Why can't the parent just decrement `client_count` when a child exits?**
Because processes have **separate memory spaces**.
-   When `fork()` happens, the child gets a *copy* of variables.
-   If the parent updates `client_count`, the child doesn't see it (and vice versa).
-   To share a counter, you must use **Shared Memory** (`shmget`, `shmat`) or message queues.

**Q: Parameters of `listen(int sockfd, int backlog)`?**
-   `sockfd`: The socket descriptor.
-   `backlog`: The length of the pending queue.
    *   If `backlog = 5`, and 5 clients are waiting to be `accept()`-ed, the 6th client will receive "Connection Refused".

### **Time Functions**
**Q: What is `time_t`?**
It is usually a `long` integer representing the number of seconds since the "Unix Epoch" (January 1, 1970 00:00:00 UTC).

**Q: How do we convert `time_t` to a readable string?**
1.  `time(&rawtime)`: Gets seconds.
2.  `localtime(&rawtime)`: Converts seconds to a `struct tm` (broken down time: year, month, day, hour...).
3.  `asctime(timeinfo)`: Converts `struct tm` to a string like "Wed Jun 30 21:49:08 1993\n".

