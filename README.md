# Banking

## Status

[![CI](https://github.com/ziatdinovamir/banking/actions/workflows/ci.yml/badge.svg)](https://github.com/ziatdinovamir/banking/actions/workflows/ci.yml)


## Coverage

[![Coverage Status](https://coveralls.io/repos/github/ziatdinovamir/banking/badge.svg?branch=main)](https://coveralls.io/github/ziatdinovamir/banking?branch=main)

## Homework

### Задание
1. Создайте `CMakeList.txt` для библиотеки *banking*.
2. Создайте модульные тесты на классы `Transaction` и `Account`.
    * Используйте mock-объекты.
    * Покрытие кода должно составлять 100%.
3. Настройте сборочную процедуру на **TravisCI**.
4. Настройте [Coveralls.io](https://coveralls.io/).

Создаем отдельную директорию для домашнего задания
```sh
cd ~/ziatdinovamir/workspace
mkdir banking
cd banking
```
Создаем аголовочный файл класса Account с методами для работы со счётом
```sh
cat > Account.h <<'EOF'
#pragma once
class Account {
 public:
  Account(int id, int balance);
  virtual ~Account();

  // Virtual to test.
  virtual int GetBalance() const;

  // Virtual to test.
  virtual void ChangeBalance(int diff);

  // Virtual to test.
  virtual void Lock();

  // Virtual to test.
  virtual void Unlock();
  int id() const { return id_; }

 private:
  int id_;
  int balance_;
  bool is_locked_;
};
EOF
```
Реализация методов класса Account
```sh
cat > Account.cpp <<'EOF'
#include "Account.h"
#include <stdexcept>

namespace banking {

Account::Account(int id, int balance) 
    : id_(id), balance_(balance), isLocked_(false) {}

int Account::getId() const { return id_; }
int Account::getBalance() const { return balance_; }

void Account::deposit(int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Deposit amount must be positive");
    }
    balance_ += amount;
}

bool Account::withdraw(int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Withdrawal amount must be positive");
    }
    if (!isLocked_) {
        throw std::runtime_error("Account must be locked before withdrawal");
    }
    if (amount > balance_) {
        return false;
    }
    balance_ -= amount;
    return true;
}

void Account::lock() {
    if (isLocked_) {
        throw std::runtime_error("Account already locked");
    }
    isLocked_ = true;
}

void Account::unlock() {
    isLocked_ = false;
}

} 
EOF
```
Интерфейс для валидатора транзакций
```sh
cat > ITransactionValidator.h <<'EOF'
#pragma once

namespace banking {

class ITransactionValidator {
public:
    virtual ~ITransactionValidator() = default;
    virtual bool validate(double amount, double balance) const = 0;
};

} 
EOF
```
Класс Transaction для операций пополнения, снятия и перевода
```sh
cat > Transaction.h <<'EOF'
#pragma once
#include <string>
#include <chrono>
#include "ITransactionValidator.h"

namespace banking {

class Transaction {
public:
    enum class Type {
        DEPOSIT,
        WITHDRAWAL,
        TRANSFER
    };
    
    Transaction(Type type, double amount, const std::string& description = "");
    
    Type getType() const;
    double getAmount() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getTimestamp() const;
    
    bool execute(double& balance, ITransactionValidator* validator = nullptr);
    std::string toString() const;
    
private:
    Type type_;
    double amount_;
    std::string description_;
    std::chrono::system_clock::time_point timestamp_;
};

} // namespace banking
EOF
```
Реализация методов Transaction
```sh
cat > Transaction.cpp <<'EOF'
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

}
EOF
```
Mock-класс для тестирования, позволяющий проверять вызовы методов
```sh
mkdir -p mocks
```
```sh
cat > mocks/MockTransactionValidator.h <<'EOF'
#pragma once
#include <gmock/gmock.h>
#include "ITransactionValidator.h"

namespace banking {

class MockTransactionValidator : public ITransactionValidator {
public:
    MOCK_METHOD(bool, validate, (double amount, double balance), (const, override));
};

}
EOF
```
Тесты для класса Account
```sh
cat > test_account.cpp <<'EOF'
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
EOF
```
Тесты для Transaction с использованием mock-объектов
```sh
cat > test_transaction.cpp <<'EOF'
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
EOF
```
CMakeLists.txt
```sh
cat > CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.14)
project(banking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)
option(COVERAGE "Enable coverage reporting" OFF)

add_library(banking STATIC
    Account.cpp
    Transaction.cpp
)

target_include_directories(banking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(COVERAGE)
    target_compile_options(banking PRIVATE --coverage -O0 -g)
    target_link_options(banking PRIVATE --coverage)
endif()

if(BUILD_TESTS)
    enable_testing()
    
    include(FetchContent)
    
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    FetchContent_MakeAvailable(googletest)
    
    add_executable(banking_test
        test_account.cpp
        test_transaction.cpp
    )
    
    target_link_libraries(banking_test PRIVATE banking gtest_main gmock)
    
    if(COVERAGE)
        target_compile_options(banking_test PRIVATE --coverage -O0 -g)
        target_link_options(banking_test PRIVATE --coverage)
        target_link_libraries(banking_test PRIVATE gcov)
    endif()
    
    target_include_directories(banking_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    )
    
    add_test(NAME banking_test COMMAND banking_test)
endif()
EOF
```
Сборка и тестирование
```sh
rm -rf build _deps
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build
./banking_test
```
Проверка покрытия кода
```sh
cd /home/amir/ziatdinovamir/workspace/banking
rm -rf build _deps
cmake -B build -DBUILD_TESTS=ON -DCOVERAGE=ON
cmake --build build
cd build
./banking_test
lcov --capture --directory . --output-file coverage.info --ignore-errors unused,mismatch
lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/tests/*' --output-file coverage.info --ignore-errors unused,mismatch
lcov --list coverage.info
genhtml coverage.info --output-directory coverage_report
```
Настройка GitHub Actions
```sh
mkdir -p .github/workflows
cat > .github/workflows/ci.yml <<'EOF'
name: CI

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

jobs:
  test:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        build_type: [Debug, Release]
    
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Configure CMake
      shell: bash
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DBUILD_TESTS=ON
    
    - name: Build
      shell: bash
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run tests
      working-directory: build
      shell: bash
      run: ctest -C ${{ matrix.build_type }} --output-on-failure

  coverage:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y lcov
    
    - name: Configure CMake with coverage
      run: cmake -B build -DBUILD_TESTS=ON -DCOVERAGE=ON
    
    - name: Build
      run: cmake --build build
    
    - name: Run tests
      working-directory: build
      run: ctest --output-on-failure
    
    - name: Collect coverage
      working-directory: build
      run: |
        lcov --capture --directory . --output-file coverage.info --ignore-errors unused,mismatch
        lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/tests/*' --output-file coverage.info --ignore-errors unused,mismatch
        lcov --list coverage.info
    
    - name: Upload to Coveralls
      uses: coverallsapp/github-action@v2
      with:
        file: build/coverage.info
        github-token: ${{ secrets.GITHUB_TOKEN }}
EOF
```
Отправка на GitHub
```sh
git add .
git commit -m "Add banking library with tests"
git push origin main 
```
Настройка Coveralls.io. 

1) Переходим на сайт https://coveralls.io

2) Авторизуемся через GitHub

3) Добавляем репозитории

4) Выбираем данный репозиторий к подлкючению
