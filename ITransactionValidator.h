#pragma once

namespace banking {

class ITransactionValidator {
public:
    virtual ~ITransactionValidator() = default;
    virtual bool validate(double amount, double balance) const = 0;
};

} // namespace banking
