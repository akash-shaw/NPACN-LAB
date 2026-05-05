#!/bin/bash

# 1. Clean up previous runs
echo "Stopping any existing daemons..."
pkill server_daemon
rm -f /tmp/daemon.log
touch /tmp/daemon.log

# 2. Compile codes
echo "Compiling..."
gcc server_daemon_syslog.c -o server_daemon
gcc client_daemon_syslog.c -o client

# 3. Start the Daemon
echo "Starting Daemon..."
./server_daemon

# 4. Give the daemon a second to initialize
sleep 1

# 5. Open tail in the background and then run client
echo "--- LOG OUTPUT BELOW ---"
tail -f /tmp/daemon.log & 
TAIL_PID=$!

# 6. Run the client after a short delay
sleep 1
echo "Running Client..."
./client

# 7. Wait a moment for the log to update, then clean up the tail process
sleep 2
kill $TAIL_PID
echo "--- DEMO COMPLETE ---"