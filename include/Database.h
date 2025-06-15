#ifndef DATABASE_H
#define DATABASE_H

#include "Customer.h"
#include "Account.h"
#include "Transaction.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

class Database {
private:
    static Database* instance;
    static std::mutex mutex;
    std::string dataDir;
    
    // In-memory storage
    std::unordered_map<int, std::unique_ptr<Customer>> customers;
    std::unordered_map<std::string, int> usernameToCustomerId;  // username -> customerId
    std::unordered_map<int, Account*> accounts;  // accountNumber -> Account*
    std::unordered_map<int, std::string> accountPasswords;  // accountNumber -> password

    // Private constructor for singleton
    Database(const std::string& dataDir);
    
    // File operations
    void createDataDirectory();
    std::string getCustomerFilePath() const;
    std::string getAccountFilePath() const;
    std::string getTransactionFilePath() const;
    std::string getAuthFilePath() const;

    // Save/Load operations
    void saveCustomer(const Customer* customer);
    void saveAccount(const Account* account);
    void saveTransaction(const Account* account, const ITransaction* transaction);
    void loadCustomers();
    void loadAccounts();
    void loadTransactions();
    void saveAuthData();
    void loadAuthData();

public:
    static Database* getInstance(const std::string& dataDir = "data");
    
    // Customer operations
    bool addCustomer(std::unique_ptr<Customer> customer, const std::string& username, const std::string& password);
    Customer* findCustomer(int customerId) const;
    bool removeCustomer(int customerId);
    
    // Account operations
    bool addAccount(std::unique_ptr<Account> account, const std::string& password);
    Account* findAccount(int accountNumber) const;
    bool removeAccount(int accountNumber);
    
    // Transaction operations
    bool addTransaction(int accountNumber, std::unique_ptr<ITransaction> transaction);
    std::vector<std::unique_ptr<ITransaction>> getTransactions(int accountNumber) const;
    
    // Authentication
    bool authenticate(const std::string& username, const std::string& password, int& customerId) const;
    bool changePassword(int customerId, const std::string& oldPassword, const std::string& newPassword);
    bool usernameExists(const std::string& username) const;
    
    // Data persistence
    void saveAll();
    void loadAll();
    
    // Static helper methods
    static bool verifyPassword(int accountNumber, const std::string& password);
    static Account* getAccount(int accountNumber);
    
    ~Database();
};

#endif // DATABASE_H 