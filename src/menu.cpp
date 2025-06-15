#include <iostream>
#include <limits>
#include "Transaction.h"
#include "Database.h"
#include "BankApp.h"

void menu(int accountNumber) {
    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│        Account Menu         │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "├─────────────────────────────┤" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│  1. Deposit                 │" << std::endl;
    std::cout << "│  2. Withdrawal              │" << std::endl;
    std::cout << "│  3. Transfer                │" << std::endl;
    std::cout << "│  4. View Balance            │" << std::endl;
    std::cout << "│  5. View Transaction History│" << std::endl;
    std::cout << "│  6. Back to Main Menu       │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;

    int choice;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    BankApp* app = BankApp::getInstance();
    if (!app) {
        std::cout << "Error: Bank application not initialized." << std::endl;
        return;
    }

    switch (choice) {
        case 1:
            app->performTransaction(accountNumber, "deposit");
            break;
        case 2:
            app->performTransaction(accountNumber, "withdrawal");
            break;
        case 3:
            app->performTransaction(accountNumber, "transfer");
            break;
        case 4: {
            auto account = Database::getAccount(accountNumber);
            if (account) {
                std::cout << "Current balance: $" << account->getBalance() << std::endl;
            } else {
                std::cout << "Account not found." << std::endl;
            }
            break;
        }
        case 5: {
            auto account = Database::getAccount(accountNumber);
            if (account) {
                std::cout << "\nTransaction History:" << std::endl;
                for (const auto& transaction : account->getTransactionHistory()) {
                    std::cout << transaction->getTimestamp() << " - "
                              << transaction->getDescription() << " - $"
                              << transaction->getAmount() << std::endl;
                }
            } else {
                std::cout << "Account not found." << std::endl;
            }
            break;
        }
        case 6:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
    
    menu(accountNumber);  // Recursive call to show menu again
} 