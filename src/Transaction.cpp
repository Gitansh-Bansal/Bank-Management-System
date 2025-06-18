#include "../include/Transaction.h"
#include "../include/Database.h"
#include "../include/Account.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <chrono>
#include <fstream>

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
        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│        Transaction Menu     │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "├─────────────────────────────┤" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│  1. Deposit                 │" << std::endl;
        std::cout << "│  2. Withdraw                │" << std::endl;
        std::cout << "│  3. Transfer                │" << std::endl;
        std::cout << "│  4. Check Balance           │" << std::endl;
        std::cout << "│  5. Statement               │" << std::endl;
        std::cout << "│  6. Close Account           │" << std::endl;
        std::cout << "│  7. Return to Main Menu     │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;
        std::cout << "Enter your choice: ";
        choice = getIntInput("");

        switch (choice) {
            case 1: { // Deposit
                amount = getDoubleInput("Enter amount to deposit: $");
                
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
                        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
                        std::cout << "│         Transaction Result        │" << std::endl;
                        std::cout << "├───────────────────────────────────┤" << std::endl;
                        std::cout << "│ Deposit Successful!               │" << std::endl;
                        std::cout << "│ Amount Deposited: $" << std::fixed << std::setprecision(2) << std::setw(12) << amount << "   │" << std::endl;
                        std::cout << "│ Updated Balance:  $" << std::fixed << std::setprecision(2) << std::setw(12) << account->getBalance() << "   │" << std::endl;
                        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
                    } else {
                        std::cout << "Failed to record deposit transaction.\n";
                    }
                } else {
                    std::cout << "Deposit failed.\n";
                }
                break;
            }
            case 2: { // Withdraw
                amount = getDoubleInput("Enter amount to withdraw: $");
                
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
                        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
                        std::cout << "│         Transaction Result        │" << std::endl;
                        std::cout << "├───────────────────────────────────┤" << std::endl;
                        std::cout << "│ Withdrawal Successful!            │" << std::endl;
                        std::cout << "│ Amount Withdrawn: $" << std::fixed << std::setprecision(2) << std::setw(12) << amount << "   │" << std::endl;
                        std::cout << "│ Updated Balance:  $" << std::fixed << std::setprecision(2) << std::setw(12) << account->getBalance() << "   │" << std::endl;
                        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
                    } else {
                        std::cout << "Failed to record withdrawal transaction.\n";
                    }
                } else {
                    std::cout << "Withdrawal failed.\n";
                }
                break;
            }
            case 3: { // Transfer
                targetAccount = getIntInput("Enter target account number: ");
                
                // Check if target account is same as source account
                if (targetAccount == accountNumber) {
                    std::cout << "Cannot transfer to the same account.\n";
                    break;
                }
                
                amount = getDoubleInput("Enter amount to transfer: $");

                if (amount <= 0) {
                    std::cout << "Invalid amount. Please enter a positive number.\n";
                    break;
                }

                auto fromAccount = Database::getAccount(accountNumber);
                auto toAccount = Database::getAccount(targetAccount);

                if (!fromAccount || !toAccount) {
                    std::cout << "Sender or Receiver Account not found.\n";
                    break;
                }

                auto transfer = std::make_unique<Transfer>(fromAccount, toAccount, amount);
                if (transfer->execute()) {
                    if (db->addTransaction(accountNumber, std::move(transfer))) {
                        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
                        std::cout << "│         Transaction Result        │" << std::endl;
                        std::cout << "├───────────────────────────────────┤" << std::endl;
                        std::cout << "│ Transfer Successful!              │" << std::endl;
                        std::cout << "│ Receiving Account: " << std::setw(15) << targetAccount << "│" << std::endl;
                        std::cout << "│ Amount Transferred: $" << std::fixed << std::setprecision(2) << std::setw(12) << amount << " │" << std::endl;
                        std::cout << "│ Updated Balance:    $" << std::fixed << std::setprecision(2) << std::setw(12) << fromAccount->getBalance() << " │" << std::endl;
                        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
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
                    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
                    std::cout << "│          Account Balance          │" << std::endl;
                    std::cout << "├───────────────────────────────────┤" << std::endl;
                    std::cout << "│ Account Number: " << std::setw(12) << account->getAccountNumber() << "      │" << std::endl;
                    std::cout << "│ Current Balance: $" << std::fixed << std::setprecision(2) << std::setw(12) << account->getBalance() << "    │" << std::endl;
                    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
                } else {
                    std::cout << "Account not found.\n";
                }
                break;
            }
            case 5: { // Statement
                printAccountStatement(accountNumber);
                break;
            }
            case 6: {
                closeAccount(accountNumber);
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                return;
            }
            case 7: // Return to Main Menu
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void printAccountStatement(int accountNumber) {
    Database* db = Database::getInstance();
    db->getTransactions(accountNumber, std::cout);
}

void closeAccount(int accountNumber) {
    Database* db = Database::getInstance();
    auto account = Database::getAccount(accountNumber);
    
    if (!account) {
        std::cout << "Account not found.\n";
        return;
    }

    // Confirm account closure
    std::cout << "Are you sure you want to close this account? (yes/no): ";
    std::string confirmation;
    std::cin >> confirmation;

    if (confirmation != "yes" && confirmation != "YES" && confirmation != "Yes") {
        std::cout << "Account closure cancelled.\n";
        return;
    }

    double remainingBalance = account->getBalance();
    if (remainingBalance > 0) {
        // Create and execute withdrawal for remaining balance
        auto withdrawal = std::make_unique<Withdrawal>(account, remainingBalance);
        if (withdrawal->execute()) {
            if (db->addTransaction(accountNumber, std::move(withdrawal))) {
                std::cout << "Remaining balance of $" << remainingBalance << " has been withdrawn.\n";
            } else {
                std::cout << "Failed to record withdrawal transaction.\n";
                return;
            }
        } else {
            std::cout << "Failed to withdraw remaining balance.\n";
            return;
        }
    }

    // Print final statement
    std::cout << "\nFinal Account Statement:\n";
    printAccountStatement(accountNumber);

    // Close the account
    if (db->removeAccount(accountNumber)) {
        std::cout << "Account closed successfully.\n";
    } else {
        std::cout << "Failed to close account.\n";
    }
}

// Deposit implementation
Deposit::Deposit(Account* account, double amount)
    : account(account), amount(amount), type(TransactionType::DEPOSIT) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
    // Set current timestamp
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H-%M-%S");
    timestamp = ss.str();
}

