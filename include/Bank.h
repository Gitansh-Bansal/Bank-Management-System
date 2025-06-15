#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Customer.h"
#include "Account.h"

class Bank {
private:
    std::string name;
    std::vector<std::unique_ptr<Customer>> customers;
    std::vector<std::unique_ptr<Account>> accounts;
    int nextCustomerId;
    int nextAccountNumber;

public:
    Bank(const std::string& name);
    
    // ID generation
    int generateCustomerId();
    int generateAccountNumber();
    
    // Customer management
    Customer* addCustomer(const std::string& name, const std::string& phone);
    Customer* findCustomer(int customerId) const;
    bool removeCustomer(int customerId);
    
    // Account management
    Account* createSavingsAccount(int customerId, double initialBalance);
    Account* createCurrentAccount(int customerId, double initialBalance);
    Account* createAuditableSavingsAccount(int customerId, double initialBalance);
    Account* findAccount(int accountNumber) const;
    bool closeAccount(int accountNumber);
    
    // Transaction processing
    bool processDeposit(int accountNumber, double amount);
    bool processWithdrawal(int accountNumber, double amount);
    bool processTransfer(int fromAccount, int toAccount, double amount);
    
    // Monthly updates
    void applyMonthlyUpdates();
    
    // Getters
    const std::string& getName() const;
    const std::vector<std::unique_ptr<Customer>>& getCustomers() const;
    const std::vector<std::unique_ptr<Account>>& getAccounts() const;
}; 