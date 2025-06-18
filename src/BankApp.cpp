#include "../include/BankApp.h"
#include "../include/Database.h"
#include "../include/Transaction.h"
#include "../include/SavingsAccount.h"
#include "../include/CurrentAccount.h"
#include "../include/AuditableSavingsAccount.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <memory>
#include <sstream>
#include <fstream>

BankApp* BankApp::instance = nullptr;

BankApp::BankApp(const std::string& bankName)
    : bankName(bankName), currentCustomer(nullptr), currentAccount(nullptr) {
    // Initialize database with correct data directory path
    // Use "../data" to point to the root data directory when running from api/bin/
    Database::getInstance("../data");
}

BankApp* BankApp::getInstance(const std::string& bankName) {
    if (instance == nullptr) {
        instance = new BankApp(bankName);
    }
    return instance;
}

BankApp::~BankApp() {
    delete Database::getInstance();
}

void BankApp::run() {
    while (true) {
        displayMainMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                handleCustomerLogin();
                break;
            case 2:
                handleCustomerRegistration();
                break;
            case 3:
                std::cout << "Thank you for using " << bankName << "!" << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void BankApp::displayMainMenu() {
    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│   Welcome to " << std::left << std::setw(12) << bankName << "   │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "├─────────────────────────────┤" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│  1. Login                   │" << std::endl;
    std::cout << "│  2. Register                │" << std::endl;
    std::cout << "│  3. Exit                    │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘\n" << std::endl;
    std::cout << "Enter Your Choice: ";
}

void BankApp::handleCustomerLogin() {
    std::string username, password;
    std::cout << "\nEnter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    int customerId;
    if (Database::getInstance()->authenticate(username, password, customerId)) {
        currentCustomer = Database::getInstance()->findCustomer(customerId);
        if (currentCustomer) {
            std::cout << "Login successful!" << std::endl;
            displayCustomerMenu();
        } else {
            std::cout << "Error: Customer not found." << std::endl;
        }
    } else {
        std::cout << "Invalid username or password." << std::endl;
    }
}

bool BankApp::isValidName(const std::string& name) {
    if (name.empty()) return false;
    
    // Check if name contains only letters and spaces
    for (char c : name) {
        if (!std::isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

bool BankApp::isValidPhone(const std::string& phone) {
    if (phone.length() != 10) return false;
    
    // Check if phone contains only digits
    for (char c : phone) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool BankApp::isValidUsername(const std::string& username) {
    if (username.empty() || username.length() < 4) {
        std::cout << "Invalid username! Username must be at least 4 characters." << std::endl;
        return false;
    }
    
    // Username can contain letters, numbers, and underscores
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            std::cout << "Invalid username! Username can only contain letters, numbers, and underscores." << std::endl;
            return false;
        }
    }

    // Check if username already exists
    if (Database::getInstance()->usernameExists(username)) {
        std::cout << "Username already exists. Please choose another one." << std::endl;
        return false;
    }
    
    return true;
}

bool BankApp::isValidPassword(const std::string& password) {
    if (password.empty() || password.length() < 6) return false;
    
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
    }
    
    return hasUpper && hasLower && hasDigit;
}

void BankApp::handleCustomerRegistration() {
    std::string name, phone, username, password;
    
    // Get and validate name
    do {
        std::cout << "\nEnter your name (letters and spaces only): ";
        std::getline(std::cin, name);
        if (!isValidName(name)) {
            std::cout << "Invalid name! Name should only contain letters and spaces." << std::endl;
        }
    } while (!isValidName(name));
    
    // Get and validate phone
    do {
        std::cout << "Enter your phone number (10 digits): ";
        std::getline(std::cin, phone);
        if (!isValidPhone(phone)) {
            std::cout << "Invalid phone number! Please enter exactly 10 digits." << std::endl;
        }
    } while (!isValidPhone(phone));
    
    // Get and validate username
    do {
        std::cout << "Choose a username (at least 4 characters, letters, numbers, and underscores only): ";
        std::getline(std::cin, username);
    } while (!isValidUsername(username));
    
    // Get and validate password
    do {
        std::cout << "Choose a password (at least 6 characters, must include uppercase, lowercase, and numbers): ";
        std::getline(std::cin, password);
        if (!isValidPassword(password)) {
            std::cout << "Invalid password! Password must be at least 6 characters and include uppercase, lowercase, and numbers." << std::endl;
        }
    } while (!isValidPassword(password));

    int customerId = Database::getNextCustomerId();
    Database::incrementCustomerId();
    auto customerPtr = std::make_unique<Customer>(customerId, name, phone);
    if (Database::getInstance()->addCustomer(std::move(customerPtr), username, password)) {
        currentCustomer = Database::getInstance()->findCustomer(customerId);
        if (currentCustomer) {
            std::cout << "Registration successful! Your customer ID is: " << currentCustomer->getId() << std::endl;
            displayCustomerMenu();
        } else {
            std::cout << "Error: Customer not found after registration." << std::endl;
        }
    } else {
        std::cout << "Registration failed." << std::endl;
    }
}

void BankApp::displayCustomerMenu() {
    while (currentCustomer) {
        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│        Customer Menu        │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "├─────────────────────────────┤" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│  1. Create Account          │" << std::endl;
        std::cout << "│  2. Select Account          │" << std::endl;
        std::cout << "│  3. List Accounts           │" << std::endl;
        std::cout << "│  4. Logout                  │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                handleAccountCreation();
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                break;
            case 2:
                handleAccountSelection();
                break;
            case 3:
                listAccounts();
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                break;
            case 4:
                currentCustomer = nullptr;
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
        }
    }
}

void BankApp::handleAccountCreation() {
    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│      Select Account Type    │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "├─────────────────────────────┤" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│  1. Savings Account         │" << std::endl;
    std::cout << "│  2. Current Account         │" << std::endl;
    std::cout << "│  3. Auditable Savings       │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter initial balance: $";
    double initialBalance;
    std::cin >> initialBalance;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter account password: ";
    std::string password;
    std::getline(std::cin, password);

    try {
        std::unique_ptr<Account> account;
        switch (choice) {
            case 1:
                account = Database::getInstance()->createSavingsAccount(currentCustomer->getId(), 0);
                break;
            case 2:
                account = Database::getInstance()->createCurrentAccount(currentCustomer->getId(), 0);
                break;
            case 3:
                account = Database::getInstance()->createAuditableSavingsAccount(currentCustomer->getId(), 0);
                break;
            default:
                std::cout << "Invalid account type." << std::endl;
                return;
        }

        if (account) {
            int accnum = account->getAccountNumber();

            auto deposit = std::make_unique<Deposit>(account.get(), initialBalance);
            Database::getInstance()->addAccount(std::move(account), password);
            if (deposit->execute()) {
                if (Database::getInstance()->addTransaction(accnum, std::move(deposit))) {
                    std::cout << "Initial deposit of $" << initialBalance << " successful." << std::endl;
                } else {
                    std::cout << "Failed to record initial deposit transaction." << std::endl;
                }
            } else {
                std::cout << "Initial deposit failed." << std::endl;
            }   

            std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
            std::cout << "│         Account Creation          │" << std::endl;
            std::cout << "├───────────────────────────────────┤" << std::endl;
            std::cout << "│ Account Created successfully!     │" << std::endl;
            std::cout << "│ Account number: " << std::setw(12) << accnum << "      │" << std::endl;
            std::cout << "│ Account Type: " << std::setw(18) << Database::getInstance()->getAccount(accnum)->getTypeString() << "  │" << std::endl;
            std::cout << "│ Initial Balance: $" << std::fixed << std::setprecision(2) << std::setw(12) 
                      << Database::getInstance()->getAccount(accnum)->getBalance() << "    │" << std::endl;
            std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Account creation failed: " << e.what() << std::endl;
    }
}

void BankApp::handleAccountSelection() {
    if (!currentCustomer) {
        std::cout << "Please login first.\n";
        return;
    }

    listAccounts();
    
    // Check if customer has any accounts
    if (currentCustomer->getAccounts().empty()) {
        return;
    }

    std::cout << "Enter account number: ";
    int accountNumber;
    std::cin >> accountNumber;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    currentAccount = currentCustomer->findAccount(accountNumber);
    if (currentAccount && currentAccount->getOwner() == currentCustomer) {
        transactionFun(accountNumber);
    } else {
        std::cout << "Invalid account number." << std::endl;
    }
}

void BankApp::listAccounts() {
    if (!currentCustomer) {
        std::cout << "Please login first.\n";
        return;
    }

    const auto& accounts = currentCustomer->getAccounts();
    
    if (accounts.empty()) {
        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
        std::cout << "│         Account Information       │" << std::endl;
        std::cout << "├───────────────────────────────────┤" << std::endl;
        std::cout << "│ No accounts found.                │" << std::endl;
        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
        return;
    }

    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│         Account Information       │" << std::endl;
    std::cout << "├───────────────────────────────────┤" << std::endl;
    for (const auto& account : accounts) {
        std::cout << "│ Account Number: " << std::setw(12) << account->getAccountNumber() << "      │" << std::endl;
        std::cout << "│ Type: " << std::setw(25) << account->getTypeString() << "   │" << std::endl;
        std::cout << "│ Balance: $" << std::fixed << std::setprecision(2) << std::setw(12) << account->getBalance() << "            │" << std::endl;
        std::cout << "├───────────────────────────────────┤" << std::endl;
    }
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘\n" << std::endl;
}

// API methods for command-line integration
bool BankApp::registerCustomer(const std::string& name, const std::string& phone, 
                              const std::string& username, const std::string& password) {
    try {
        int customerId = Database::getNextCustomerId();
        Database::incrementCustomerId();
        auto customerPtr = std::make_unique<Customer>(customerId, name, phone);
        return Database::getInstance()->addCustomer(std::move(customerPtr), username, password);
    } catch (const std::exception& e) {
        return false;
    }
}

bool BankApp::authenticateCustomer(const std::string& username, const std::string& password, int& customerId) {
    return Database::getInstance()->authenticate(username, password, customerId);
}

int BankApp::createAccount(const std::string& username, const std::string& password, const std::string& accountType, double initialBalance) {
    try {
        // Find customer by username (no password authentication needed)
        int customerId = Database::getInstance()->getCustomerIdByUsername(username);
        if (customerId == -1) {
            return -1;
        }
        
        Customer* customer = Database::getInstance()->findCustomer(customerId);
        if (!customer) {
            return -1;
        }
        
        // Create account based on type with 0 initial balance
        int accountNumber = Database::getNextAccountNumber();
        Database::incrementAccountNumber();
        
        std::unique_ptr<Account> account;
        
        if (accountType == "savings") {
            account = std::make_unique<SavingsAccount>(accountNumber, 0, customer);
        } else if (accountType == "current") {
            account = std::make_unique<CurrentAccount>(accountNumber, 0, customer);
        } else if (accountType == "auditable") {
            account = std::make_unique<AuditableSavingsAccount>(accountNumber, 0, customer);
        } else {
            return -1;
        }
        
        // First add the account to the database with 0 balance
        if (Database::getInstance()->addAccount(std::move(account), password)) {
            // Now get the account from the database and create the deposit transaction if there's an initial balance
            if (initialBalance > 0) {
                Account* dbAccount = Database::getInstance()->getAccount(accountNumber);
                if (dbAccount) {
                    auto deposit = std::make_unique<Deposit>(dbAccount, initialBalance);
                    if (deposit->execute()) {
                        Database::getInstance()->addTransaction(accountNumber, std::move(deposit));
                        Database::getInstance()->saveAll();
                    }
                }
            }
            return accountNumber;
        }
        return -1;
    } catch (const std::exception& e) {
        return -1;
    }
}

bool BankApp::deposit(int accountNumber, double amount, const std::string& password) {
    try {
        Account* account = Database::getInstance()->getAccount(accountNumber);
        if (!account) {
            std::cerr << "Account not found" << std::endl;
            return false;
        }
        
        // Verify account password
        if (!Database::getInstance()->verifyPassword(accountNumber, password)) {
            std::cerr << "Incorrect password" << std::endl;
            return false;
        }
        
        if (account->deposit(amount)) {
            // Create and save transaction record
            auto transaction = std::make_unique<Deposit>(account, amount);
            Database::getInstance()->addTransaction(accountNumber, std::move(transaction));
            
            // Save changes to database
            Database::getInstance()->saveAll();
            return true;
        } else {
            std::cerr << "Deposit operation failed" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool BankApp::withdraw(int accountNumber, double amount, const std::string& password) {
    try {
        Account* account = Database::getInstance()->getAccount(accountNumber);
        if (!account) {
            std::cerr << "Account not found" << std::endl;
            return false;
        }
        
        // Verify account password
        if (!Database::getInstance()->verifyPassword(accountNumber, password)) {
            std::cerr << "Incorrect password" << std::endl;
            return false;
        }
        
        if (account->withdraw(amount)) {
            // Create and save transaction record
            auto transaction = std::make_unique<Withdrawal>(account, amount);
            Database::getInstance()->addTransaction(accountNumber, std::move(transaction));
            
            // Save changes to database
            Database::getInstance()->saveAll();
            return true;
        } else {
            std::cerr << "Insufficient funds" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool BankApp::transfer(int fromAccount, int toAccount, double amount, const std::string& password) {
    try {
        // Check if transferring to the same account
        if (fromAccount == toAccount) {
            std::cerr << "Cannot transfer to the same account" << std::endl;
            return false;
        }
        
        Account* fromAcc = Database::getInstance()->getAccount(fromAccount);
        Account* toAcc = Database::getInstance()->getAccount(toAccount);
        
        if (!fromAcc || !toAcc) {
            std::cerr << "Account not found" << std::endl;
            return false;
        }
        
        // Verify account password for the source account
        if (!Database::getInstance()->verifyPassword(fromAccount, password)) {
            std::cerr << "Incorrect password" << std::endl;
            return false;
        }
        
        // Perform transfer
        if (fromAcc->withdraw(amount)) {
            toAcc->deposit(amount);
            
            // Create and save transaction records for both accounts
            auto fromTransaction = std::make_unique<Transfer>(fromAcc, toAcc, amount);
            
            Database::getInstance()->addTransaction(fromAccount, std::move(fromTransaction));
            
            // Save changes to database
            Database::getInstance()->saveAll();
            return true;
        } else {
            std::cerr << "Insufficient funds" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool BankApp::closeAccount(int accountNumber, const std::string& password) {
    try {
        Account* account = Database::getInstance()->getAccount(accountNumber);
        if (!account) {
            std::cerr << "Account not found" << std::endl;
            return false;
        }
        
        // Verify account password
        if (!Database::getInstance()->verifyPassword(accountNumber, password)) {
            std::cerr << "Incorrect password" << std::endl;
            return false;
        }
        
        double remainingBalance = account->getBalance();
        if (remainingBalance > 0) {
            // Create and execute withdrawal for remaining balance
            auto withdrawal = std::make_unique<Withdrawal>(account, remainingBalance);
            if (withdrawal->execute()) {
                Database::getInstance()->addTransaction(accountNumber, std::move(withdrawal));
            } else {
                std::cerr << "Failed to withdraw remaining balance" << std::endl;
                return false;
            }
        }
        
        // Close the account
        if (Database::getInstance()->removeAccount(accountNumber)) {
            Database::getInstance()->saveAll();
            return true;
        } else {
            std::cerr << "Failed to close account" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

std::string BankApp::getAccounts(const std::string& username) {
    try {
        int customerId = Database::getInstance()->getCustomerIdByUsername(username);
        if (customerId == -1) {
            return "[]";
        }
        
        Customer* customer = Database::getInstance()->findCustomer(customerId);
        if (!customer) {
            return "[]";
        }
        
        std::string result = "[";
        const auto& accounts = customer->getAccounts();
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (i > 0) result += ",";
            result += "{";
            result += "\"accountNumber\":" + std::to_string(accounts[i]->getAccountNumber()) + ",";
            result += "\"type\":\"" + accounts[i]->getTypeString() + "\",";
            result += "\"balance\":" + std::to_string(accounts[i]->getBalance());
            result += "}";
        }
        result += "]";
        return result;
    } catch (const std::exception& e) {
        return "[]";
    }
}

std::string BankApp::getAccountDetails(int accountNumber) {
    try {
        Account* account = Database::getInstance()->getAccount(accountNumber);
        if (!account) {
            return "{}";
        }
        
        std::string result = "{";
        result += "\"accountNumber\":" + std::to_string(account->getAccountNumber()) + ",";
        result += "\"type\":\"" + account->getTypeString() + "\",";
        result += "\"balance\":" + std::to_string(account->getBalance()) + ",";
        result += "\"owner\":\"" + account->getOwner()->getName() + "\"";
        result += "}";
        return result;
    } catch (const std::exception& e) {
        return "{}";
    }
}

std::string BankApp::getTransactions(int accountNumber) {
    try {
        Account* account = Database::getInstance()->getAccount(accountNumber);
        if (!account) {
            std::cerr << "Account " << accountNumber << " not found" << std::endl;
            return "[]";
        }
        
        // Read transactions from the database file
        std::string result = "[";
        std::string transactionFilePath = "../data/transactions.txt";
        std::ifstream file(transactionFilePath);
        
        std::cerr << "Opening transaction file: " << transactionFilePath << std::endl;
        
        if (file.is_open()) {
            std::string line;
            bool firstTransaction = true;
            int lineCount = 0;
            double runningBalance = 0.0; // Track running balance
            
            while (std::getline(file, line)) {
                lineCount++;
                std::cerr << "Reading line " << lineCount << ": " << line << std::endl;
                
                std::stringstream ss(line);
                std::string accNumStr, timestamp, typeStr, amountStr, relatedAccountStr;
                
                // Parse using colons as separators
                std::getline(ss, accNumStr, ':');
                std::getline(ss, timestamp, ':');
                std::getline(ss, typeStr, ':');
                std::getline(ss, amountStr, ':');
                std::getline(ss, relatedAccountStr); // This might be empty for deposits/withdrawals
                
                int accNum = std::stoi(accNumStr);
                std::cerr << "Account number from line: " << accNum << ", looking for: " << accountNumber << std::endl;
                
                if (accNum != accountNumber) {
                    std::cerr << "Skipping line - account mismatch" << std::endl;
                    continue;
                }
                
                std::cerr << "Parsed: timestamp='" << timestamp << "', type='" << typeStr << "', amount='" << amountStr << "', related='" << relatedAccountStr << "'" << std::endl;
                
                if (!timestamp.empty() && !typeStr.empty() && !amountStr.empty()) {
                    if (!firstTransaction) {
                        result += ",";
                    }
                    
                    double amount = std::stod(amountStr);
                    int typeInt = std::stoi(typeStr);
                    
                    // Update running balance based on transaction type
                    if (typeInt == static_cast<int>(TransactionType::DEPOSIT) || 
                        (typeInt == static_cast<int>(TransactionType::TRANSFER))) {
                        runningBalance += amount;
                    } else if (typeInt == static_cast<int>(TransactionType::WITHDRAWAL)) {
                        runningBalance -= amount; // amount is already negative for withdrawals/transfer out
                    }
                    
                    std::string typeName;
                    std::string relatedAccount = "null";
                    
                    switch (typeInt) {
                        case static_cast<int>(TransactionType::DEPOSIT):
                            typeName = "Deposit";
                            break;
                        case static_cast<int>(TransactionType::WITHDRAWAL):
                            typeName = "Withdrawal";
                            break;
                        case static_cast<int>(TransactionType::TRANSFER):
                            // Determine if this is a transfer in or out based on amount
                            if (amount > 0) {
                                typeName = "Transfer In";
                                if (!relatedAccountStr.empty()) {
                                    relatedAccount = "\"From " + relatedAccountStr + "\"";
                                }
                            } else {
                                typeName = "Transfer Out";
                                if (!relatedAccountStr.empty()) {
                                    relatedAccount = "\"To " + relatedAccountStr + "\"";
                                }
                            }
                            break;
                        default:
                            typeName = "Unknown";
                    }
                    
                    result += "{";
                    result += "\"timestamp\":\"" + timestamp + "\",";
                    result += "\"type\":\"" + typeName + "\",";
                    result += "\"amount\":" + std::to_string(amount) + ",";
                    result += "\"relatedAccount\":" + relatedAccount + ",";
                    result += "\"balance\":" + std::to_string(runningBalance);
                    result += "}";
                    
                    std::cerr << "Added transaction to result with balance: " << runningBalance << std::endl;
                    firstTransaction = false;
                } else {
                    std::cerr << "Skipping line - missing required fields" << std::endl;
                }
            }
            
            std::cerr << "Processed " << lineCount << " lines total" << std::endl;
        } else {
            std::cerr << "Failed to open transaction file" << std::endl;
        }
        
        result += "]";
        std::cerr << "Final result: " << result << std::endl;
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Exception in getTransactions: " << e.what() << std::endl;
        return "[]";
    }
}

std::string BankApp::getUserDetails(const std::string& username) {
    try {
        int customerId = Database::getInstance()->getCustomerIdByUsername(username);
        if (customerId == -1) {
            return "{}";
        }
        
        Customer* customer = Database::getInstance()->findCustomer(customerId);
        if (!customer) {
            return "{}";
        }
        
        std::string result = "{";
        result += "\"id\":" + std::to_string(customer->getId()) + ",";
        result += "\"name\":\"" + customer->getName() + "\",";
        result += "\"username\":\"" + username + "\",";
        result += "\"phone\":\"" + customer->getPhone() + "\"";
        result += "}";
        return result;
    } catch (const std::exception& e) {
        return "{}";
    }
}

bool BankApp::updateProfile(const std::string& username, const std::string& name, const std::string& phone) {
    try {
        // Validate input
        if (!isValidName(name)) {
            std::cerr << "Invalid name format" << std::endl;
            return false;
        }
        if (!isValidPhone(phone)) {
            std::cerr << "Invalid phone number format" << std::endl;
            return false;
        }
        
        int customerId = Database::getInstance()->getCustomerIdByUsername(username);
        if (customerId == -1) {
            std::cerr << "User not found" << std::endl;
            return false;
        }
        
        Customer* customer = Database::getInstance()->findCustomer(customerId);
        if (!customer) {
            std::cerr << "Customer not found" << std::endl;
            return false;
        }
        
        // Update customer information
        customer->setName(name);
        customer->setPhone(phone);
        
        // Save changes to database
        Database::getInstance()->saveAll();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error updating profile: " << e.what() << std::endl;
        return false;
    }
}

bool BankApp::changePassword(const std::string& username, const std::string& currentPassword, const std::string& newPassword) {
    try {
        // Validate new password
        if (!isValidPassword(newPassword)) {
            std::cerr << "Invalid new password format" << std::endl;
            return false;
        }
        
        // Verify current password
        int customerId;
        if (!Database::getInstance()->authenticate(username, currentPassword, customerId)) {
            std::cerr << "Current password is incorrect" << std::endl;
            return false;
        }
        
        // Change password in database
        if (Database::getInstance()->changePassword(customerId, currentPassword, newPassword)) {
            return true;
        } else {
            std::cerr << "Failed to update password in database" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error changing password: " << e.what() << std::endl;
        return false;
    }
} 