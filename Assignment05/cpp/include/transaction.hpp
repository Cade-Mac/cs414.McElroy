#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "store.hpp"

class Transaction {
public:
    explicit Transaction(Store& store);
    ~Transaction();

    void commit();

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

private:
    Store& store_;
    Store backup_;
    bool committed_{false};
};

#endif