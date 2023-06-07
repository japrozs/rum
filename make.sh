#!/bin/sh

set -xe
clang -Wall -Wextra -pedantic main.c -o out/rum -lm
rm -rf out/*.dSYM
