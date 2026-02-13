# Lab 02 Q&A: File Handling & Binary Data

## 1. Sending Non-Text Data
**Q: How do I send an integer array or a structure instead of just strings?**

The `send` and `recv` functions take a `void *` pointer, meaning they can send raw memory bytes of *anything*.

**Sending an Array:**
```c
int numbers[5] = {10, 20, 30, 40, 50};
// Send the total number of bytes: 5 * 4 bytes = 20 bytes
send(sockfd, numbers, 5 * sizeof(int), 0);
```

**Receiving an Array:**
```c
int buffer[5];
recv(sockfd, buffer, 5 * sizeof(int), 0);
// buffer now contains {10, 20, 30, 40, 50}
```
*Tip: Always valid to verify `sizeof(int)` is the same on both machines, but for these labs (localhost), it is safe.*

## 2. Checking File Existence
**Q: How do I check if a file exists before trying to open it?**

Use the `access()` function from `<unistd.h>`.
```c
if (access("filename.txt", F_OK) != -1) {
    // File exists
} else {
    // File does not exist
}
```
*   `F_OK`: Checks existence.
*   `R_OK`: Checks if you have read permission.
*   `W_OK`: Checks if you have write permission.

## 3. "Address Already in Use"
**Q: Why do I get a bind error when I restart my server?**

When you stop a server (Ctrl+C), the OS doesn't immediately free the port. It puts it in a `TIME_WAIT` state for a minute to handle any straggler packets.

**Fix:** Use `setsockopt` before `bind`:
```c
int opt = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
bind(sockfd, ...);
```
This tells the OS, "I know this port looks busy, but let me use it anyway."

## 4. Calculating File Size
**Q: How do I find the size of a file in C?**

You can use the seek-and-tell method:
```c
FILE *fp = fopen("file.txt", "r");
fseek(fp, 0, SEEK_END); // Jump to the very last byte
long size = ftell(fp);  // Ask "Where am I?" (Returns byte offset)
rewind(fp);             // Jump back to start so you can read it
```

## 5. Additional Viva Questions
**Q: What is `setsockopt`? Why do we use `SO_REUSEADDR`?**
A: `setsockopt` allows us to configure low-level socket options. `SO_REUSEADDR` is critical in servers: if the server crashes, the port normally stays locked for 1-2 minutes ("TIME_WAIT" state). This option forces the OS to let us restart the server *immediately* on the same port.

**Q: What does `scanf` return?**
A: It returns the number of items successfully read. Useful for checking if the user actually typed a number or garbage.

**Q: Why use `ftell`?**
A: `ftell` returns the current position of the file pointer. We use it with `fseek(..., SEEK_END)` to find out how big a file is without reading the whole thing.

**Q: What is `access(..., F_OK)`?**
A: A system call to verify if a file exists. `F_OK` (File OK) means check existence. `R_OK` means check readability.

**Q: Can I send a struct directly with `write`?**
A: Yes. `write(sock, &person, sizeof(Person))` takes the raw memory address. As long as both client and server agree on the struct layout (same OS/compiler), it works perfectly.

**Q: Why `sizeof(int) * n` when sending arrays?**
A: `send` needs the total number of *bytes*, not elements. An array of 5 integers is actually `5 * 4 = 20` bytes.

**Q: What is `rewind(fp)` effectively synonymous with?**
A: `fseek(fp, 0, SEEK_SET)`. It resets the read pointer to the very beginning of the file.

