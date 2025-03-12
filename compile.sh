#!/bin/bash

echo "Compiling src files."

mkdir -p ./build

g++ -Wall -Wextra -O2 -g ./src/redis.c -o ./build/redis
g++ -Wall -Wextra -O2 -g ./src/client.c -o ./build/client
