#include "../include/Database.h"
#include "../include/SavingsAccount.h"
#include "../include/CurrentAccount.h"
#include "../include/AuditableSavingsAccount.h"
#include "../include/Transaction.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sys/stat.h>

// Initialize static members
Database* Database::instance = nullptr;
// std::mutex Database::mutex;  // Temporarily commented out for compilation

Database::Database(const std::string& dataDir) : dataDir(dataDir) {
    createDataDirectory();
    loadAll();
}

Database* Database::getInstance(const std::string& dataDir) {
    // std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new Database(dataDir);
    }
    return instance;
}

void Database::createDataDirectory() {
    try {
        // Create the data directory by creating a temporary file in it
        // This approach works on Windows and Unix-like systems
        std::string testFile = dataDir + "/.test";
        std::ofstream file(testFile);
        if (file.is_open()) {
            file.close();
            // Remove the test file
            std::remove(testFile.c_str());
        } else {
            // If we can't create a file, try to create the directory using system commands
            #ifdef _WIN32
                std::string command = "mkdir \"" + dataDir + "\" 2>nul";
                system(command.c_str());
            #else
                std::string command = "mkdir -p \"" + dataDir + "\" 2>/dev/null";
                system(command.c_str());
            #endif
            
            // Test again
            std::ofstream testFile2(testFile);
            if (!testFile2.is_open()) {
                throw std::runtime_error("Failed to create data directory: " + dataDir);
            }
            testFile2.close();
            std::remove(testFile.c_str());
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create data directory: " + std::string(e.what()));
    }
}

std::string Database::getCustomerFilePath() const {
    return dataDir + "/customers.txt";
}

std::string Database::getAccountFilePath() const {
    return dataDir + "/accounts.txt";
}

std::string Database::getTransactionFilePath() const {
    return dataDir + "/transactions.txt";
}

std::string Database::getAuthFilePath() const {
    return dataDir + "/auth.txt";
}

std::string Database::getCounterFilePath() const {
    return dataDir + "/counters.txt";
}

bool Database::addCustomer(std::unique_ptr<Customer> customer, const std::string& username, const std::string& password) {
    if (usernameExists(username)) {
        return false;
    }
    
    try {
        // If customer is nullptr, we're just adding authentication data
        if (customer) {
            int customerId = customer->getId();
            usernameToCustomerId[username] = customerId;
            usernamePasswords[username] = password;
            customers[customerId] = std::move(customer);
        }
        
        saveAll(); // Save all data after adding customer
        return true;
    } catch (const std::exception& e) {
        if (customer) {
            customers.erase(customer->getId());
            usernameToCustomerId.erase(username);
            usernamePasswords.erase(username);
        }
        throw std::runtime_error("Failed to save customer data: " + std::string(e.what()));
    }
}

Customer* Database::findCustomer(int customerId) const {
    auto it = customers.find(customerId);
    return it != customers.end() ? it->second.get() : nullptr;
}

bool Database::removeCustomer(int customerId) {
    auto it = customers.find(customerId);
    if (it == customers.end()) {
    return false;
}

    try {
        // Remove all associated accounts
        for (const auto& account : it->second->getAccounts()) {
            removeAccount(account->getAccountNumber());
        }
        
        // Remove from maps
        customers.erase(it);

        std::string username;
        
        // Remove from auth data
        for (auto it = usernameToCustomerId.begin(); it != usernameToCustomerId.end();) {
            if (it->second == customerId) {
                username = it->first;
                it = usernameToCustomerId.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = usernamePasswords.begin(); it != usernamePasswords.end();) {
            if (it->first == username) {
                it = usernamePasswords.erase(it);
            } else {
                ++it;
            }
        }
        
        // Save changes
        saveAll();
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to remove customer: " + std::string(e.what()));
    }
}

bool Database::addAccount(std::unique_ptr<Account> account, const std::string& password) {
    int accountNumber = account->getAccountNumber();
    
    try {
        // Store the pointer BEFORE moving the account
        Account* accountPtr = account.get();
        
        accounts[accountNumber] = accountPtr;
        
        accountPasswords[accountNumber] = password;
        
        // Now move the account to the customer
        Customer* owner = account->getOwner();
        
        owner->addAccount(std::move(account));
        
        saveAll(); // Save all data after adding account
        
        return true;
    } catch (const std::exception& e) {
        accounts.erase(accountNumber);
        accountPasswords.erase(accountNumber);
        throw std::runtime_error("Failed to add account: " + std::string(e.what()));
    }
}

Account* Database::findAccount(int accountNumber) const {
    auto it = accounts.find(accountNumber);
    if (it != accounts.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

bool Database::removeAccount(int accountNumber) {
    auto it = accounts.find(accountNumber);
    auto it2 = accountPasswords.find(accountNumber);
    if (it == accounts.end() || it2 == accountPasswords.end()) {
        return false;
    }

    try {
        // 1. Get the account and its owner before removing
        Account* account = it->second;
        Customer* owner = account->getOwner();
        
        // 2. Remove from customer's account list
        if (owner) {
            owner->removeAccount(accountNumber);
        }

        // 3. Remove from database maps
        accounts.erase(it);
        accountPasswords.erase(it2);

        // 4. Clean up transaction history
        std::ifstream transFile(getTransactionFilePath());
        std::ofstream tempFile(getTransactionFilePath() + ".tmp");
        std::string line;
        
        while (std::getline(transFile, line)) {
            std::stringstream ss(line);
            int accNum;
            ss >> accNum;
            if (accNum != accountNumber) {
                tempFile << line << std::endl;
            }
        }
        
        transFile.close();
        tempFile.close();
        std::remove(getTransactionFilePath().c_str());
        std::rename((getTransactionFilePath() + ".tmp").c_str(), getTransactionFilePath().c_str());

        // 5. Clean up account file
        // std::ifstream accFile(getAccountFilePath());
        // std::ofstream accTempFile(getAccountFilePath() + ".tmp");
        
        // while (std::getline(accFile, line)) {
        //     std::stringstream ss(line);
        //     int accNum;
        //     ss >> accNum;
        //     if (accNum != accountNumber) {
        //         accTempFile << line << std::endl;
        //     }
        // }
        
        // accFile.close();
        // accTempFile.close();
        // std::experimental::filesystem::remove(getAccountFilePath());
        // std::experimental::filesystem::rename(getAccountFilePath() + ".tmp", getAccountFilePath());

        // 6. Clean up auth file
        // std::ifstream authFile(getAuthFilePath());
        // std::ofstream authTempFile(getAuthFilePath() + ".tmp");
        
        // while (std::getline(authFile, line)) {
        //     std::stringstream ss(line);
        //     std::string username, password;
        //     int accNum;
        //     ss >> username >> password >> accNum;
        //     if (accNum != accountNumber) {
        //         authTempFile << line << std::endl;
        //     }
        // }
        
        // authFile.close();
        // authTempFile.close();
        // std::experimental::filesystem::remove(getAuthFilePath());
        // std::experimental::filesystem::rename(getAuthFilePath() + ".tmp", getAuthFilePath());

        // 7. Save all remaining changes
        //saveAll();
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to remove account: " + std::string(e.what()));
    }
}

bool Database::addTransaction(int accountNumber, std::unique_ptr<ITransaction> transaction) {
    Account* account = findAccount(accountNumber);
    if (!account) {
        return false;
    }
    
    try {
        saveTransaction(account, transaction.get());      // Save before moving
        // account->addTransaction(std::move(transaction));  // Move after saving
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to add transaction: " + std::string(e.what()));
    }
}

void Database::getTransactions(int accountNumber, std::ostream& out) const {
    std::ifstream file(getTransactionFilePath());
    if (!file.is_open()) {
        out << "No transaction history found." << std::endl;
        return;
    }

    std::string line;
    bool foundTransactions = false;
    double runningBalance = 0.0; // Track running balance
    
    // Header
    out << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    out << "│                           Transaction History for Account #" << accountNumber << "                          │" << std::endl;
    out << "├─────────────────────┬─────────────────┬────────────────┬────────────────┬─────────────────┤" << std::endl;
    out << "│ Timestamp           │ Type            │ Amount         │ Balance        │ Related Account │" << std::endl;
    out << "├─────────────────────┼─────────────────┼────────────────┼────────────────┼─────────────────┤" << std::endl;
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string accNumStr, timestamp, type, amountStr, relatedAccountStr;
        
        // Parse using colons as separators
        std::getline(ss, accNumStr, ':');
        std::getline(ss, timestamp, ':');
        std::getline(ss, type, ':');
        std::getline(ss, amountStr, ':');
        std::getline(ss, relatedAccountStr); // This might be empty for deposits/withdrawals
        
        int accNum = std::stoi(accNumStr);
        if (accNum != accountNumber) continue;
        
        if (!timestamp.empty() && !type.empty() && !amountStr.empty()) {
            double amountValue = std::stod(amountStr);
            foundTransactions = true;
            
            // Update running balance based on transaction type
            if (type == "0" || type == "2") { // Deposit or Transfer In
                runningBalance += amountValue;
            } else if (type == "1") { // Withdrawal or Transfer Out
                runningBalance -= amountValue; // amountValue is already negative for withdrawals/transfer out
            }
            
            // Format the output
            std::string vertical = "│ ";
            int timestampWidth = 20;
            int typeWidth = 16;
            int amountWidth = 14;
            int balanceWidth = 14;
            int relatedWidth = 16;
            
            // Determine transaction type display
            std::string typeDisplay = type;
            if (type == "2") { // Transfer transaction
                if (amountValue > 0) {
                    typeDisplay = "Transfer In";
                } else {
                    typeDisplay = "Transfer Out";
                }
            } else if (type == "0") {
                typeDisplay = "Deposit";
            } else if (type == "1") {
                typeDisplay = "Withdrawal";
            }

            // Add related account information for transfers
            if (type == "2" && !relatedAccountStr.empty()) {
                if (amountValue > 0) {
                    relatedAccountStr = "From " + relatedAccountStr;
                } else {
                    relatedAccountStr = "To " + relatedAccountStr;
                }
            }
            else relatedAccountStr = "-";
            
            out << vertical << std::left << std::setw(timestampWidth) << timestamp
                << vertical << std::setw(typeWidth) << typeDisplay
                << vertical << "$" << std::setw(amountWidth) << std::fixed << std::setprecision(2) << (amountValue < 0 ? -amountValue : amountValue)
                << vertical << "$" << std::setw(balanceWidth) << std::fixed << std::setprecision(2) << runningBalance
                << vertical << std::setw(relatedWidth) << relatedAccountStr
                << vertical;
            out << std::endl;
        }
    }
    
    if (!foundTransactions) {
    out << "│                             No transactions found for this account                            │" << std::endl;
    }
    out << "├─────────────────────┴─────────────────┴────────────────┴────────────────┴─────────────────┤" << std::endl;
    out << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
}

bool Database::authenticate(const std::string& username, const std::string& password, int& customerId) const {
    auto it = usernameToCustomerId.find(username);
    if (it == usernameToCustomerId.end()) {
        return false;
    }
    
    customerId = it->second;

    auto it2 = usernamePasswords.find(username);
    if (it2 == usernamePasswords.end()){
        return false;
    }

    return password == it2->second;
}

bool Database::changePassword(int customerId, const std::string& oldPassword, const std::string& newPassword) {
    try {
        // First, find the username for this customer ID
        std::string username;
        for (const auto& pair : usernameToCustomerId) {
            if (pair.second == customerId) {
                username = pair.first;
                break;
            }
        }
        
        if (username.empty()) {
            std::cerr << "Username not found for customer ID: " << customerId << std::endl;
            return false;
        }
        
        // Verify the old password matches what's in memory
        auto it = usernamePasswords.find(username);
        if (it == usernamePasswords.end() || it->second != oldPassword) {
            std::cerr << "Old password verification failed for username: " << username << std::endl;
            return false;
        }
        
        // Update the file
        std::ifstream file(getAuthFilePath());
        if (!file.is_open()) {
            std::cerr << "Failed to open auth file for reading" << std::endl;
            return false;
        }
        
        std::vector<std::string> lines;
        std::string line;
        bool found = false;
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string type, fileUsername, storedPassword, storedId;
            std::getline(ss, type, ':');
            
            if (type == "CUSTOMER") {
                std::getline(ss, fileUsername, ':');
                std::getline(ss, storedPassword, ':');
                std::getline(ss, storedId);
                
                if (fileUsername == username) {
                    if (storedPassword != oldPassword) {
                        std::cerr << "Password mismatch in file for username: " << username << std::endl;
                        return false;
                    }
                    lines.push_back("CUSTOMER:" + username + ":" + newPassword + ":" + storedId);
                    found = true;
                } else {
                    lines.push_back(line);
                }
            } else {
                // Keep non-CUSTOMER lines as they are (ACCOUNT lines)
                lines.push_back(line);
            }
        }

        if (!found) {
            std::cerr << "Customer record not found in auth file for username: " << username << std::endl;
            return false;
        }

        // Write the updated data back to file
        std::ofstream outFile(getAuthFilePath());
        if (!outFile.is_open()) {
            std::cerr << "Failed to open auth file for writing" << std::endl;
            return false;
        }
        
        for (const auto& l : lines) {
            outFile << l << std::endl;
        }
        
        // Update the in-memory data structure
        usernamePasswords[username] = newPassword;
        
        std::cerr << "Password successfully changed for username: " << username << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in changePassword: " << e.what() << std::endl;
        return false;
    }
}

bool Database::usernameExists(const std::string& username) const {
    return usernameToCustomerId.find(username) != usernameToCustomerId.end();
}

void Database::saveAll() {
    try {
        saveCustomer(nullptr); // Save all customers
        saveAccount(nullptr);  // Save all accounts
        saveAuthData();
        saveCounters();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save all data: " + std::string(e.what()));
    }
}

void Database::loadAll() {
    try {
        loadCustomers();
        loadAccounts();
        // loadTransactions();
        loadAuthData();
        loadCounters();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load data: " + std::string(e.what()));
    }
}

void Database::saveCustomer(const Customer* customer) {
    (void)customer; // Suppress unused parameter warning
    try {
        std::ofstream file(getCustomerFilePath());
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open customer file for writing");
        }
        
        for (const auto& pair : customers) {
            int id = pair.first;
            const Customer* cust = pair.second.get();
            file << id << ":" << cust->getName() << ":" << cust->getPhone() << std::endl;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save customer data: " + std::string(e.what()));
    }
}

void Database::saveAccount(const Account* account) {
    (void)account; // Suppress unused parameter warning
    try {
        std::ofstream file(getAccountFilePath());
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open account file for writing");
        }
        
        for (const auto& pair : accounts) {
            int number = pair.first;
            Account* acc = pair.second;
            file << number << ":" << acc->getOwner()->getId() << ":" 
                 << acc->getBalance() << ":" << static_cast<int>(acc->getType()) << std::endl;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save account data: " + std::string(e.what()));
    }
}

void Database::saveTransaction(const Account* account, const ITransaction* transaction) {
    try {
        // Ensure the data directory exists
        createDataDirectory();
        
        // Open file in append mode
        std::ofstream file(getTransactionFilePath(), std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open transaction file for writing");
        }

        if (static_cast<int>(transaction->getType()) == 2) {
            saveTransferTransaction(transaction);
            return;
        }
        
        file << account->getAccountNumber() << ":" 
             << transaction->getTimestamp() << ":"
             << static_cast<int>(transaction->getType()) << ":"
             << transaction->getAmount() << std::endl;
             
        if (!file.good()) {
            throw std::runtime_error("Failed to write transaction data");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save transaction: " + std::string(e.what()));
    }
}

void Database::saveTransferTransaction(const ITransaction* transaction) {
    std::ofstream file(getTransactionFilePath(), std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open transaction file for writing");
    }
    const Transfer* transfer = dynamic_cast<const Transfer*>(transaction);

    //save in format accountNumber:timestamp:type:amount:toAccountNumber
    file << transfer->getFromAccount() << ":"
         << transaction->getTimestamp() << ":"
         << static_cast<int>(transaction->getType()) << ":"
         << (-1 * transaction->getAmount()) << ":"
         << transfer->getToAccount() << std::endl;

    file << transfer->getToAccount() << ":"
         << transaction->getTimestamp() << ":"
         << static_cast<int>(transaction->getType()) << ":"
         << transaction->getAmount() << ":"
         << transfer->getFromAccount() << std::endl;
}

void Database::loadCustomers() {
    std::ifstream file(getCustomerFilePath());
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, name, phone;
        std::getline(ss, id, ':');
        std::getline(ss, name, ':');
        std::getline(ss, phone);
        
        auto customer = std::make_unique<Customer>(std::stoi(id), name, phone);
        customers[customer->getId()] = std::move(customer);
    }
}

void Database::loadAccounts() {
    std::ifstream file(getAccountFilePath());
    if (!file.is_open()) {
        std::cerr << "Failed to open account file: " << getAccountFilePath() << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string accountNumber, ownerId, balance, type;
        std::getline(ss, accountNumber, ':');
        std::getline(ss, ownerId, ':');
        std::getline(ss, balance, ':');
        std::getline(ss, type);
        
        Customer* owner = findCustomer(std::stoi(ownerId));
        if (!owner) {
            continue;
        }
        
        std::unique_ptr<Account> account;
        switch (std::stoi(type)) {
            case static_cast<int>(AccountType::SAVINGS):
                account = std::make_unique<SavingsAccount>(
                    std::stoi(accountNumber),
                    std::stod(balance),
                    owner,
                    SavingsAccount::getDefaultInterestRate(),
                    AccountType::SAVINGS
                );
                break;
            case static_cast<int>(AccountType::CURRENT):
                account = std::make_unique<CurrentAccount>(
                    std::stoi(accountNumber),
                    std::stod(balance),
                    owner
                );
                break;
            case static_cast<int>(AccountType::AUDITABLE_SAVINGS):
                account = std::make_unique<AuditableSavingsAccount>(
                    std::stoi(accountNumber),
                    std::stod(balance),
                    owner
                );
                break;
            default:
                continue;
        }
        
        // Store the pointer BEFORE moving the account
        Account* accountPtr = account.get();
        int accNumber = accountPtr->getAccountNumber();
        
        // Add to customer's accounts (this moves the account)
        owner->addAccount(std::move(account));
        
        // Add to accounts map
        accounts[accNumber] = accountPtr;
    }
}

// void Database::loadTransactions() {
//     std::ifstream file(getTransactionFilePath());
//     if (!file.is_open()) {
//         return;
//     }

//     std::string line;
//     while (std::getline(file, line)) {
//         std::stringstream ss(line);
//         std::string accountNumber, timestamp, type, amount;
//         std::getline(ss, accountNumber, ':');
//         std::getline(ss, timestamp, ':');
//         std::getline(ss, type, ':');
//         std::getline(ss, amount);
        
//         Account* account = findAccount(std::stoi(accountNumber));
//         if (!account) {
//             continue;
//         }
        
//         std::unique_ptr<ITransaction> transaction;
//         int typeInt = std::stoi(type);
        
//         if (typeInt == static_cast<int>(TransactionType::DEPOSIT)) {
//             transaction = std::make_unique<Deposit>(account, std::stod(amount));
//         }
//         else if (typeInt == static_cast<int>(TransactionType::WITHDRAWAL)) {
//             transaction = std::make_unique<Withdrawal>(account, std::stod(amount));
//         }
//         else if (typeInt == static_cast<int>(TransactionType::TRANSFER)) {
//             std::string targetAccountStr = description.substr(description.find("to account ") + 11);
//             Account* toAccount = findAccount(std::stoi(targetAccountStr));
//             if (toAccount) {
//                 transaction = std::make_unique<Transfer>(account, toAccount, std::stod(amount));
//             }
//         }
        
//         if (transaction) {
//             account->addTransaction(std::move(transaction));
//         }
//     }
// }

void Database::saveAuthData() {
    std::ofstream file(getAuthFilePath());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open auth file for writing");
    }
    
    // Save customer auth data in form of customer :username : password  : customerId
    for (const auto& pair : usernameToCustomerId) {
        const std::string& username = pair.first;
        int customerId = pair.second;
        file << "CUSTOMER:" << username << ":" << usernamePasswords[username] << ":" << customerId << std::endl;
    }
    
    // Save account authentication data
    for (const auto& pair : accountPasswords) {
        int accountNumber = pair.first;
        const std::string& password = pair.second;
        file << "ACCOUNT:" << accountNumber << ":" << password << std::endl;
    }
}

void Database::loadAuthData() {
    std::ifstream file(getAuthFilePath());
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        //customer :username : password  : customerId
        std::stringstream ss(line);
        std::string type, username, password, customerId;
        std::getline(ss, type, ':');
        std::getline(ss, username, ':');
        
        if (type == "CUSTOMER") {
            std::getline(ss, password, ':');
            std::getline(ss, customerId);
            if (!username.empty() && !customerId.empty()) {
                usernameToCustomerId[username] = std::stoi(customerId);
                usernamePasswords[username] = password;  // Also store the password
            }
        } else if (type == "ACCOUNT") {
            std::getline(ss, password);
            accountPasswords[std::stoi(username)] = password;
        }
    }
}

