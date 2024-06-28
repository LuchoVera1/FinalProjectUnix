#!/bin/bash

HOST="localhost"
PORT=12211
NUM_CLIENTS=35

for (( i=1; i<=$NUM_CLIENTS; i++ ))
do
    builds/client $HOST $PORT &
    sleep 0.1  
done

echo "All clients launched."
