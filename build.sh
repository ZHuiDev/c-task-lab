#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
action=${1:-build}
name=${2:-}
mkdir -p build
cc=${CC:-cc}
compile() {
    output=$1
    shift
    if [ "$(basename "$cc")" = zig ]; then
        "$cc" cc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -g -Iinclude "$@" -o "build/$output"
    else
        "$cc" -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -g -Iinclude "$@" -o "build/$output"
    fi
}
core() { compile "$1" "$2" src/task.c src/storage.c; }
single() {
    folder=$1
    base=$2
    case "$base" in ''|*[!a-z_0-9]*) echo 'Invalid source basename' >&2; exit 2;; esac
    compile example "$folder/$base.c"
    ./build/example
}
case "$action" in
    build|run|demo)
        compile c-task-lab src/main.c src/input.c src/task.c src/storage.c
        case "$action" in run) ./build/c-task-lab;; demo) ./build/c-task-lab --demo;; esac
        ;;
    test|check)
        core test_core tests/test_core.c
        ./build/test_core
        compile test_input tests/test_input.c src/input.c
        ./build/test_input
        if [ "$action" = check ]; then
            compile c-task-lab src/main.c src/input.c src/task.c src/storage.c
            ./build/c-task-lab --demo
            for source in lessons/*.c exercises/solutions/*.c stages/*.c; do
                compile example "$source"
                case "$source" in */03_menu.c) printf '0\n' | ./build/example;; *) ./build/example;; esac
            done
            echo 'CHECK PASSED. TODO exercises intentionally excluded.'
        fi
        ;;
    lesson) single lessons "${name:-01_basics}";;
    stage) single stages "${name:-01_task_card}";;
    exercise) single exercises "${name:-01_temperature}";;
    solution) single exercises/solutions "${name:-01_temperature}";;
    *) echo 'Usage: sh build.sh build|run|demo|test|check|lesson|stage|exercise|solution [basename]' >&2; exit 2;;
esac