bool Database::verifyPassword(int accountNumber, const std::string& password) {
    Database* db = getInstance();
    auto it = db->accountPasswords.find(accountNumber);
    if (it == db->accountPasswords.end()) {
    return false;
    }
    return it->second == password;
}

Account* Database::getAccount(int accountNumber) {
    Database* db = getInstance();
    return db->findAccount(accountNumber);
}

void Database::saveCounters() const {
    try {
        std::ofstream file(getCounterFilePath());
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open counter file for writing");
        }
        file << nextCustomerId << ":" << nextAccountNumber << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save counters: " + std::string(e.what()));
    }
}

void Database::loadCounters() {
    try {
        std::ifstream file(getCounterFilePath());
        if (!file.is_open()) {
            // If file doesn't exist, initialize with default values
            nextCustomerId = 1000;
            nextAccountNumber = 10000;
            return;
        }

        std::string line;
        if (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string customerIdStr, accountNumberStr;
            if (std::getline(ss, customerIdStr, ':') && std::getline(ss, accountNumberStr)) {
                nextCustomerId = std::stoi(customerIdStr);
                nextAccountNumber = std::stoi(accountNumberStr);
            } else {
                throw std::runtime_error("Invalid counter file format");
            }
        } else {
            // If file is empty, initialize with default values
            nextCustomerId = 1000;
            nextAccountNumber = 10000;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load counters: " + std::string(e.what()));
    }
}

int Database::getNextCustomerId() {
    Database* db = getInstance();
    return db->nextCustomerId;
}

int Database::getNextAccountNumber() {
    Database* db = getInstance();
    return db->nextAccountNumber;
}

void Database::incrementCustomerId() {
    Database* db = getInstance();
    db->nextCustomerId++;
    db->saveCounters();
}

void Database::incrementAccountNumber() {
    Database* db = getInstance();
    db->nextAccountNumber++;
    db->saveCounters();
}


std::unique_ptr<Account> Database::createSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = getNextAccountNumber();
    incrementAccountNumber();
    return std::make_unique<SavingsAccount>(accountNumber, initialBalance, customer, SavingsAccount::getDefaultInterestRate(), AccountType::SAVINGS);
}

std::unique_ptr<Account> Database::createCurrentAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = getNextAccountNumber();
    incrementAccountNumber();
    return std::make_unique<CurrentAccount>(accountNumber, initialBalance, customer);
}

std::unique_ptr<Account> Database::createAuditableSavingsAccount(int customerId, double initialBalance) {
    Customer* customer = findCustomer(customerId);
    if (!customer) {
        throw std::runtime_error("Customer not found");
    }
    
    int accountNumber = getNextAccountNumber();
    incrementAccountNumber();
    return std::make_unique<AuditableSavingsAccount>(accountNumber, initialBalance, customer);
}

int Database::getCustomerIdByUsername(const std::string& username) const {
    auto it = usernameToCustomerId.find(username);
    return it != usernameToCustomerId.end() ? it->second : -1;
}

Database::~Database() {
    try {
        saveAll();
    } catch (const std::exception& e) {
        std::cerr << "Error saving data: " << e.what() << std::endl;
    }
} 