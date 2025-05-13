#!/usr/bin/env bash

ERROR='\033[0;31m' # Red
INFO='\033[1;34m' # Blue
NC='\033[0m' # No Color

# check if user remembered to pull --recurse-submodules
if [ ! -e "ext/Unity/src"] || [ ! -e "ext/uthash/src"]
then
    echo -e "Cannot find submodules, call: \ngit submodule update --init"
fi

mkdir -p build

# if installed use "bear" to make a compile_commands.json
if command -v bear >/dev/null 2>&1
then
    BEAR="bear -- "
fi

compile() {
 $BEAR cc -g -Wall -Wextra -Wshadow $@ -I src -I ext/ \
    ext/Unity/src/unity.c \
    src/doubly_linked_list.c \
    src/scheduler_round_robin.c \
    src/scheduler_sjf.c \
    test.c
}

echo -e "${INFO}Building${NC}"
if ! compile -fsanitize=address,undefined -o build/test.asan
then
    echo -e "${ERROR}Build failed :(${NC}"
    exit 1
fi

echo -e "${INFO}Running tests${NC}"
./build/test.asan

if command -v valgrind >/dev/null 2>&1
then
    echo -e "${INFO}Valgrinding${NC}"
    compile -o build/test >/dev/null 2>&1
    valgrind -q --leak-check=full --errors-for-leak-kinds=all ./build/test >/dev/null
else 
    echo -e "${INFO}Valgrind could not be found consider installing valgrind for memory checking${NC}"
fi