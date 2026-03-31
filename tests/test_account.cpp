#include <gtest/gtest.h>
#include <memory>
#include "banking/Account.h"

using namespace banking;

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = std::make_unique<Account>(1, 100.0);
    }
    
    std::unique_ptr<Account> account;
};

TEST_F(AccountTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(account->getId(), 1);
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountTest, DepositIncreasesBalance) {
    account->deposit(50.0);
    EXPECT_DOUBLE_EQ(account->getBalance(), 150.0);
}

TEST_F(AccountTest, DepositThrowsOnNegativeAmount) {
    EXPECT_THROW(account->deposit(-10.0), std::invalid_argument);
}

TEST_F(AccountTest, WithdrawDecreasesBalance) {
    EXPECT_TRUE(account->withdraw(30.0));
    EXPECT_DOUBLE_EQ(account->getBalance(), 70.0);
}

TEST_F(AccountTest, WithdrawFailsWhenInsufficientFunds) {
    EXPECT_FALSE(account->withdraw(150.0));
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountTest, WithdrawThrowsOnNegativeAmount) {
    EXPECT_THROW(account->withdraw(-10.0), std::invalid_argument);
}

TEST_F(AccountTest, ConstructorWithDefaultBalance) {
    Account acc(1);
    EXPECT_EQ(acc.getId(), 1);
    EXPECT_DOUBLE_EQ(acc.getBalance(), 0.0);
}

TEST_F(AccountTest, DestructorWorks) {
    auto acc = std::make_unique<Account>(1, 100.0);
    acc.reset();
    SUCCEED();
}

TEST_F(AccountTest, GetIdReturnsCorrectValue) {
    EXPECT_EQ(account->getId(), 1);
}

TEST_F(AccountTest, GetBalanceReturnsCorrectValue) {
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountTest, WithdrawAfterMultipleDeposits) {
    account->deposit(50.0);
    account->deposit(30.0);
    EXPECT_TRUE(account->withdraw(80.0));
    EXPECT_DOUBLE_EQ(account->getBalance(), 100.0);
}

TEST_F(AccountTest, DepositAfterWithdrawal) {
    EXPECT_TRUE(account->withdraw(50.0));
    account->deposit(25.0);
    EXPECT_DOUBLE_EQ(account->getBalance(), 75.0);
}
