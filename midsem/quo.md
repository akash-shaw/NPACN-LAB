**1.** Design and implement a connection-oriented (TCP) client-server system in C for managing employee records. The server maintains a database (array of structures) containing employee details: Name, Employee ID, Experience (in years), and Salary. The server must support multiple clients simultaneously and enable full-duplex communication. Initially, the server should contain at least three employee records. **[20 Marks]**

**a.** The client sends employee details to the server. A child process on the server stores the information in the database and displays the client's IP address, port number, and child process ID.

**b.** The client sends an Employee ID to the server. The server removes duplicate entries with that ID and stores the removed records in dup.txt.

**c.** The server displays employees in descending order of experience. Employees with equal experience are sorted in ascending ASCII order of their names

**d.** If more than two clients are connected, the server broadcasts "terminate session" to all clients and terminates.

All results must be displayed on the client side.