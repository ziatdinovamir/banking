#include <gtest/gtest.h>
#include "Account.h"

using namespace banking;

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = std::make_unique<Account>(1, 500);
    }
    
    std::unique_ptr<Account> account;
};

TEST_F(AccountTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(account->getId(), 1);
    EXPECT_EQ(account->getBalance(), 500);
}

TEST_F(AccountTest, DepositIncreasesBalance) {
    account->deposit(100);
    EXPECT_EQ(account->getBalance(), 600);
}

TEST_F(AccountTest, DepositThrowsOnNegativeAmount) {
    EXPECT_THROW(account->deposit(-50), std::invalid_argument);
}

TEST_F(AccountTest, DepositThrowsOnZeroAmount) {
    EXPECT_THROW(account->deposit(0), std::invalid_argument);
}

TEST_F(AccountTest, WithdrawRequiresLock) {
    EXPECT_THROW(account->withdraw(100), std::runtime_error);
}

TEST_F(AccountTest, WithdrawWorksWhenLocked) {
    account->lock();
    EXPECT_TRUE(account->withdraw(100));
    EXPECT_EQ(account->getBalance(), 400);
}

TEST_F(AccountTest, WithdrawFailsWhenInsufficientFunds) {
    account->lock();
    EXPECT_FALSE(account->withdraw(600));
    EXPECT_EQ(account->getBalance(), 500);
}

TEST_F(AccountTest, WithdrawThrowsOnNegativeAmount) {
    account->lock();
    EXPECT_THROW(account->withdraw(-50), std::invalid_argument);
}

TEST_F(AccountTest, WithdrawThrowsOnZeroAmount) {
    account->lock();
    EXPECT_THROW(account->withdraw(0), std::invalid_argument);
}

TEST_F(AccountTest, LockWorks) {
    EXPECT_NO_THROW(account->lock());
    EXPECT_THROW(account->lock(), std::runtime_error);
}

TEST_F(AccountTest, UnlockWorks) {
    account->lock();
    EXPECT_NO_THROW(account->unlock());
    EXPECT_NO_THROW(account->lock());
}

TEST_F(AccountTest, WithdrawExactBalance) {
    account->lock();
    EXPECT_TRUE(account->withdraw(500));
    EXPECT_EQ(account->getBalance(), 0);
}

TEST_F(AccountTest, MultipleOperations) {
    account->deposit(100);
    account->lock();
    account->withdraw(200);
    account->unlock();
    account->deposit(50);
    account->lock();
    account->withdraw(100);
    EXPECT_EQ(account->getBalance(), 350);
}
