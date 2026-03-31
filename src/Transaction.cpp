#include "banking/Transaction.h"
#include "banking/Account.h"
#include <string>

namespace banking {

Transaction::Transaction(std::shared_ptr<Account> from, std::shared_ptr<Account> to, double amount)
    : from_(from), to_(to), amount_(amount), executed_(false), rolledBack_(false) {}

bool Transaction::execute() {
    if (executed_ || rolledBack_) {
        return false;
    }
    
    if (!from_ || !to_) {
        return false;
    }
    
    if (!from_->withdraw(amount_)) {
        return false;
    }
    
    to_->deposit(amount_);
    executed_ = true;
    return true;
}

bool Transaction::rollback() {
    if (!executed_ || rolledBack_) {
        return false;
    }
    
    if (!from_ || !to_) {
        return false;
    }
    
    if (!to_->withdraw(amount_)) {
        return false;
    }
    
    from_->deposit(amount_);
    rolledBack_ = true;
    return true;
}

std::string Transaction::getStatus() const {
    if (rolledBack_) return "ROLLED_BACK";
    if (executed_) return "EXECUTED";
    return "PENDING";
}

} // namespace banking
