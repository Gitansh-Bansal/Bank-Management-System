#pragma once

#include "Bank.h"
#include "Customer.h"
#include "Account.h"
#include "Transaction.h"
#include <memory>
#include <string>

class BankApp {
private:
    static BankApp* instance;
    std::unique_ptr<Bank> bank;
    Customer* currentCustomer;
    Account* currentAccount;
    std::string bankName;

    // Private constructor for singleton
    BankApp(const std::string& bankName);
    ~BankApp();  // Add destructor declaration

    // UI helper methods
    void displayMainMenu();
    void displayCustomerMenu();
    void displayAccountMenu();
    void listAccounts();
    
    // Customer operations
    void handleCustomerLogin();
    void handleCustomerRegistration();
    void handleCustomerLogout();
    
    // Account operations
    void handleAccountCreation();
    void handleAccountSelection();
    void handleBalanceInquiry();
    void handleDeposit();
    void handleWithdrawal();
    void handleTransfer();
    void handleAccountStatement();
    void handleAccountClosure();

public:
    static BankApp* getInstance(const std::string& bankName = "MyBank");
    void run();
    
    // Transaction handling
    void performTransaction(int accountNumber, const std::string& type);
}; 