#!/bin/bash

SRC_DIR="src"
BUILD_DIR="builds"

SERVER_EXEC="server"
CLIENT_EXEC="client"

compile_program() {
    echo "Compiling server..."
    gcc -Wall -Wextra ${SRC_DIR}/socketInet.c -o ${BUILD_DIR}/${SERVER_EXEC}
    echo "Compiling client..."
    gcc -Wall -Wextra ${SRC_DIR}/client.c -o ${BUILD_DIR}/${CLIENT_EXEC}

    echo "Server compiled"
    echo "Client compiled"
}


clean() {
    echo "Cleaning build directory..."
    rm -rf ${BUILD_DIR}/*
}


show_help() {
    echo "Usage: $0 [option]"
    echo "Options:"
    echo "  compile      - Compile server and client programs"
    echo "  run_server   - Run server"
    echo "  run_client   - Run client"
    echo "  clean        - Clean build directory"
    echo "  help         - Show this help menu"
}



run_server() {
    read -p "Enter port number for server: " port
    echo "Running server on port $port in a new terminal window..."
    if command -v gnome-terminal &> /dev/null
    then
        gnome-terminal -- bash -c "cd ${BUILD_DIR} && ./${SERVER_EXEC} -p $port; exec bash"
    elif command -v x-terminal-emulator &> /dev/null
    then
        x-terminal-emulator -e "bash -c 'cd ${BUILD_DIR} && ./${SERVER_EXEC} -p $port; exec bash'"
    else
        echo "Error: No suitable terminal emulator found."
        return 1
    fi
}

run_client() {
    read -p "Enter server hostname (default: localhost): " hostname
    hostname=${hostname:-localhost}
    read -p "Enter server port: " port
    echo "Running client connecting to $hostname:$port in a new terminal window..."
    if command -v gnome-terminal &> /dev/null
    then
        gnome-terminal -- bash -c "cd ${BUILD_DIR} && ./${CLIENT_EXEC} $hostname $port; exec bash"
    elif command -v x-terminal-emulator &> /dev/null
    then
        x-terminal-emulator -e "bash -c 'cd ${BUILD_DIR} && ./${CLIENT_EXEC} $hostname $port; exec bash'"
    else
        echo "Error: No suitable terminal emulator found."
        return 1
    fi
}


case "$1" in
    compile)
        compile_program
        ;;
    run_server)
        run_server
        ;;
    run_client)
        run_client
        ;;
    clean)
        clean
        ;;
    help)
        show_help
        ;;
    *)
        echo "Error: Invalid option. Use '$0 help' for usage."
        exit 1
        ;;
esac

exit 0
