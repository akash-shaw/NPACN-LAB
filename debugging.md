# Beginner's Guide to Debugging Network C Programs in VS Code (WSL)

Since you are writing Network Programming code using Linux headers (`<sys/socket.h>`, `<netinet/in.h>`), you **must** be running VS Code in a Linux environment. On Windows, this is best done using **WSL (Windows Subsystem for Linux)**.

This guide assumes you have the **C/C++ Extension** installed and are connected to WSL.

---

## 1. Prerequisites (Check these first!)

Open your **WSL Terminal** in VS Code (Terminal -> New Terminal) and run:

```bash
gcc --version
gdb --version
```
If you get "command not found", install them:
```bash
sudo apt-get update
sudo apt-get install build-essential gdb
```

---

## 2. Setting Up the Debugger (`launch.json`)

To click "Play" and debug, VS Code needs to know how to compile your specific file.

1.  Go to the **Run and Debug** view (Play icon on the left sidebar).
2.  Click **create a launch.json file**.
3.  Select **C++ (GDB/LLDB)**.
4.  Select **gcc - Build and debug active file**.

This creates a `.vscode/launch.json` file. Ensure it looks like this (or copy-paste this content):

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Active File (GCC)",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}/${fileBasenameNoExtension}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: gcc build active file",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```

*Note: This configuration tells VS Code to compile whichever C file you have open and then debug it.*

---

## 3. How to Debug a Client-Server Architecture

Network programming is tricky because you have **two programs** running at once (Server and Client). You usually want to debug the **Server** to see how it handles requests, while running the **Client** normally in a terminal to send those requests.

### Step 1: Open the Server Code
Open `server.c` (e.g., `Lab01/server.c`) in the editor.

### Step 2: Set Breakpoints
Click to the **left of the line number** where you want execution to pause. A red dot will appear.
*   **Good places:**
    *   Inside the `while(1)` loop.
    *   Right after `recv()` or `read()` to see what data arrived.
    *   Inside `if` statements (e.g., `if (choice == 1)`).

### Step 3: Start Debugging the Server
Press **F5** (or Run -> Start Debugging).
*   VS Code will compile `server.c`.
*   The bottom terminal will show something like "Build finished successfully".
*   The "Debug Console" usually opens. **Switch back to the "Terminal" tab.**
*   You should see your server print: `Server waiting...` or `Socket created`.
*   **Status:** The Server is now running and attached to the debugger.

### Step 4: Run the Client (Manually)
Since the debugger is busy watching the server, you need a separate terminal for the client.
1.  Click the **+** button in the Terminal panel to open a **new** WSL terminal.
2.  Navigate to the folder: `cd Lab01`
3.  Compile the client manually:
    ```bash
    gcc client.c -o client
    ```
4.  Run the client:
    ```bash
    ./client
    ```

### Step 5: Trigger the Breakpoint
1.  In the Client terminal, type a message or perform an action (depending on the lab).
2.  Press Enter.
3.  **VS Code will flash orange/yellow at the bottom.**
4.  Execution will **Pause** at your red dot in `server.c`.

---

## 4. Using the Debug Controls

Once paused, use the floating toolbar at the top:

*   **Continue (F5):** Resume creating untill the next breakpoint.
*   **Step Over (F10):** Execute the current line and move to the next one. (Great for skipping over `printf` or simple math).
*   **Step Into (F11):** Go *inside* a function call. (Use this if you call a custom function like `check_anagram()` and want to see how it works).
*   **Step Out (Shift+F11):** Finish the current function and go back to where it was called.
*   **Restart (Ctrl+Shift+F5):** Kill the server and start over.
*   **Stop (Shift+F5):** Stop debugging.

---

## 5. Inspecting Variables

When paused:
1.  **Hover:** Hover your mouse over any variable (e.g., `buffer`, `sockfd`, `client_count`) to see its current value.
2.  **Variables Pane:** Look at the **Run and Debug** sidebar (left). The "Variables" section lists all local variables automatically.
3.  **Watch Pane:** If you want to track a specific expression (e.g., `buffer[0]` or `client_count > 2`), click `+` in the **Watch** section and type it in.

---

## 6. Common Issues & Fixes

### "Address already in use" Error
If you stop debugging and try to start again immediately, `bind()` might fail because the OS is holding the port.
*   **Fix 1:** Wait 30-60 seconds.
*   **Fix 2:** Change the `PORT` number in your code (e.g., 3388 -> 3389).
*   **Fix 3:** Run this command in terminal to find and kill the zombies:
    ```bash
    lsof -i :3388  # Replace 3388 with your port
    kill -9 <PID>  # Use the PID from the output above
    ```

### "No such file or directory" (Header files)
If red squiggles appear under `<sys/socket.h>`:
*   Ensure you are using **WSL**.
*   Click the blue `><` icon in the bottom left corner and select "New Window using Distro... -> Ubuntu/WSL". 
*   Windows compilers (MinGW) do not support these Linux headers natively.

### Debugger doesn't stop at `recv` / `accept`
These are **blocking calls**.
*   The debugger *is* working, but the program is sleeping, waiting for the OS to give it data.
*   It will only "wake up" and hit your breakpoint **after** the Client sends something.

### Debugging `fork()` processes
By default, GDB follows the **Parent** process.
*   If your logic is inside `if (fork() == 0) { ... }`, the debugger might skip it!
*   **To debug the child:** You generally have to use `printf` debugging for the child process or attach a second debugger instance (advanced).
*   **Quick Tip:** For debugging, temporarily remove the `fork()` and just call the logic directly to see if it works.

---

## 7. Summary Workflow

1.  **Open** `server.c`.
2.  **F5** to start Server Debugger.
3.  **New Terminal** -> Compile & Run `client.c`.
4.  **Interact** with Client.
5.  **Watch** Server pause at breakpoint.
6.  **Step (F10)** through logic and check variables.
