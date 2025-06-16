#include "../include/SavingsAccount.h"
#include <stdexcept>

SavingsAccount::SavingsAccount(int accNo, double initialBalance, Customer* owner, double interestRate, AccountType type)
    : Account(accNo, initialBalance, owner, type), interestRate(interestRate) {
    if (interestRate < 0) {
        throw std::invalid_argument("Interest rate cannot be negative");
    }
}

bool SavingsAccount::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    try {
        updateBalance(getBalance() + amount);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool SavingsAccount::withdraw(double amount) {
    if (amount <= 0 || amount > getBalance()) {
        return false;
    }
    try {
        updateBalance(getBalance() - amount);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void SavingsAccount::applyMonthlyUpdate() {
    double interest = calculateInterest();
    updateBalance(getBalance() + interest);
}

double SavingsAccount::calculateInterest() const {
    return getBalance() * (interestRate / 12.0); // Monthly interest
} 

