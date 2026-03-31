#pragma once
#include <gmock/gmock.h>
#include "ITransactionValidator.h"

namespace banking {

class MockTransactionValidator : public ITransactionValidator {
public:
    MOCK_METHOD(bool, validate, (double amount, double balance), (const, override));
};

} // namespace banking
