#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "banking/Transaction.h"
#include "banking/Account.h"

using namespace banking;
using ::testing::_;
using ::testing::Return;

class MockAccount : public Account {
public:
    MockAccount(int id, double balance) : Account(id, balance) {}
    
    MOCK_METHOD(void, deposit, (double amount), (override));
    MOCK_METHOD(bool, withdraw, (double amount), (override));
};

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        from = std::make_shared<MockAccount>(1, 100.0);
        to = std::make_shared<MockAccount>(2, 50.0);
    }
    
    std::shared_ptr<MockAccount> from;
    std::shared_ptr<MockAccount> to;
};

TEST_F(TransactionTest, ExecuteSuccessfulTransfer) {
    EXPECT_CALL(*from, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*to, deposit(30.0))
        .Times(1);
    
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "EXECUTED");
}

TEST_F(TransactionTest, ExecuteFailsWhenWithdrawFails) {
    EXPECT_CALL(*from, withdraw(30.0))
        .WillOnce(Return(false));
    EXPECT_CALL(*to, deposit(_))
        .Times(0);
    
    Transaction transaction(from, to, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, RollbackRestoresState) {
    EXPECT_CALL(*from, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*to, deposit(30.0))
        .Times(1);
    EXPECT_CALL(*to, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*from, deposit(30.0))
        .Times(1);
    
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_TRUE(transaction.rollback());
    EXPECT_EQ(transaction.getStatus(), "ROLLED_BACK");
}

TEST_F(TransactionTest, ExecuteWithNegativeAmount) {
    Transaction transaction(from, to, -10.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, RollbackWithNegativeAmount) {
    Transaction transaction(from, to, -10.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, ExecuteFailsWhenFromAccountIsNull) {
    std::shared_ptr<Account> nullFrom = nullptr;
    Transaction transaction(nullFrom, to, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, ExecuteFailsWhenToAccountIsNull) {
    std::shared_ptr<Account> nullTo = nullptr;
    Transaction transaction(from, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, ExecuteWithZeroAmount) {
    EXPECT_CALL(*from, withdraw(0.0))
        .WillOnce(Return(false));
    EXPECT_CALL(*to, deposit(_))
        .Times(0);
    
    Transaction transaction(from, to, 0.0);
    EXPECT_FALSE(transaction.execute());
}

TEST_F(TransactionTest, ExecuteAfterRollbackFails) {
    EXPECT_CALL(*from, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*to, deposit(30.0))
        .Times(1);
    EXPECT_CALL(*to, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*from, deposit(30.0))
        .Times(1);
    
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_TRUE(transaction.rollback());
    EXPECT_FALSE(transaction.execute());
    EXPECT_EQ(transaction.getStatus(), "ROLLED_BACK");
}

TEST_F(TransactionTest, GetStatusPending) {
    Transaction transaction(from, to, 30.0);
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, RollbackFailsWhenWithdrawFromToFails) {
    EXPECT_CALL(*from, withdraw(30.0))
        .WillOnce(Return(true));
    EXPECT_CALL(*to, deposit(30.0))
        .Times(1);
    EXPECT_CALL(*to, withdraw(30.0))
        .WillOnce(Return(false));
    EXPECT_CALL(*from, deposit(_))
        .Times(0);
    
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
    EXPECT_EQ(transaction.getStatus(), "EXECUTED");
}

TEST_F(TransactionTest, ExecuteWithRealAccounts) {
    auto realFrom = std::make_shared<Account>(1, 100.0);
    auto realTo = std::make_shared<Account>(2, 50.0);
    
    Transaction transaction(realFrom, realTo, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_DOUBLE_EQ(realFrom->getBalance(), 70.0);
    EXPECT_DOUBLE_EQ(realTo->getBalance(), 80.0);
    EXPECT_EQ(transaction.getStatus(), "EXECUTED");
}

TEST_F(TransactionTest, ExecuteFailsWithInsufficientFunds) {
    auto realFrom = std::make_shared<Account>(1, 30.0);
    auto realTo = std::make_shared<Account>(2, 50.0);
    
    Transaction transaction(realFrom, realTo, 50.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_DOUBLE_EQ(realFrom->getBalance(), 30.0);
    EXPECT_DOUBLE_EQ(realTo->getBalance(), 50.0);
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(TransactionTest, RollbackWithRealAccounts) {
    auto realFrom = std::make_shared<Account>(1, 100.0);
    auto realTo = std::make_shared<Account>(2, 50.0);
    
    Transaction transaction(realFrom, realTo, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_TRUE(transaction.rollback());
    EXPECT_DOUBLE_EQ(realFrom->getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(realTo->getBalance(), 50.0);
    EXPECT_EQ(transaction.getStatus(), "ROLLED_BACK");
}

TEST_F(TransactionTest, RollbackFailsWhenFromAccountIsNullAfterExecute) {
    auto realTo = std::make_shared<Account>(2, 50.0);
    std::shared_ptr<Account> nullFrom = nullptr;
    
    Transaction transaction(nullFrom, realTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(TransactionTest, RollbackFailsWhenToAccountIsNullAfterExecute) {
    auto realFrom = std::make_shared<Account>(1, 100.0);
    std::shared_ptr<Account> nullTo = nullptr;
    
    Transaction transaction(realFrom, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(TransactionTest, RollbackFailsWhenBothAccountsAreNull) {
    std::shared_ptr<Account> nullFrom = nullptr;
    std::shared_ptr<Account> nullTo = nullptr;
    
    Transaction transaction(nullFrom, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}
