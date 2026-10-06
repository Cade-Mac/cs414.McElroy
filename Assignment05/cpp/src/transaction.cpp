#include "transaction.hpp"

Transaction::Transaction(Store& store)
    : store_(store), backup_(store), committed_(false) {}

void Transaction::commit() {
    committed_ = true;
}

Transaction::~Transaction() {
    if (!committed_) {
        store_ = backup_; // RAII Rollback: restores prior state if not committed
    }
}