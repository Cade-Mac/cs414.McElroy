#include "store.hpp"
#include "transaction.hpp"
#include <cassert>
#include <iostream>

void test_core_ops() {
    Store s;
    s.set("x", "10");
    assert(s.get("x") == "10");

    s.remove("x");
    assert(s.get("x") == std::nullopt);
    std::cout << "[PASS] C++ Core Operations Test\n";
}

void test_transaction_abort() {
    Store s;
    s.set("x", "10");

    {
        Transaction tx(s);
        s.set("x", "20");
        s.set("y", "30");
        // tx.commit() NOT called -> trigger destructor rollback
    }

    assert(s.get("x") == "10");
    assert(s.get("y") == std::nullopt);
    std::cout << "[PASS] C++ Transaction Abort Test\n";
}

void test_save_load() {
    Store s1;
    s1.set("a", "1");
    s1.set("b", "2");
    assert(s1.save("test_data.txt"));

    Store s2;
    assert(s2.load("test_data.txt"));
    assert(s2.get("a") == "1");
    assert(s2.get("b") == "2");
    std::cout << "[PASS] C++ Save/Load Test\n";
}

int main() {
    test_core_ops();
    test_transaction_abort();
    test_save_load();
    return 0;
}