# Assignment 05: Side Effects, State, and Transactions in C++ and OCaml

## 1. Side Effects & Purity
In the C++ implementation, every member function of the `Store` class that modifies internal state (`set`, `remove`, `load`) introduces side effects by altering memory in-place. Furthermore, file operations (`save`, `load`) perform I/O side effects. In contrast, core OCaml store manipulation functions (`set`, `delete`, `get`, `empty`) are pure functions. Given the same inputs, `set` returns a new immutable map without mutating the existing map or modifying external memory. Side effects in OCaml are strictly isolated to file I/O operations (`save`, `load`) and console interactions (`main.ml`).

## 2. Location of Mutable State
In C++, mutable state resides on the heap inside the `std::unordered_map` member variable of the `Store` instance. This state is mutated in-place as operations are called. In OCaml, mutable state does not exist within the store data structure itself. Instead, state evolution is represented functionally as an immutable value passed across recursive function calls in the command line loop (`loop current_store`).

## 3. Control of Side Effects: RAII vs. Immutability
C++ isolates side effects and resource lifecycles using RAII (Resource Acquisition Is Initialization). File streams (`std::ifstream`, `std::ofstream`) open resources during object construction and automatically flush and close them via destructors upon scope exit. Similarly, transactions rely on RAII destructors to guarantee state cleanup. OCaml controls side effects by enforcing value immutability; operations cannot accidentally alter shared data structures because modifications produce distinct new data instances.

## 4. Transaction Rollback Mechanics
Rollback in C++ requires explicit state preservation and manual restoration. When a `Transaction` object is instantiated, it makes a full copy (`backup_`) of the `Store`. If `commit()` is not invoked before the transaction object goes out of scope, the destructor restores `store_ = backup_`. In OCaml, rollback requires no state copying. Because `StringMap.t` is persistent, adding or deleting keys creates a new map reference while leaving the `original` map completely unmodified in memory. Rolling back a transaction simply means discarding the new map reference and retaining the `original` reference.

## 5. Persistence and Structural Sharing
The OCaml implementation can retain earlier states without duplicating the entire map in memory because `Map.Make` uses balanced persistent AVL trees. When a key is inserted or removed, OCaml reuses unchanged tree branches (structural sharing) and creates only a path of new nodes to the modified element. Thus, snapshotting prior states has $O(\log N)$ time and memory overhead.

## 6. Model Comparison
The OCaml programming model makes transaction rollback significantly easier and safer to express. Rollback in OCaml is an inherent property of immutable persistence: abandoning a failed transaction requires doing nothing other than returning the untouched input reference. C++ requires explicit state duplication, active lifetime tracking, and RAII destructors to guarantee that modified state is overwritten back to its original condition.