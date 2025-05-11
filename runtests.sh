#!/usr/bin/env bash
mkdir -p build

#if installed use "bear" to make a compile_commands.json
if command -v bear >/dev/null 2>&1
then
    BEAR="bear -- "
fi

compile() {
 $BEAR cc -g -Wall -Wextra -Wshadow $@ -I src -I ext/ \
    test.c \
    ext/Unity/src/unity.c \
    src/doubly_linked_list.c \
    src/scheduler_round_robin.c \
    src/scheduler_sjf.c
}

echo "Building..."
if ! compile -fsanitize=address,undefined -o build/test.asan
then
    echo "Build failed :("
    exit 1
fi

echo "Running tests"
./build/test.asan

if command -v valgrind >/dev/null 2>&1
then
    echo "Valgrinding"
    compile -o build/test >/dev/null 2>&1
    valgrind -q --leak-check=full --errors-for-leak-kinds=all ./build/test >/dev/null
else 
    echo "Valgrind could not be found consider installing valgrind for memory checking"
fi