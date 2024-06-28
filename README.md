# UNIX FINAL PROJECT

This project consists of creating a server service in C that uses INET sockets for communication. The service handles multiple client connections concurrently using fork() and supports various functionalities requested by clients through specific messages.

## Requirements

* C programming environment with a compiler (Tested using gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0)


## Implementation Details
* **getInfo**: This command return a string indicating the service name and version.

* **getNumberOfPartitions**: This command return the number of partitions on the device

* **getCurrentKernelVersion**: This command returns the kernel version

* **sshdRunning**: This command check if the sshd process is running.

## Usage

The user have a script (run_program.sh) to use easily the program.
The user have a help menu using "./run_program.sh help"
This script have several options to compile, clean and run the server and the client in different terminals windows 

## Script Usage examples

* **./run_program.sh compile**: This command compiles the server and the client programs generating the compiled files in the "builds" folder
* **./run_program.sh run_server**: This command executes the server file enabling a server in a new terminal window with the port number typed in
* **./run_program.sh run_client**: This command executes the client file enabling and connecting a client with the active server in a new terminal window with the port number and the host typed in
* **./run_program.sh clean**: This command erase all files in the builds folder
* **./run_program.sh help**: This command displays a help menu showing all the options for using the script

## Clarifications
If any script does not have the permissions to be executed you can use **chmod +x run_program.sh** in ubuntu to give execution permissions to the file




_Proyect by: Luis Espinoza_
