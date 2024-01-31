
# TCP Networking Project

This project implements a basic TCP client-server architecture with custom console color output. It is designed to demonstrate TCP networking concepts, including establishing connections, sending and receiving data, and handling multiple clients.

## Components

- **`color.c` and `color.h`:** These files implement functions for manipulating console output colors. They define various color codes and provide functions to apply these colors to text output, enhancing the visual representation of the console output.

- **`data.h`:** This header file defines data structures, constants, or enums used across the application. It includes configurations, error codes, or other shared data elements that are essential for the functioning of the TCP client and server.

- **`Makefile`:** Contains instructions for compiling the project. It specifies how to build the TCP client and server applications and manages dependencies, ensuring that all components are compiled in the correct order.

- **`tcp_client.c`:** Implements the TCP client part of the application. The client can connect to a server, send data, and receive responses. It utilizes functionalities defined in `color.h` and `data.h` for output formatting and data handling.

- **`tcp_server.c`:** Implements the TCP server part of the application. The server listens for connections from clients, processes incoming data, and responds accordingly. It shares utilities with the client for output formatting and uses shared data structures.

## Building the Project

To compile the project, ensure you have a C compiler installed and run the following command in the terminal:

\```bash
make
\```

This will use the `Makefile` to compile all necessary files and produce executables for the TCP client and server.

## Running the Applications

After compilation, you can start the server and client applications. First, run the server:

\```bash
./server
\```

Then, in a new terminal window, start the client by connecting to the server's IP and port:

\```bash
./client 
\```

## Contributing

Contributions to this project are welcome. Please ensure to follow coding standards and add comments to your code for clarity.
