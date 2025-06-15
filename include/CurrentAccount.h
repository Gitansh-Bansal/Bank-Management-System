#pragma once

#include "Account.h"

class CurrentAccount : public Account {
private:
    double maintenanceFee;
    static constexpr double DEFAULT_MAINTENANCE_FEE = 10.0; // $10 monthly fee

public:
    CurrentAccount(int accNo, double initialBalance, Customer* owner,
                  double maintenanceFee = DEFAULT_MAINTENANCE_FEE);
    
    bool deposit(double amount) override;
    bool withdraw(double amount) override;
    void applyMonthlyUpdate() override;
    double calculateInterest() const override; // Returns 0 as current accounts don't earn interest
    
    double getMaintenanceFee() const { return maintenanceFee; }
    void setMaintenanceFee(double newFee) { maintenanceFee = newFee; }
}; 