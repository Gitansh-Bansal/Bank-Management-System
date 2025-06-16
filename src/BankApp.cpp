#include "../include/BankApp.h"
#include "../include/Database.h"
#include "../include/Transaction.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <memory>

BankApp* BankApp::instance = nullptr;

BankApp::BankApp(const std::string& bankName)
    : bankName(bankName), currentCustomer(nullptr), currentAccount(nullptr) {
    // Initialize database
    Database::getInstance();
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

// void BankApp::displayAccountMenu() {
//     while (true) {
//         std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
//         std::cout << "│                             │" << std::endl;
//         std::cout << "│        Account Menu         │" << std::endl;
//         std::cout << "│                             │" << std::endl;
//         std::cout << "├─────────────────────────────┤" << std::endl;
//         std::cout << "│                             │" << std::endl;
//         std::cout << "│  1. Deposit                 │" << std::endl;
//         std::cout << "│  2. Withdraw                │" << std::endl;
//         std::cout << "│  3. Transfer                │" << std::endl;
//         std::cout << "│  4. Statement               │" << std::endl;
//         std::cout << "│  5. Close Account           │" << std::endl;
//         std::cout << "│  6. Back to Customer Menu   │" << std::endl;
//         std::cout << "│                             │" << std::endl;
//         std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;

//         int choice;
//         std::cin >> choice;
//         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//         switch (choice) {
//             case 1:
//                 handleDeposit();
//                 break;
//             case 2:
//                 handleWithdrawal();
//                 break;
//             case 3:
//                 handleTransfer();
//                 break;
//             case 4:
//                 handleAccountStatement();
//                 break;
//             case 5:
//                 handleAccountClosure();
//                 return;
//             case 6:
//                 currentAccount = nullptr;
//                 return;
//             default:
//                 std::cout << "Invalid choice!" << std::endl;
//         }
//     }
// }

// void BankApp::handleDeposit() {
//     std::cout << "Enter amount to deposit: $";
//     double amount;
//     std::cin >> amount;
//     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//     if (bank->processDeposit(currentAccount->getAccountNumber(), amount)) {
//         std::cout << "Deposit successful!" << std::endl;
//     } else {
//         std::cout << "Deposit failed." << std::endl;
//     }
// }

// void BankApp::handleWithdrawal() {
//     std::cout << "Enter amount to withdraw: $";
//     double amount;
//     std::cin >> amount;
//     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//     if (bank->processWithdrawal(currentAccount->getAccountNumber(), amount)) {
//         std::cout << "Withdrawal successful!" << std::endl;
//     } else {
//         std::cout << "Withdrawal failed." << std::endl;
//     }
// }

// void BankApp::handleTransfer() {
//     std::cout << "Enter recipient account number: ";
//     int toAccount;
//     std::cin >> toAccount;
//     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//     std::cout << "Enter amount to transfer: $";
//     double amount;
//     std::cin >> amount;
//     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//     if (bank->processTransfer(currentAccount->getAccountNumber(), toAccount, amount)) {
//         std::cout << "Transfer successful!" << std::endl;
//     } else {
//         std::cout << "Transfer failed." << std::endl;
//     }
// }

// void BankApp::handleAccountStatement() {
//     if (!currentAccount) {
//         std::cout << "No account selected." << std::endl;
//         return;
//     }

//     std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
//     std::cout << "│                     Account Statement                    │" << std::endl;
//     std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;
//     std::cout << "│ Account Number: " << std::setw(40) << currentAccount->getAccountNumber() << "│" << std::endl;
//     std::cout << "│ Account Type: " << std::setw(42) << (currentAccount->getType() == AccountType::SAVINGS ? "Savings" :
//                                                          currentAccount->getType() == AccountType::CURRENT ? "Current" :
//                                                          "Auditable Savings") << "│" << std::endl;
//     std::cout << "│ Current Balance: $" << std::fixed << std::setprecision(2) << std::setw(36) 
//               << currentAccount->getBalance() << "│" << std::endl;
//     std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;
//     std::cout << "│                     Transaction History                  │" << std::endl;
//     std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;

//     const auto& transactions = currentAccount->getTransactionHistory();
//     if (transactions.empty()) {
//         std::cout << "│ No transactions found.                                    │" << std::endl;
//     } else {
//         for (const auto& transaction : transactions) {
//             std::cout << "│ " << std::left << std::setw(20) << transaction->getTimestamp()
//                       << std::setw(20) << transaction->getDescription()
//                       << std::right << std::setw(15) << "$" << std::fixed << std::setprecision(2)
//                       << transaction->getAmount() << " │" << std::endl;
//         }
//     }
//     std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
// }

// void BankApp::handleAccountClosure() {
//     if (Database::getInstance()->removeAccount(currentAccount->getAccountNumber())) {
//         std::cout << "Account closed successfully." << std::endl;
//         currentAccount = nullptr;
//     } else {
//         std::cout << "Failed to close account." << std::endl;
//     }
// } 