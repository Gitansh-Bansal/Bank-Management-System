#include "../include/Bank.h"
#include "../include/SavingsAccount.h"
#include "../include/CurrentAccount.h"
#include "../include/AuditableSavingsAccount.h"
#include "../include/Transaction.h"
#include "../include/Database.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

Bank::Bank(const std::string& name)
    : name(name) {
    if (name.empty()) {
        throw std::invalid_argument("Bank name cannot be empty");
    }
}

// int Bank::generateCustomerId() {
//     int id = Database::getNextCustomerId();
//     Database::incrementCustomerId();
//     return id;
// }

// int Bank::generateAccountNumber() {
//     int number = Database::getNextAccountNumber();
//     Database::incrementAccountNumber();
//     return number;
// }

// Customer* Bank::addCustomer(const std::string& name, const std::string& phone) {
//     auto customer = std::make_unique<Customer>(generateCustomerId(), name, phone);
//     Customer* customerPtr = customer.get();
//     customers.push_back(std::move(customer));
//     return customerPtr;
// }

// Customer* Bank::findCustomer(int customerId) const {
//     auto it = std::find_if(customers.begin(), customers.end(),
//         [customerId](const auto& customer) {
//             return customer->getId() == customerId;
//         });
//     return it != customers.end() ? it->get() : nullptr;
// }

// bool Bank::removeCustomer(int customerId) {
//     auto it = std::find_if(customers.begin(), customers.end(),
//         [customerId](const auto& customer) {
//             return customer->getId() == customerId;
//         });
    
//     if (it != customers.end()) {
//         customers.erase(it);
//         return true;
//     }
//     return false;
// }

std::unique_ptr<Account> Bank::createSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = Database::getInstance()->findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = Database::getNextAccountNumber();
    Database::incrementAccountNumber();
    std::cout<<"Account Number fetch in Bank.cpp: "<<accountNumber<<std::endl; // correct ////////
    auto account = std::make_unique<SavingsAccount>(accountNumber, initialBalance, customer);
    std::cout<<"Created with: "<<account->getAccountNumber()<<std::endl;
    return account;
}

std::unique_ptr<Account> Bank::createCurrentAccount(int customerId, double initialBalance) {
    Customer* customer = Database::getInstance()->findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = Database::getNextAccountNumber();
    Database::incrementAccountNumber();
    auto account = std::make_unique<CurrentAccount>(accountNumber, initialBalance, customer);
    return account;
}

std::unique_ptr<Account> Bank::createAuditableSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = Database::getInstance()->findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = Database::getNextAccountNumber();
    Database::incrementAccountNumber();
    auto account = std::make_unique<AuditableSavingsAccount>(accountNumber, initialBalance, customer);
    return account;
}

// Account* Bank::findAccount(int accountNumber) const {
//     auto it = std::find_if(accounts.begin(), accounts.end(),
//         [accountNumber](const auto& account) {
//             return account->getAccountNumber() == accountNumber;
//         });
//     return it != accounts.end() ? it->get() : nullptr;
// }

// bool Bank::closeAccount(int accountNumber) {
//     return Database::getInstance()->removeAccount(accountNumber);
// }

bool Bank::processDeposit(int accountNumber, double amount) {
    Account* account = Database::getInstance()->findAccount(accountNumber);
    if (!account) {
        return false;
    }
    return account->deposit(amount);
}

bool Bank::processWithdrawal(int accountNumber, double amount) {
    Account* account = Database::getInstance()->findAccount(accountNumber);
    if (!account) {
        return false;
    }
    return account->withdraw(amount);
}

bool Bank::processTransfer(int fromAccount, int toAccount, double amount) {
    Account* from = Database::getInstance()->findAccount(fromAccount);
    Account* to = Database::getInstance()->findAccount(toAccount);
    
    if (!from || !to) {
        return false;
    }
    
    auto transfer = std::make_unique<Transfer>(from, to, amount);
    return transfer->execute();
}

// void Bank::applyMonthlyUpdates() {
//     for (auto& account : accounts) {
//         account->applyMonthlyUpdate();
//     }
// }

const std::string& Bank::getName() const {
    return name;
}

// const std::vector<std::unique_ptr<Customer>>& Bank::getCustomers() const {
//     return customers;
// }

// const std::vector<std::unique_ptr<Account>>& Bank::getAccounts() const {
//     return accounts;
// } 