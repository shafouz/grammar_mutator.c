#!/bin/bash

bear -- gcc -g -O0 -I./include -o build/main src/main.c src/parser.c -lgvc -lcgraph -lcdt -lcjson

echo "Running main"
./build/main
