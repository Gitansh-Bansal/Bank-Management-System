#include "../include/Bank.h"
#include "../include/SavingsAccount.h"
#include "../include/CurrentAccount.h"
#include "../include/AuditableSavingsAccount.h"
#include "../include/Transaction.h"
#include <algorithm>
#include <stdexcept>

Bank::Bank(const std::string& name)
    : name(name), nextCustomerId(1000), nextAccountNumber(10000) {
    if (name.empty()) {
        throw std::invalid_argument("Bank name cannot be empty");
    }
}

int Bank::generateCustomerId() {
    return nextCustomerId++;
}

int Bank::generateAccountNumber() {
    return nextAccountNumber++;
}

Customer* Bank::addCustomer(const std::string& name, const std::string& phone) {
    auto customer = std::make_unique<Customer>(generateCustomerId(), name, phone);
    Customer* customerPtr = customer.get();
    customers.push_back(std::move(customer));
    return customerPtr;
}

Customer* Bank::findCustomer(int customerId) const {
    auto it = std::find_if(customers.begin(), customers.end(),
        [customerId](const auto& customer) {
            return customer->getId() == customerId;
        });
    return it != customers.end() ? it->get() : nullptr;
}

bool Bank::removeCustomer(int customerId) {
    auto it = std::find_if(customers.begin(), customers.end(),
        [customerId](const auto& customer) {
            return customer->getId() == customerId;
        });
    
    if (it != customers.end()) {
        customers.erase(it);
        return true;
    }
    return false;
}

Account* Bank::createSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    auto account = std::make_unique<SavingsAccount>(generateAccountNumber(), initialBalance, customer);
    Account* accountPtr = account.get();
    accounts.push_back(std::move(account));
    return accountPtr;
}

Account* Bank::createCurrentAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    auto account = std::make_unique<CurrentAccount>(generateAccountNumber(), initialBalance, customer);
    Account* accountPtr = account.get();
    accounts.push_back(std::move(account));
    return accountPtr;
}

Account* Bank::createAuditableSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    auto account = std::make_unique<AuditableSavingsAccount>(generateAccountNumber(), initialBalance, customer);
    Account* accountPtr = account.get();
    accounts.push_back(std::move(account));
    return accountPtr;
}

Account* Bank::findAccount(int accountNumber) const {
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [accountNumber](const auto& account) {
            return account->getAccountNumber() == accountNumber;
        });
    return it != accounts.end() ? it->get() : nullptr;
}

bool Bank::closeAccount(int accountNumber) {
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [accountNumber](const auto& account) {
            return account->getAccountNumber() == accountNumber;
        });
    
    if (it != accounts.end()) {
        accounts.erase(it);
        return true;
    }
    return false;
}

bool Bank::processDeposit(int accountNumber, double amount) {
    Account* account = findAccount(accountNumber);
    if (!account) {
        return false;
    }
    return account->deposit(amount);
}

bool Bank::processWithdrawal(int accountNumber, double amount) {
    Account* account = findAccount(accountNumber);
    if (!account) {
        return false;
    }
    return account->withdraw(amount);
}

bool Bank::processTransfer(int fromAccount, int toAccount, double amount) {
    Account* from = findAccount(fromAccount);
    Account* to = findAccount(toAccount);
    
    if (!from || !to) {
        return false;
    }
    
    auto transfer = std::make_unique<Transfer>(from, to, amount);
    return transfer->execute();
}

void Bank::applyMonthlyUpdates() {
    for (auto& account : accounts) {
        account->applyMonthlyUpdate();
    }
}

const std::string& Bank::getName() const {
    return name;
}

const std::vector<std::unique_ptr<Customer>>& Bank::getCustomers() const {
    return customers;
}

const std::vector<std::unique_ptr<Account>>& Bank::getAccounts() const {
    return accounts;
} 