bool Deposit::execute() {
    account->deposit(amount);
    return true;
}

bool Deposit::undo() {
    return account->withdraw(amount);
}

// Withdrawal implementation
Withdrawal::Withdrawal(Account* account, double amount)
    : account(account), amount(amount), type(TransactionType::WITHDRAWAL) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
    // Set current timestamp
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H-%M-%S");
    timestamp = ss.str();
}

bool Withdrawal::execute() {
    return account->withdraw(amount);
}

bool Withdrawal::undo() {
    account->deposit(amount);
    return true;
}

// Transfer implementation
Transfer::Transfer(Account* from, Account* to, double amount)
    : fromAccount(from), toAccount(to), amount(amount), type(TransactionType::TRANSFER) {
    if (!from || !to) {
        throw std::invalid_argument("Accounts cannot be null");
    }
    if (amount <= 0) {
        throw std::invalid_argument("Amount must be positive");
    }
    if (from == to) {
        throw std::invalid_argument("Cannot transfer to the same account");
    }
    // Set current timestamp
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H-%M-%S");
    timestamp = ss.str();
}

bool Transfer::execute() {
    if (fromAccount->withdraw(amount)) {
        toAccount->deposit(amount);
        return true;
    }
    return false;
}

bool Transfer::undo() {
    if (toAccount->withdraw(amount)) {
        fromAccount->deposit(amount);
        return true;
    }
    return false;
} 