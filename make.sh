#!/bin/sh

set -xe
clang -Wall -Wextra -pedantic -g main.c -o out/rum
rm -rf out/*.dSYM
