#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "ITransaction.h"

class Account;  // Forward declaration

class Deposit : public ITransaction {
private:
    Account* account;
    double amount;
    std::string description;
    std::string timestamp;
    TransactionType type;

public:
    Deposit(Account* account, double amount, const std::string& description = "Deposit");
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;
    double getAmount() const override { return amount; }
    std::string getTimestamp() const override { return timestamp; }
    TransactionType getType() const override { return TransactionType::DEPOSIT; }
};

class Withdrawal : public ITransaction {
private:
    Account* account;
    double amount;
    std::string description;
    std::string timestamp;
    TransactionType type;

public:
    Withdrawal(Account* account, double amount, const std::string& description = "Withdrawal");
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;
    double getAmount() const override { return amount; }
    std::string getTimestamp() const override { return timestamp; }
    TransactionType getType() const override { return TransactionType::WITHDRAWAL; }
};

class Transfer : public ITransaction {
private:
    Account* fromAccount;
    Account* toAccount;
    double amount;
    std::string description;
    std::string timestamp;
    TransactionType type;

public:
    Transfer(Account* from, Account* to, double amount, const std::string& description = "Transfer");
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;
    double getAmount() const override { return amount; }
    std::string getTimestamp() const override { return timestamp; }
    TransactionType getType() const override { return TransactionType::TRANSFER; }
}; 