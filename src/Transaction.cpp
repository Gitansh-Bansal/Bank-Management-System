#include "../include/Transaction.h"
#include "../include/Database.h"
#include "../include/Account.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

// Helper function for safe double input
static double getDoubleInput(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();  // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear input buffer
        std::cout << "Invalid input! Please enter a valid number." << std::endl;
    }
}

// Helper function for safe integer input
static int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();  // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear input buffer
        std::cout << "Invalid input! Please enter a valid number." << std::endl;
    }
}

// Forward declaration of menu function
void menu(int accountNumber);

bool verifyPasswordWithAttempts(int accountNumber) {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;
    
    while (attempts < MAX_ATTEMPTS) {
        std::string password;
        std::cout << "Enter password (attempt " << (attempts + 1) << "/" << MAX_ATTEMPTS << "): ";
        std::cin >> password;
        
        if (Database::verifyPassword(accountNumber, password)) {
            return true;
        }
        
        attempts++;
        if (attempts < MAX_ATTEMPTS) {
            std::cout << "Incorrect password. Please try again.\n";
        }
    }
    
    std::cout << "Too many failed attempts. Access denied.\n";
    return false;
}

void transactionFun(int accountNumber) {
    if (!verifyPasswordWithAttempts(accountNumber)) {
        return;
    }

    Database* db = Database::getInstance();
    int choice;
    double amount;
    int targetAccount;

    while (true) {
        std::cout << "\nTransaction Menu:\n";
        std::cout << "1. Deposit\n";
        std::cout << "2. Withdraw\n";
        std::cout << "3. Transfer\n";
        std::cout << "4. Check Balance\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: { // Deposit
                std::cout << "Enter amount to deposit: ";
                std::cin >> amount;
                
                if (amount <= 0) {
                    std::cout << "Invalid amount. Please enter a positive number.\n";
                    break;
                }

                auto account = Database::getAccount(accountNumber);
                if (!account) {
                    std::cout << "Account not found.\n";
                    break;
                }

                auto deposit = std::make_unique<Deposit>(account, amount);
                if (deposit->execute()) {
                    if (db->addTransaction(accountNumber, std::move(deposit))) {
                        std::cout << "Deposit successful.\n";
                    } else {
                        std::cout << "Failed to record deposit transaction.\n";
                    }
                } else {
                    std::cout << "Deposit failed.\n";
                }
                break;
            }
            case 2: { // Withdraw
                std::cout << "Enter amount to withdraw: ";
                std::cin >> amount;
                
                if (amount <= 0) {
                    std::cout << "Invalid amount. Please enter a positive number.\n";
                    break;
                }

                auto account = Database::getAccount(accountNumber);
                if (!account) {
                    std::cout << "Account not found.\n";
                    break;
                }

                auto withdrawal = std::make_unique<Withdrawal>(account, amount);
                if (withdrawal->execute()) {
                    if (db->addTransaction(accountNumber, std::move(withdrawal))) {
                        std::cout << "Withdrawal successful.\n";
                    } else {
                        std::cout << "Failed to record withdrawal transaction.\n";
                    }
                } else {
                    std::cout << "Withdrawal failed.\n";
                }
                break;
            }
            case 3: { // Transfer
                std::cout << "Enter target account number: ";
                std::cin >> targetAccount;
                std::cout << "Enter amount to transfer: ";
                std::cin >> amount;

                if (amount <= 0) {
                    std::cout << "Invalid amount. Please enter a positive number.\n";
                    break;
                }

                auto fromAccount = Database::getAccount(accountNumber);
                auto toAccount = Database::getAccount(targetAccount);

                if (!fromAccount || !toAccount) {
                    std::cout << "One or both accounts not found.\n";
                    break;
                }

                auto transfer = std::make_unique<Transfer>(fromAccount, toAccount, amount);
                if (transfer->execute()) {
                    if (db->addTransaction(accountNumber, std::move(transfer))) {
                        std::cout << "Transfer successful.\n";
                    } else {
                        std::cout << "Failed to record transfer transaction.\n";
                    }
                } else {
                    std::cout << "Transfer failed.\n";
                }
                break;
            }
            case 4: { // Check Balance
                auto account = Database::getAccount(accountNumber);
                if (account) {
                    std::cout << "Current balance: $" << account->getBalance() << "\n";
                } else {
                    std::cout << "Account not found.\n";
                }
                break;
            }
            case 5: // Return to Main Menu
                menu(accountNumber);
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Deposit implementation
Deposit::Deposit(Account* account, double amount, const std::string& description)
    : account(account), amount(amount), description(description) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
}

bool Deposit::execute() {
    return account->deposit(amount);
}

bool Deposit::undo() {
    return account->withdraw(amount);
}

std::string Deposit::getDescription() const {
    std::stringstream ss;
    ss << description << " of $" << std::fixed << std::setprecision(2) << amount;
    return ss.str();
}

// Withdrawal implementation
Withdrawal::Withdrawal(Account* account, double amount, const std::string& description)
    : account(account), amount(amount), description(description) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
}

bool Withdrawal::execute() {
    return account->withdraw(amount);
}

bool Withdrawal::undo() {
    return account->deposit(amount);
}

std::string Withdrawal::getDescription() const {
    std::stringstream ss;
    ss << description << " of $" << std::fixed << std::setprecision(2) << amount;
    return ss.str();
}

// Transfer implementation
Transfer::Transfer(Account* from, Account* to, double amount, const std::string& description)
    : fromAccount(from), toAccount(to), amount(amount), description(description) {
    if (!from || !to) {
        throw std::invalid_argument("Accounts cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
    if (from == to) {
        throw std::invalid_argument("Cannot transfer to the same account");
    }
}

bool Transfer::execute() {
    if (fromAccount->withdraw(amount)) {
        if (toAccount->deposit(amount)) {
            return true;
        }
        // If deposit fails, undo the withdrawal
        fromAccount->deposit(amount);
    }
    return false;
}

bool Transfer::undo() {
    if (toAccount->withdraw(amount)) {
        if (fromAccount->deposit(amount)) {
            return true;
        }
        // If deposit fails, undo the withdrawal
        toAccount->deposit(amount);
    }
    return false;
}

std::string Transfer::getDescription() const {
    std::stringstream ss;
    ss << description << " of $" << std::fixed << std::setprecision(2) << amount 
       << " from account " << fromAccount->getAccountNumber() 
       << " to account " << toAccount->getAccountNumber();
    return ss.str();
} 