#!/bin/sh

set -xe
clang -Wall -Wextra -pedantic -g main.c -o out/rum -lm
rm -rf out/*.dSYM
