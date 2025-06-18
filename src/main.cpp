#include "../include/BankApp.h"
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    try {
        // If command-line arguments are provided, handle API mode
        if (argc > 1) {
            BankApp* app = BankApp::getInstance("Apna UniBank");
            std::string command = argv[1];
            
            if (command == "register" && argc == 6) {
                std::string name = argv[2];
                std::string phone = argv[3];
                std::string username = argv[4];
                std::string password = argv[5];
                
                // Validate input
                if (!app->isValidName(name)) {
                    std::cerr << "Error: Invalid name format" << std::endl;
                    return 1;
                }
                if (!app->isValidPhone(phone)) {
                    std::cerr << "Error: Invalid phone number format" << std::endl;
                    return 1;
                }
                if (!app->isValidUsername(username)) {
                    std::cerr << "Error: Invalid username format" << std::endl;
                    return 1;
                }
                if (!app->isValidPassword(password)) {
                    std::cerr << "Error: Invalid password format" << std::endl;
                    return 1;
                }
                
                // Perform registration
                if (app->registerCustomer(name, phone, username, password)) {
                    std::cout << "Registration successful" << std::endl;
                    return 0;
                } else {
                    std::cerr << "Registration failed" << std::endl;
                    return 1;
                }
            }
            else if (command == "login" && argc == 4) {
                std::string username = argv[2];
                std::string password = argv[3];
                
                int customerId;
                if (app->authenticateCustomer(username, password, customerId)) {
                    std::cout << "Login successful" << std::endl;
                    return 0;
                } else {
                    std::cerr << "Invalid username or password" << std::endl;
                    return 1;
                }
            }
            else if (command == "create-account" && argc == 6) {
                std::string username = argv[2];
                std::string password = argv[3];
                std::string accountType = argv[4];
                double initialBalance = std::stod(argv[5]);
                
                int accountNumber = app->createAccount(username, password, accountType, initialBalance);
                if (accountNumber > 0) {
                    std::cout << "Account created successfully. Account number: " << accountNumber << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to create account. Error code: " << accountNumber << std::endl;
                    return 1;
                }
            }
            else if (command == "deposit" && argc == 5) {
                int accountNumber = std::stoi(argv[2]);
                double amount = std::stod(argv[3]);
                std::string password = argv[4];
                
                if (app->deposit(accountNumber, amount, password)) {
                    std::cout << "Deposit successful" << std::endl;
                    return 0;
                } else {
                    // std::cerr << "Deposit failed" << std::endl;
                    return 1;
                }
            }
            else if (command == "withdraw" && argc == 5) {
                int accountNumber = std::stoi(argv[2]);
                double amount = std::stod(argv[3]);
                std::string password = argv[4];
                
                if (app->withdraw(accountNumber, amount, password)) {
                    std::cout << "Withdrawal successful" << std::endl;
                    return 0;
                } else {
                    // std::cerr << "Withdrawal failed" << std::endl;
                    return 1;
                }
            }
            else if (command == "transfer" && argc == 6) {
                int fromAccount = std::stoi(argv[2]);
                int toAccount = std::stoi(argv[3]);
                double amount = std::stod(argv[4]);
                std::string password = argv[5];
                
                if (app->transfer(fromAccount, toAccount, amount, password)) {
                    std::cout << "Transfer successful" << std::endl;
                    return 0;
                } else {
                    // std::cerr << "Transfer failed" << std::endl;
                    return 1;
                }
            }
            else if (command == "get-accounts" && argc == 3) {
                std::string username = argv[2];
                std::string accounts = app->getAccounts(username);
                std::cout << accounts << std::endl;
                return 0;
            }
            else if (command == "get-account" && argc == 3) {
                int accountNumber = std::stoi(argv[2]);
                std::string account = app->getAccountDetails(accountNumber);
                std::cout << account << std::endl;
                return 0;
            }
            else if (command == "get-transactions" && argc == 3) {
                int accountNumber = std::stoi(argv[2]);
                std::string transactions = app->getTransactions(accountNumber);
                std::cout << transactions << std::endl;
                return 0;
            }
            else if (command == "get-user" && argc == 3) {
                std::string username = argv[2];
                std::string userDetails = app->getUserDetails(username);
                std::cout << userDetails << std::endl;
                return 0;
            }
            else if (command == "update-profile" && argc == 5) {
                std::string username = argv[2];
                std::string name = argv[3];
                std::string phone = argv[4];
                
                if (app->updateProfile(username, name, phone)) {
                    std::cout << "Profile updated successfully" << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to update profile" << std::endl;
                    return 1;
                }
            }
            else if (command == "change-password" && argc == 5) {
                std::string username = argv[2];
                std::string currentPassword = argv[3];
                std::string newPassword = argv[4];
                
                if (app->changePassword(username, currentPassword, newPassword)) {
                    std::cout << "Password changed successfully" << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to change password" << std::endl;
                    return 1;
                }
            }
            else if (command == "close-account" && argc == 4) {
                int accountNumber = std::stoi(argv[2]);
                std::string password = argv[3];
                
                if (app->closeAccount(accountNumber, password)) {
                    std::cout << "Account closed successfully" << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to close account" << std::endl;
                    return 1;
                }
            }
            else {
                std::cerr << "Usage:" << std::endl;
                std::cerr << "  register <name> <phone> <username> <password>" << std::endl;
                std::cerr << "  login <username> <password>" << std::endl;
                std::cerr << "  create-account <username> <password> <type> <balance>" << std::endl;
                std::cerr << "  deposit <account> <amount> <password>" << std::endl;
                std::cerr << "  withdraw <account> <amount> <password>" << std::endl;
                std::cerr << "  transfer <from> <to> <amount> <password>" << std::endl;
                std::cerr << "  get-accounts <username>" << std::endl;
                std::cerr << "  get-account <account>" << std::endl;
                std::cerr << "  get-transactions <account>" << std::endl;
                std::cerr << "  get-user <username>" << std::endl;
                std::cerr << "  update-profile <username> <name> <phone>" << std::endl;
                std::cerr << "  change-password <username> <current-password> <new-password>" << std::endl;
                std::cerr << "  close-account <account> <password>" << std::endl;
                return 1;
            }
            delete BankApp::getInstance("Apna UniBank");
        } else {
            // Interactive mode - original behavior
            BankApp* app = BankApp::getInstance("Apna UniBank");
            app->run();
            delete BankApp::getInstance("Apna UniBank");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 