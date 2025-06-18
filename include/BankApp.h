#pragma once

#include "Customer.h"
#include "Account.h"
#include "Transaction.h"
#include "Database.h"
#include <memory>
#include <string>

class BankApp {
private:
    static BankApp* instance;
    std::string bankName;
    Customer* currentCustomer;
    Account* currentAccount;

    // Private constructor for singleton
    BankApp(const std::string& bankName);
    
    // UI helper methods
    void displayMainMenu();
    void displayCustomerMenu();
    // void displayAccountMenu();
    void listAccounts();
    
    // Customer operations
    void handleCustomerLogin();
    void handleCustomerRegistration();
    // void handleCustomerLogout();
    
    // Account operations
    void handleAccountCreation();
    void handleAccountSelection();
    // void handleBalanceInquiry();
    // void handleDeposit();
    // void handleWithdrawal();
    // void handleTransfer();
    // void handleAccountStatement();
    // void handleAccountClosure();

public:
    static BankApp* getInstance(const std::string& bankName = "MyBank");
    void run();
    ~BankApp();  // Add destructor declaration
    
    // Input validation methods (moved from private)
    bool isValidName(const std::string& name);
    bool isValidPhone(const std::string& phone);
    bool isValidUsername(const std::string& username);
    bool isValidPassword(const std::string& password);
    
    // API methods for command-line integration
    bool registerCustomer(const std::string& name, const std::string& phone, 
                         const std::string& username, const std::string& password);
    bool authenticateCustomer(const std::string& username, const std::string& password, int& customerId);
    int createAccount(const std::string& username, const std::string& password, const std::string& accountType, double initialBalance);
    bool deposit(int accountNumber, double amount, const std::string& password);
    bool withdraw(int accountNumber, double amount, const std::string& password);
    bool transfer(int fromAccount, int toAccount, double amount, const std::string& password);
    bool closeAccount(int accountNumber, const std::string& password);
    std::string getAccounts(const std::string& username);
    std::string getAccountDetails(int accountNumber);
    std::string getTransactions(int accountNumber);
    std::string getUserDetails(const std::string& username);
    bool updateProfile(const std::string& username, const std::string& name, const std::string& phone);
    bool changePassword(const std::string& username, const std::string& currentPassword, const std::string& newPassword);
    
    // Transaction handling
    // void performTransaction(int accountNumber, const std::string& type);
}; 