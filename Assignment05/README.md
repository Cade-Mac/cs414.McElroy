# Assignment 05: Key-Value Store (C++ & OCaml)

## How to Build and Run

### C++ Implementation
Navigate to `Assignment05/cpp/build` and compile directly with `g++`:

```bash
cd Assignment05/cpp/build

# Compile and run unit tests
g++ -std=c++20 -I../include ../src/store.cpp ../src/transaction.cpp ../tests/store_tests.cpp -o store_tests
./store_tests

# Compile and run interactive shell
g++ -std=c++20 -I../include ../src/store.cpp ../src/transaction.cpp ../src/main.cpp -o kv_store
./kv_store

==========================================

OCaml Implementation (WSL)
Navigate to Assignment05/ocaml in WSL:

# Bash
cd Assignment05/ocaml

# Build project, run tests, and start interactive shell
dune build
dune runtest
dune exec ./bin/main.exe

==========================================

Interactive Shell Session Logs
C++ Interactive Session (kv_store)
$ ./kv_store
> SET name Ada
> SET language C++
> SET x 10
> SET y 2
> GET name
Ada
> GET x
10
> LIST
language = C++
name = Ada
x = 10
y = 2
> DELETE language
> LIST
name = Ada
x = 10
y = 2
> SAVE data.txt
> QUIT

==========================================

OCaml Interactive Session (main.exe)
$ dune exec ./bin/main.exe
> SET name Ada
> SET language OCaml
> SET x 10
> SET y 2
> GET name
Ada
> GET x
10
> LIST
language = OCaml
name = Ada
x = 10
y = 2
> DELETE language
> LIST
name = Ada
x = 10
y = 2
> SAVE data.txt
> QUIT

==========================================

Running SAVE data.txt in either interactive shell outputs the following remaining key-value pairs:
name Ada
x 10
y 2