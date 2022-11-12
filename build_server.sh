#! /bin/sh

cmake -DCMAKE_CXX_COMPILER=/software/gcc/12.1.0/bin/gcc -DCMAKE_C_COMPILER=/software/gcc/12.1.0/bin/gcc -S . -B out/build
