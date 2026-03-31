#pragma once
#include <memory>
#include <string>

namespace banking {

class Account;

class Transaction {
public:
    Transaction(std::shared_ptr<Account> from, std::shared_ptr<Account> to, double amount);
    
    bool execute();
    bool rollback();
    std::string getStatus() const;
    
private:
    std::shared_ptr<Account> from_;
    std::shared_ptr<Account> to_;
    double amount_;
    bool executed_;
    bool rolledBack_;
};

} // namespace banking
