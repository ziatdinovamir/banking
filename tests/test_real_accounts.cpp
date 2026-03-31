#include <gtest/gtest.h>
#include "banking/Account.h"
#include "banking/Transaction.h"

using namespace banking;

class RealAccountsTest : public ::testing::Test {
protected:
    void SetUp() override {
        from = std::make_shared<Account>(1, 100.0);
        to = std::make_shared<Account>(2, 50.0);
    }
    
    std::shared_ptr<Account> from;
    std::shared_ptr<Account> to;
};

TEST_F(RealAccountsTest, AccountDeposit) {
    from->deposit(50.0);
    EXPECT_DOUBLE_EQ(from->getBalance(), 150.0);
}

TEST_F(RealAccountsTest, AccountWithdraw) {
    EXPECT_TRUE(from->withdraw(30.0));
    EXPECT_DOUBLE_EQ(from->getBalance(), 70.0);
}

TEST_F(RealAccountsTest, AccountWithdrawInsufficient) {
    EXPECT_FALSE(from->withdraw(150.0));
    EXPECT_DOUBLE_EQ(from->getBalance(), 100.0);
}

TEST_F(RealAccountsTest, AccountDepositNegativeThrows) {
    EXPECT_THROW(from->deposit(-10.0), std::invalid_argument);
}

TEST_F(RealAccountsTest, AccountWithdrawNegativeThrows) {
    EXPECT_THROW(from->withdraw(-10.0), std::invalid_argument);
}

TEST_F(RealAccountsTest, TransactionExecuteWithRealAccounts) {
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_DOUBLE_EQ(from->getBalance(), 70.0);
    EXPECT_DOUBLE_EQ(to->getBalance(), 80.0);
    EXPECT_EQ(transaction.getStatus(), "EXECUTED");
}

TEST_F(RealAccountsTest, TransactionExecuteInsufficientFunds) {
    Transaction transaction(from, to, 150.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_DOUBLE_EQ(from->getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(to->getBalance(), 50.0);
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(RealAccountsTest, TransactionRollbackWithRealAccounts) {
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_TRUE(transaction.rollback());
    EXPECT_DOUBLE_EQ(from->getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(to->getBalance(), 50.0);
    EXPECT_EQ(transaction.getStatus(), "ROLLED_BACK");
}

TEST_F(RealAccountsTest, TransactionRollbackWithoutExecute) {
    Transaction transaction(from, to, 30.0);
    EXPECT_FALSE(transaction.rollback());
    EXPECT_EQ(transaction.getStatus(), "PENDING");
}

TEST_F(RealAccountsTest, TransactionExecuteTwiceFails) {
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_FALSE(transaction.execute());
    EXPECT_DOUBLE_EQ(from->getBalance(), 70.0);
    EXPECT_DOUBLE_EQ(to->getBalance(), 80.0);
}

TEST_F(RealAccountsTest, TransactionRollbackTwiceFails) {
    Transaction transaction(from, to, 30.0);
    EXPECT_TRUE(transaction.execute());
    EXPECT_TRUE(transaction.rollback());
    EXPECT_FALSE(transaction.rollback());
    EXPECT_DOUBLE_EQ(from->getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(to->getBalance(), 50.0);
}

TEST_F(RealAccountsTest, TransactionExecuteWithZeroAmount) {
    Transaction transaction(from, to, 0.0);
    EXPECT_THROW(transaction.execute(), std::invalid_argument);
}

TEST_F(RealAccountsTest, TransactionExecuteWithNegativeAmount) {
    Transaction transaction(from, to, -10.0);
    EXPECT_THROW(transaction.execute(), std::invalid_argument);
}

TEST_F(RealAccountsTest, TransactionRollbackWithNullFromAfterExecute) {
    auto realTo = std::make_shared<Account>(2, 50.0);
    std::shared_ptr<Account> nullFrom = nullptr;
    
    Transaction transaction(nullFrom, realTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(RealAccountsTest, TransactionRollbackWithNullToAfterExecute) {
    auto realFrom = std::make_shared<Account>(1, 100.0);
    std::shared_ptr<Account> nullTo = nullptr;
    
    Transaction transaction(realFrom, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(RealAccountsTest, TransactionRollbackWithBothNullAfterExecute) {
    std::shared_ptr<Account> nullFrom = nullptr;
    std::shared_ptr<Account> nullTo = nullptr;
    
    Transaction transaction(nullFrom, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(RealAccountsTest, TransactionRollbackAfterFailedExecuteWithNull) {
    auto realTo = std::make_shared<Account>(2, 50.0);
    std::shared_ptr<Account> nullFrom = nullptr;
    
    Transaction transaction(nullFrom, realTo, 30.0);
    transaction.execute();
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(RealAccountsTest, TransactionRollbackCoverNullCheck) {
    auto realFrom = std::make_shared<Account>(1, 100.0);
    std::shared_ptr<Account> nullTo = nullptr;
    
    Transaction transaction(realFrom, nullTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());
}

TEST_F(RealAccountsTest, FinalCoverNullCheck) {
    auto acc = std::make_shared<Account>(1, 100.0);
    std::shared_ptr<Account> nullAcc = nullptr;
    
    Transaction tx1(acc, nullAcc, 50.0);
    EXPECT_FALSE(tx1.execute());
    EXPECT_FALSE(tx1.rollback());
    
    Transaction tx2(nullAcc, acc, 50.0);
    EXPECT_FALSE(tx2.execute());
    EXPECT_FALSE(tx2.rollback());
}

TEST_F(RealAccountsTest, CoverRollbackNullptrCheck) {
    auto realTo = std::make_shared<Account>(2, 50.0);
    std::shared_ptr<Account> nullFrom = nullptr;
    
    Transaction transaction(nullFrom, realTo, 30.0);
    EXPECT_FALSE(transaction.execute());
    EXPECT_FALSE(transaction.rollback());  // Это покроет строку 33-34 в rollback()
}
