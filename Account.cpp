#include "Account.h"
#include <stdexcept>

namespace banking {

Account::Account(int id, int balance) 
    : id_(id), balance_(balance), isLocked_(false) {}

int Account::getId() const { return id_; }
int Account::getBalance() const { return balance_; }

void Account::deposit(int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Deposit amount must be positive");
    }
    balance_ += amount;
}

bool Account::withdraw(int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Withdrawal amount must be positive");
    }
    if (!isLocked_) {
        throw std::runtime_error("Account must be locked before withdrawal");
    }
    if (amount > balance_) {
        return false;
    }
    balance_ -= amount;
    return true;
}

void Account::lock() {
    if (isLocked_) {
        throw std::runtime_error("Account already locked");
    }
    isLocked_ = true;
}

void Account::unlock() {
    isLocked_ = false;
}

} // namespace banking
