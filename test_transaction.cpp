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

TEST_F(TransactionTest, GetTimestamp) {
    auto timestamp = depositTransaction->getTimestamp();
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
    EXPECT_LE(diff.count(), 1);
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

TEST_F(TransactionTest, ExecuteWithdrawalExactBalance) {
    Transaction exact(Transaction::Type::WITHDRAWAL, 50.0);
    double balance = 50.0;
    EXPECT_TRUE(exact.execute(balance));
    EXPECT_EQ(balance, 0.0);
}

TEST_F(TransactionTest, ExecuteTransferValid) {
    double balance = 500.0;
    EXPECT_TRUE(transferTransaction->execute(balance));
    EXPECT_EQ(balance, 425.0);
}

TEST_F(TransactionTest, ExecuteTransferInsufficientFunds) {
    double balance = 50.0;
    EXPECT_FALSE(transferTransaction->execute(balance));
    EXPECT_EQ(balance, 50.0);
}

TEST_F(TransactionTest, ExecuteTransferExactBalance) {
    Transaction exact(Transaction::Type::TRANSFER, 75.0);
    double balance = 75.0;
    EXPECT_TRUE(exact.execute(balance));
    EXPECT_EQ(balance, 0.0);
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

TEST_F(TransactionTest, ExecuteWithValidatorRejecting) {
    MockTransactionValidator validator;
    double balance = 500.0;
    
    EXPECT_CALL(validator, validate(100.0, 500.0))
        .Times(Exactly(1))
        .WillOnce(Return(false));
    
    EXPECT_FALSE(depositTransaction->execute(balance, &validator));
    EXPECT_EQ(balance, 500.0);
}

TEST_F(TransactionTest, ToString) {
    std::string result = depositTransaction->toString();
    EXPECT_THAT(result, testing::HasSubstr("DEPOSIT"));
    EXPECT_THAT(result, testing::HasSubstr("$100.00"));
    EXPECT_THAT(result, testing::HasSubstr("Test deposit"));
}

TEST_F(TransactionTest, ToStringWithoutDescription) {
    Transaction trans(Transaction::Type::WITHDRAWAL, 200.0);
    std::string result = trans.toString();
    EXPECT_THAT(result, testing::HasSubstr("WITHDRAWAL"));
    EXPECT_THAT(result, testing::HasSubstr("$200.00"));
    EXPECT_THAT(result, testing::Not(testing::HasSubstr(" - ")));
}

TEST_F(TransactionTest, ToStringForTransfer) {
    Transaction transfer(Transaction::Type::TRANSFER, 150.0, "Test transfer");
    std::string result = transfer.toString();
    EXPECT_THAT(result, testing::HasSubstr("TRANSFER"));
    EXPECT_THAT(result, testing::HasSubstr("$150.00"));
    EXPECT_THAT(result, testing::HasSubstr("Test transfer"));
}

TEST_F(TransactionTest, ConstructorWithoutDescription) {
    Transaction trans(Transaction::Type::TRANSFER, 100.0);
    EXPECT_EQ(trans.getType(), Transaction::Type::TRANSFER);
    EXPECT_EQ(trans.getAmount(), 100.0);
    EXPECT_EQ(trans.getDescription(), "");
}
