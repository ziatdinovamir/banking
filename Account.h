#pragma once

namespace banking {

class Account {
public:
    Account(int id, int balance);
    virtual ~Account() = default;
    
    int getId() const;
    int getBalance() const;
    virtual void deposit(int amount);
    virtual bool withdraw(int amount);
    virtual void lock();
    virtual void unlock();
    
private:
    int id_;
    int balance_;
    bool isLocked_;
};

} // namespace banking
