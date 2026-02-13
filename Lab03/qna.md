# Lab 03 Q&A: Processes & Forking

## 1. Understanding `fork()`
**Q: What exactly happens when I call `fork()`?**

`fork()` creates a **clone** of the current process.
*   The Operating System copies the entire memory space (variables, code, open sockets).
*   Code execution continues from the *exact same line* in both processes.

**Differentiation:**
```c
int pid = fork();
if (pid == 0) {
    // I am the Child (Clone)
} else {
    // I am the Parent (Original)
    // 'pid' holds the ID of the child I just created
}
```

## 2. Zombie Processes
**Q: What is a zombie process and how do I prevent it?**

If a child process finishes (`exit(0)`), but the parent never checks on it (using `wait()`), the child's entry remains in the process table as a "Zombie" (defunct).

**Prevention:**
1.  **Wait:** Call `wait(NULL)` in the parent (blocks parent until child finishes).
2.  **Signal Handler (Advanced):** Use `signal(SIGCHLD, SIG_IGN)` to tell OS to automatically clean up children.

## 3. AF_UNIX vs AF_INET
**Q: Why did we use `AF_UNIX` in the chat program?**

*   **AF_INET:** Uses IP Addresses (127.0.0.1) and Ports. Used for communicating across a network or internet.
*   **AF_UNIX:** Uses **File Paths** (e.g., `/tmp/mysocket`). Used for communicating between processes on the **same machine**.
    *   *Advantage:* Faster (no network protocol overhead).
    *   *Disadvantage:* Cannot talk to a different computer.

## 4. Full Duplex Chat
**Q: How does the two-way chat work without blocking?**

In a normal `cin`/`scanf` program, the program stops and waits for you to type. If it's waiting for you to type, it can't check if a message arrived from the network.

**Solution: Two Processes (or Threads)**
1.  **Child Process:** Only loops on `recv()`. If data arrives, print it. It never waits for keyboard input.
2.  **Parent Process:** Only loops on `scanf/fgets`. If you type, send it. It never waits for network data.
Since OS runs them simultaneously, sending and receiving happen independently.

## 5. Additional Viva Questions
**Q: What is a process table?**
A: A structure in the OS kernel that keeps track of all running processes (PIDs, states, memory usage).

**Q: What does `wait(NULL)` specifically do?**
A: It pauses the parent execution until *any one* of its children dies (`exit`). If a child is already dead (Zombie), it cleans it up instantly.

**Q: Why fork inside a loop for concurrent servers?**
A: Because each iteration of the loop `accepts` a new client. We then fork a dedicated "worker" process just for that client, while the original "manager" (parent) loops back to answer the next call.

**Q: What is IPC?**
A: Inter-Process Communication. It's how separate processes share data (since they don't share memory).
    *   **Pipes:** Unidirectional (`|`).
    *   **Shared Memory:** Fast, complex.
    *   **Sockets:** Can talk between processes on same OR different machines.
    *   **Signals:** Notifications (`SIGINT`, `SIGKILL`).

**Q: In the Chat program, why do we use `AF_UNIX`?**
A: Because both "chatters" are running on the same computer. It avoids the overhead of packaging data into TCP/IP packets. It writes directly to a socket file on disk.

**Q: What is a "Race Condition"?**
A: When two processes try to access/modify a shared resource (like a file or variable) at the exact same time, leading to unpredictable results.

**Q: What is `getpid()` vs `getppid()`?**
A: `getpid()` returns *my* ID. `getppid()` returns my *Parent's* ID. If the parent dies, the child is often adopted by `init` (PID 1).

