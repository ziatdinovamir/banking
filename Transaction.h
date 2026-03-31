#pragma once
#include <string>
#include <chrono>
#include "ITransactionValidator.h"

namespace banking {

class Transaction {
public:
    enum class Type {
        DEPOSIT,
        WITHDRAWAL,
        TRANSFER
    };
    
    Transaction(Type type, double amount, const std::string& description = "");
    
    Type getType() const;
    double getAmount() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getTimestamp() const;
    
    bool execute(double& balance, ITransactionValidator* validator = nullptr);
    std::string toString() const;
    
private:
    Type type_;
    double amount_;
    std::string description_;
    std::chrono::system_clock::time_point timestamp_;
};

} // namespace banking
