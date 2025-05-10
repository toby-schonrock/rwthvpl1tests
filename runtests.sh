#!/usr/bin/env bash
mkdir -p build

compile() {
 cc -g -Wall -Wextra -Wshadow $@ -I src \
    test.c \
    src/doubly_linked_list.c \
    src/scheduler_round_robin.c \
    src/scheduler_sjf.c
}

echo "Building"
compile -fsanitize=address,undefined -o build/test.asan

echo "Running tests"
./build/test.asan

if ! command -v valgrind >/dev/null 2>&1
then
    echo "Valgrind could not be found consider installing valgrind for memory checking"
else 
    echo "Valgrinding"
    compile -o build/test >/dev/null 2>&1
    valgrind -q --leak-check=full --errors-for-leak-kinds=all ./build/test >/dev/null
fi