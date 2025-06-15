#include "../include/CurrentAccount.h"
#include <stdexcept>

CurrentAccount::CurrentAccount(int accNo, double initialBalance, Customer* owner, double maintenanceFee)
    : Account(accNo, initialBalance, owner, AccountType::CURRENT), maintenanceFee(maintenanceFee) {
    if (maintenanceFee < 0) {
        throw std::invalid_argument("Maintenance fee cannot be negative");
    }
}

bool CurrentAccount::deposit(double amount) {
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

bool CurrentAccount::withdraw(double amount) {
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

void CurrentAccount::applyMonthlyUpdate() {
    if (getBalance() < maintenanceFee) {
        throw std::runtime_error("Insufficient balance for maintenance fee");
    }
    updateBalance(getBalance() - maintenanceFee);
}

double CurrentAccount::calculateInterest() const {
    return 0.0; // Current accounts don't earn interest
} 