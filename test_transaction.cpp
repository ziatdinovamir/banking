#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "ITransactionValidator.h"
#include "mocks/MockTransactionValidator.h"

using namespace banking;
using ::testing::_;
using ::testing::Return;
using ::testing::Exactly;

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        depositTransaction = std::make_unique<Transaction>(
            Transaction::Type::DEPOSIT, 100.0, "Test deposit");
        withdrawalTransaction = std::make_unique<Transaction>(
            Transaction::Type::WITHDRAWAL, 50.0, "Test withdrawal");
        transferTransaction = std::make_unique<Transaction>(
            Transaction::Type::TRANSFER, 75.0, "Test transfer");
    }
    
    std::unique_ptr<Transaction> depositTransaction;
    std::unique_ptr<Transaction> withdrawalTransaction;
    std::unique_ptr<Transaction> transferTransaction;
};

TEST_F(TransactionTest, ConstructorAndGetters) {
    EXPECT_EQ(depositTransaction->getType(), Transaction::Type::DEPOSIT);
    EXPECT_EQ(depositTransaction->getAmount(), 100.0);
    EXPECT_EQ(depositTransaction->getDescription(), "Test deposit");
}

TEST_F(TransactionTest, ExecuteDeposit) {
    double balance = 500.0;
    EXPECT_TRUE(depositTransaction->execute(balance));
    EXPECT_EQ(balance, 600.0);
}

TEST_F(TransactionTest, ExecuteWithdrawalValid) {
    double balance = 500.0;
    EXPECT_TRUE(withdrawalTransaction->execute(balance));
    EXPECT_EQ(balance, 450.0);
}

TEST_F(TransactionTest, ExecuteWithdrawalInsufficientFunds) {
    double balance = 30.0;
    EXPECT_FALSE(withdrawalTransaction->execute(balance));
    EXPECT_EQ(balance, 30.0);
}

TEST_F(TransactionTest, ExecuteTransferValid) {
    double balance = 500.0;
    EXPECT_TRUE(transferTransaction->execute(balance));
    EXPECT_EQ(balance, 425.0);
}

TEST_F(TransactionTest, ExecuteWithValidator) {
    MockTransactionValidator validator;
    double balance = 500.0;
    
    EXPECT_CALL(validator, validate(100.0, 500.0))
        .Times(Exactly(1))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(depositTransaction->execute(balance, &validator));
    EXPECT_EQ(balance, 600.0);
}

TEST_F(TransactionTest, ToString) {
    std::string result = depositTransaction->toString();
    EXPECT_THAT(result, testing::HasSubstr("DEPOSIT"));
    EXPECT_THAT(result, testing::HasSubstr("$100.00"));
    EXPECT_THAT(result, testing::HasSubstr("Test deposit"));
}
