# Sampatti Bank - Bank Management System

A comprehensive C++ project demonstrating all fundamental Object-Oriented Programming (OOP) principles through a fully functional bank management system.

---

## Features

- **Customer Registration & Login**
  - Secure registration with username, password, and phone validation.
  - Login with authentication and password protection.

- **Account Management**
  - Multiple account types: Savings, Current, Auditable Savings.
  - Create, select, and list accounts for each customer.
  - Password-protected accounts.

- **Transactions**
  - Deposit, Withdraw, and Transfer funds between accounts.
  - Transaction history and account statements.
  - Auditable accounts with detailed logs.

- **Account Closure**
  - Close accounts with options to withdraw or transfer remaining balance.
  - Final statement generation.

- **Persistence**
  - All data (customers, accounts, transactions) is saved and loaded from files.
  - Data directory structure for easy management.

---

## OOP Principles Demonstrated

- **Encapsulation:**  
  All data members are private/protected. Access is provided via public methods. Validation is enforced in constructors and setters.

- **Abstraction:**  
  Abstract base classes (`Account`, `ITransaction`) define interfaces for accounts and transactions. Concrete classes implement specific behaviors.

- **Inheritance:**  
  - `SavingsAccount`, `CurrentAccount`, and `AuditableSavingsAccount` inherit from `Account`.
  - `Deposit`, `Withdrawal`, and `Transfer` inherit from `ITransaction`.

- **Polymorphism:**  
  - Transactions are handled via pointers/references to `ITransaction`.
  - Account operations use virtual and pure virtual methods for deposit, withdraw, and interest calculation.

- **Virtual Methods & Pure Virtual Methods:**  
  - The `Account` and `ITransaction` classes declare virtual and pure virtual methods, enabling runtime polymorphism and enforcing interface contracts for derived classes.

- **Composition:**  
  - `Customer` contains a list of `Account` objects.
  - `Account` can contain a list of `ITransaction` objects (transaction history).

- **Singleton Pattern:**  
  - `Database` and `BankApp` are implemented as singletons to ensure a single point of access.


---

## File Structure

```
include/         # Header files (class definitions)
src/             # Source files (class implementations)
data/            # Data persistence (created at runtime)
Makefile         # Build instructions
README.md        # Project documentation
```

---

## How to Build & Run

1. **Build the project:**
   ```sh
   make
   ```

2. **Run the application:**
   ```sh
   make run
   ```

---

## Main Classes & Responsibilities

- **BankApp:**  
  Main application controller. Handles menus, user input, and high-level flow.

- **Database:**  
  Singleton. Manages all customers, accounts, authentication, and file persistence.

- **Customer:**  
  Represents a bank customer. Manages their accounts.

- **Account (Abstract):**  
  Base class for all account types. Defines interface for deposit, withdraw, and interest.

- **SavingsAccount, CurrentAccount, AuditableSavingsAccount:**  
  Concrete account types with specific rules and features.

- **ITransaction (Interface):**  
  Abstract base for all transactions (deposit, withdrawal, transfer).

- **Deposit, Withdrawal, Transfer:**  
  Concrete transaction types implementing `ITransaction`.

---

## Utilities & Validation

- Input validation for all user data (names, phone numbers, usernames, passwords).
- Password protection for both customer and account access.
- Safe input handling to prevent invalid or malicious entries.

---

## Contributors

- Gitansh Bansal
- Shaurya Anant

---
