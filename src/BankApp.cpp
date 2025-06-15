#include "../include/BankApp.h"
#include "../include/Database.h"
#include <iostream>
#include <limits>
#include <iomanip>

BankApp* BankApp::instance = nullptr;

BankApp* BankApp::getInstance(const std::string& bankName) {
    if (instance == nullptr) {
        instance = new BankApp(bankName);
    }
    return instance;
}

BankApp::BankApp(const std::string& bankName)
    : bank(std::make_unique<Bank>(bankName)), currentCustomer(nullptr), currentAccount(nullptr) {
    // Initialize database
    Database::getInstance();
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
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void BankApp::displayMainMenu() {
    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│        Welcome to " << std::left << std::setw(10) << bank->getName() << "│" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "├─────────────────────────────┤" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "│  1. Login                   │" << std::endl;
    std::cout << "│  2. Register                │" << std::endl;
    std::cout << "│  3. Exit                    │" << std::endl;
    std::cout << "│                             │" << std::endl;
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;
}

void BankApp::handleCustomerLogin() {
    std::string username, password;
    std::cout << "\nEnter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    int customerId;
    if (Database::getInstance()->authenticate(username, password, customerId)) {
        currentCustomer = bank->findCustomer(customerId);
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

void BankApp::handleCustomerRegistration() {
    std::string name, phone, username, password;
    std::cout << "\nEnter your name: ";
    std::getline(std::cin, name);
    std::cout << "Enter your phone number: ";
    std::getline(std::cin, phone);
    std::cout << "Choose a username: ";
    std::getline(std::cin, username);
    std::cout << "Choose a password: ";
    std::getline(std::cin, password);

    if (Database::getInstance()->usernameExists(username)) {
        std::cout << "Username already exists. Please choose another one." << std::endl;
        return;
    }

    // Add customer to bank first
    currentCustomer = bank->addCustomer(name, phone);
    if (!currentCustomer) {
        std::cout << "Error: Failed to create customer." << std::endl;
        return;
    }

    // Create a unique_ptr for the database
    auto customerPtr = std::make_unique<Customer>(currentCustomer->getId(), name, phone);
    
    // Then add to database with authentication
    if (Database::getInstance()->addCustomer(std::move(customerPtr), username, password)) {
        std::cout << "Registration successful! Your customer ID is: " << currentCustomer->getId() << std::endl;
        displayCustomerMenu();
    } else {
        std::cout << "Registration failed." << std::endl;
        // Remove customer from bank if database registration fails
        bank->removeCustomer(currentCustomer->getId());
        currentCustomer = nullptr;
    }
}

void BankApp::displayCustomerMenu() {
    while (true) {
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

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                handleAccountCreation();
                break;
            case 2:
                handleAccountSelection();
                break;
            case 3:
                listAccounts();
                break;
            case 4:
                currentCustomer = nullptr;
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void BankApp::handleAccountCreation() {
    std::cout << "\nSelect account type:" << std::endl;
    std::cout << "1: Savings Account" << std::endl;
    std::cout << "2: Current Account" << std::endl;
    std::cout << "3: Auditable Savings Account" << std::endl;
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
        Account* account = nullptr;
        switch (choice) {
            case 1:
                account = bank->createSavingsAccount(currentCustomer->getId(), initialBalance);
                break;
            case 2:
                account = bank->createCurrentAccount(currentCustomer->getId(), initialBalance);
                break;
            case 3:
                account = bank->createAuditableSavingsAccount(currentCustomer->getId(), initialBalance);
                break;
            default:
                std::cout << "Invalid account type." << std::endl;
                return;
        }

        if (account) {
            std::cout << "Account created successfully! Account number: " << account->getAccountNumber() << std::endl;
            Database::getInstance()->addAccount(std::unique_ptr<Account>(account), password);
        }
    } catch (const std::exception& e) {
        std::cout << "Error creating account: " << e.what() << std::endl;
    }
}

void BankApp::handleAccountSelection() {
    listAccounts();
    std::cout << "Enter account number: ";
    int accountNumber;
    std::cin >> accountNumber;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    currentAccount = bank->findAccount(accountNumber);
    if (currentAccount && currentAccount->getOwner() == currentCustomer) {
        displayAccountMenu();
    } else {
        std::cout << "Invalid account number." << std::endl;
    }
}

void BankApp::listAccounts() {
    std::cout << "\nYour Accounts:" << std::endl;
    for (const auto& account : bank->getAccounts()) {
        if (account->getOwner() == currentCustomer) {
            std::cout << "Account Number: " << account->getAccountNumber()
                      << ", Balance: $" << std::fixed << std::setprecision(2) 
                      << account->getBalance() << std::endl;
        }
    }
}

void BankApp::displayAccountMenu() {
    while (true) {
        std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│        Account Menu         │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "├─────────────────────────────┤" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "│  1. Deposit                 │" << std::endl;
        std::cout << "│  2. Withdraw                │" << std::endl;
        std::cout << "│  3. Transfer                │" << std::endl;
        std::cout << "│  4. Statement               │" << std::endl;
        std::cout << "│  5. Close Account           │" << std::endl;
        std::cout << "│  6. Back to Customer Menu   │" << std::endl;
        std::cout << "│                             │" << std::endl;
        std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl << std::endl;

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                handleDeposit();
                break;
            case 2:
                handleWithdrawal();
                break;
            case 3:
                handleTransfer();
                break;
            case 4:
                handleAccountStatement();
                break;
            case 5:
                handleAccountClosure();
                return;
            case 6:
                currentAccount = nullptr;
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void BankApp::handleDeposit() {
    std::cout << "Enter amount to deposit: $";
    double amount;
    std::cin >> amount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (bank->processDeposit(currentAccount->getAccountNumber(), amount)) {
        std::cout << "Deposit successful!" << std::endl;
    } else {
        std::cout << "Deposit failed." << std::endl;
    }
}

void BankApp::handleWithdrawal() {
    std::cout << "Enter amount to withdraw: $";
    double amount;
    std::cin >> amount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (bank->processWithdrawal(currentAccount->getAccountNumber(), amount)) {
        std::cout << "Withdrawal successful!" << std::endl;
    } else {
        std::cout << "Withdrawal failed." << std::endl;
    }
}

void BankApp::handleTransfer() {
    std::cout << "Enter recipient account number: ";
    int toAccount;
    std::cin >> toAccount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter amount to transfer: $";
    double amount;
    std::cin >> amount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (bank->processTransfer(currentAccount->getAccountNumber(), toAccount, amount)) {
        std::cout << "Transfer successful!" << std::endl;
    } else {
        std::cout << "Transfer failed." << std::endl;
    }
}

void BankApp::handleAccountStatement() {
    if (!currentAccount) {
        std::cout << "No account selected." << std::endl;
        return;
    }

    std::cout << "\n┌─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┐" << std::endl;
    std::cout << "│                     Account Statement                    │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ Account Number: " << std::setw(40) << currentAccount->getAccountNumber() << "│" << std::endl;
    std::cout << "│ Account Type: " << std::setw(42) << (currentAccount->getType() == AccountType::SAVINGS ? "Savings" :
                                                         currentAccount->getType() == AccountType::CURRENT ? "Current" :
                                                         "Auditable Savings") << "│" << std::endl;
    std::cout << "│ Current Balance: $" << std::fixed << std::setprecision(2) << std::setw(36) 
              << currentAccount->getBalance() << "│" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│                     Transaction History                  │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────┤" << std::endl;

    const auto& transactions = currentAccount->getTransactionHistory();
    if (transactions.empty()) {
        std::cout << "│ No transactions found.                                    │" << std::endl;
    } else {
        for (const auto& transaction : transactions) {
            std::cout << "│ " << std::left << std::setw(20) << transaction->getTimestamp()
                      << std::setw(20) << transaction->getDescription()
                      << std::right << std::setw(15) << "$" << std::fixed << std::setprecision(2)
                      << transaction->getAmount() << " │" << std::endl;
        }
    }
    std::cout << "└─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─x─┘" << std::endl;
}

void BankApp::handleAccountClosure() {
    if (bank->closeAccount(currentAccount->getAccountNumber())) {
        std::cout << "Account closed successfully." << std::endl;
        currentAccount = nullptr;
    } else {
        std::cout << "Failed to close account." << std::endl;
    }
}

void BankApp::performTransaction(int accountNumber, const std::string& type) {
    currentAccount = Database::getAccount(accountNumber);
    if (!currentAccount) {
        std::cout << "Account not found." << std::endl;
        return;
    }

    if (type == "deposit") {
        handleDeposit();
    } else if (type == "withdrawal") {
        handleWithdrawal();
    } else if (type == "transfer") {
        handleTransfer();
    } else {
        std::cout << "Invalid transaction type." << std::endl;
    }
} 