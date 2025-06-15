#pragma once

#include "Account.h"

class SavingsAccount : public Account {
protected:
    static constexpr double DEFAULT_INTEREST_RATE = 0.05; // 5% annual interest
    double interestRate;

public:
    static double getDefaultInterestRate() { return DEFAULT_INTEREST_RATE; }
    
    SavingsAccount(int accNo, double initialBalance, Customer* owner,
                  double interestRate = DEFAULT_INTEREST_RATE);
    
    bool deposit(double amount) override;
    bool withdraw(double amount) override;
    void applyMonthlyUpdate() override;
    double calculateInterest() const override;
    
    double getInterestRate() const { return interestRate; }
    void setInterestRate(double newRate) { interestRate = newRate; }
}; 