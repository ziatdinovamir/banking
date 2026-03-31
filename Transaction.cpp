#include "Transaction.h"
#include <sstream>
#include <iomanip>

namespace banking {

Transaction::Transaction(Type type, double amount, const std::string& description)
    : type_(type), amount_(amount), description_(description), 
      timestamp_(std::chrono::system_clock::now()) {}

Transaction::Type Transaction::getType() const { return type_; }
double Transaction::getAmount() const { return amount_; }
std::string Transaction::getDescription() const { return description_; }
std::chrono::system_clock::time_point Transaction::getTimestamp() const { return timestamp_; }

bool Transaction::execute(double& balance, ITransactionValidator* validator) {
    if (validator && !validator->validate(amount_, balance)) {
        return false;
    }
    
    switch (type_) {
        case Type::DEPOSIT:
            balance += amount_;
            return true;
            
        case Type::WITHDRAWAL:
            if (balance >= amount_) {
                balance -= amount_;
                return true;
            }
            return false;
            
        case Type::TRANSFER:
            if (balance >= amount_) {
                balance -= amount_;
                return true;
            }
            return false;
    }
    return false;
}

std::string Transaction::toString() const {
    std::ostringstream oss;
    
    std::string typeStr;
    switch (type_) {
        case Type::DEPOSIT: typeStr = "DEPOSIT"; break;
        case Type::WITHDRAWAL: typeStr = "WITHDRAWAL"; break;
        case Type::TRANSFER: typeStr = "TRANSFER"; break;
    }
    
    oss << "[" << typeStr << "] $" << std::fixed << std::setprecision(2) << amount_;
    if (!description_.empty()) {
        oss << " - " << description_;
    }
    
    return oss.str();
}

} // namespace banking
