#!/bin/bash

set -xe

mkdir -p ./bin/

gcc -Wall -Wextra -ggdb -o ./bin/example example.c
gcc -Wall -Wextra -ggdb -o ./bin/test test.c -lm
#clang -Wall -Wextra --target=wasm32 -o wasm.o -c ./wasm.c
#wasm-ld -m wasm32 --no-entry --export-all --allow-undefined -o wasm.wasm wasm.o

./bin/example
