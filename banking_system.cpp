#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// ==================== CLASSES ====================

class Transaction {
private:
    string type;
    double amount;
    string date;
    string fromAccount;
    string toAccount;
    double balanceAfter;
    
public:
    Transaction(string t, double amt, double balance, string from = "", string to = "") {
        type = t;
        amount = amt;
        balanceAfter = balance;
        fromAccount = from;
        toAccount = to;
        
        time_t now = time(0);
        char* dt = ctime(&now);
        date = string(dt);
        date.pop_back();
    }
    
    void displayTransaction() {
        cout << "| " << setw(12) << type 
             << " | $" << setw(10) << amount 
             << " | " << setw(20) << date.substr(0, 19)
             << " | $" << setw(10) << balanceAfter << " |" << endl;
    }
    
    string getType() { return type; }
    double getAmount() { return amount; }
    string getDate() { return date; }
};

class Account {
private:
    string accountNumber;
    string accountType;
    double balance;
    vector<Transaction> transactions;
    
public:
    Account(string accNo, string accType) {
        accountNumber = accNo;
        accountType = accType;
        balance = 0.0;
    }
    
    string getAccountNumber() { return accountNumber; }
    string getAccountType() { return accountType; }
    double getBalance() { return balance; }
    
    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "[ERROR] Invalid deposit amount!\n";
            return false;
        }
        
        balance += amount;
        Transaction t("Deposit", amount, balance);
        transactions.push_back(t);
        
        cout << "[SUCCESS] Deposited $" << amount << " successfully!\n";
        cout << "Current Balance: $" << balance << endl;
        return true;
    }
    
    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "[ERROR] Invalid withdrawal amount!\n";
            return false;
        }
        
        if (amount > balance) {
            cout << "[ERROR] Insufficient balance! Available: $" << balance << endl;
            return false;
        }
        
        balance -= amount;
        Transaction t("Withdraw", amount, balance);
        transactions.push_back(t);
        
        cout << "[SUCCESS] Withdrawn $" << amount << " successfully!\n";
        cout << "Remaining Balance: $" << balance << endl;
        return true;
    }
    
    bool transfer(Account &toAccountObj, double amount) {
        if (amount <= 0) {
            cout << "[ERROR] Invalid transfer amount!\n";
            return false;
        }
        
        if (amount > balance) {
            cout << "[ERROR] Insufficient balance! Available: $" << balance << endl;
            return false;
        }
        
        // Withdraw from this account
        balance -= amount;
        Transaction t1("Transfer Out", amount, balance, accountNumber, toAccountObj.getAccountNumber());
        transactions.push_back(t1);
        
        // Deposit to other account
        toAccountObj.balance += amount;
        Transaction t2("Transfer In", amount, toAccountObj.balance, accountNumber, toAccountObj.getAccountNumber());
        toAccountObj.transactions.push_back(t2);
        
        cout << "[SUCCESS] Transferred $" << amount << " to Account: " << toAccountObj.getAccountNumber() << endl;
        cout << "Your New Balance: $" << balance << endl;
        return true;
    }
    
    void showTransactionHistory() {
        cout << "\n===================================================\n";
        cout << "  TRANSACTION HISTORY - Account: " << accountNumber << endl;
        cout << "===================================================\n";
        cout << "| " << setw(12) << "Type" 
             << " | " << setw(10) << "Amount" 
             << " | " << setw(20) << "Date" 
             << " | " << setw(10) << "Balance" << " |" << endl;
        cout << "---------------------------------------------------\n";
        
        if (transactions.empty()) {
            cout << "|              No transactions found!              |\n";
        } else {
            for (auto &t : transactions) {
                t.displayTransaction();
            }
        }
        cout << "===================================================\n";
    }
    
    void displayAccountInfo() {
        cout << "\n========== ACCOUNT INFORMATION ==========\n";
        cout << "Account Number: " << accountNumber << endl;
        cout << "Account Type: " << accountType << endl;
        cout << "Current Balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "==========================================\n";
    }
    
    void saveToFile(ofstream &file) {
        file << accountNumber << endl;
        file << accountType << endl;
        file << balance << endl;
        file << transactions.size() << endl;
    }
};

class Customer {
private:
    string customerId;
    string name;
    string phone;
    string email;
    vector<Account> accounts;
    
public:
    Customer(string id, string n, string p, string e) {
        customerId = id;
        name = n;
        phone = p;
        email = e;
    }
    
    string getCustomerId() { return customerId; }
    string getName() { return name; }
    string getPhone() { return phone; }
    string getEmail() { return email; }
    
    void addAccount(Account acc) {
        accounts.push_back(acc);
        cout << "[SUCCESS] Account added successfully for " << name << "!\n";
    }
    
    Account* getAccount(string accNo) {
        for (auto &acc : accounts) {
            if (acc.getAccountNumber() == accNo) {
                return &acc;
            }
        }
        return nullptr;
    }
    
    void displayAllAccounts() {
        cout << "\n========== CUSTOMER ACCOUNTS ==========\n";
        cout << "Customer: " << name << " (ID: " << customerId << ")\n";
        cout << "Phone: " << phone << " | Email: " << email << endl;
        cout << "----------------------------------------\n";
        
        if (accounts.empty()) {
            cout << "No accounts found!\n";
        } else {
            for (auto &acc : accounts) {
                acc.displayAccountInfo();
            }
        }
    }
    
    void displayCustomerInfo() {
        cout << "\n========== CUSTOMER INFORMATION ==========\n";
        cout << "Customer ID: " << customerId << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phone << endl;
        cout << "Email: " << email << endl;
        cout << "Total Accounts: " << accounts.size() << endl;
        cout << "==========================================\n";
    }
    
    void saveToFile(ofstream &file) {
        file << customerId << endl;
        file << name << endl;
        file << phone << endl;
        file << email << endl;
        file << accounts.size() << endl;
    }
};

// ==================== BANKING SYSTEM CLASS ====================

class BankingSystem {
private:
    vector<Customer> customers;
    
public:
    void createCustomer() {
        string id, name, phone, email;
        
        cout << "\n========== CREATE NEW CUSTOMER ==========\n";
        cout << "Enter Customer ID: ";
        cin >> id;
        
        if (getCustomer(id) != nullptr) {
            cout << "[ERROR] Customer ID already exists!\n";
            return;
        }
        
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Phone: ";
        getline(cin, phone);
        cout << "Enter Email: ";
        getline(cin, email);
        
        Customer newCustomer(id, name, phone, email);
        customers.push_back(newCustomer);
        
        cout << "[SUCCESS] Customer created successfully!\n";
        cout << "Customer ID: " << id << endl;
    }
    
    Customer* getCustomer(string id) {
        for (auto &cust : customers) {
            if (cust.getCustomerId() == id) {
                return &cust;
            }
        }
        return nullptr;
    }
    
    void createAccount() {
        string custId, accNo, accType;
        
        cout << "\n========== CREATE ACCOUNT ==========\n";
        cout << "Enter Customer ID: ";
        cin >> custId;
        
        Customer* cust = getCustomer(custId);
        if (cust == nullptr) {
            cout << "[ERROR] Customer not found!\n";
            return;
        }
        
        cout << "Enter Account Number: ";
        cin >> accNo;
        cout << "Enter Account Type (Savings/Current): ";
        cin >> accType;
        
        if (cust->getAccount(accNo) != nullptr) {
            cout << "[ERROR] Account number already exists!\n";
            return;
        }
        
        Account newAccount(accNo, accType);
        cust->addAccount(newAccount);
    }
    
    void deposit() {
        string accNo;
        double amount;
        
        cout << "\n========== DEPOSIT MONEY ==========\n";
        cout << "Enter Account Number: ";
        cin >> accNo;
        
        Customer* cust = findCustomerByAccount(accNo);
        if (cust == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        Account* acc = cust->getAccount(accNo);
        if (acc == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        cout << "Enter Deposit Amount: $";
        cin >> amount;
        
        acc->deposit(amount);
    }
    
    void withdraw() {
        string accNo;
        double amount;
        
        cout << "\n========== WITHDRAW MONEY ==========\n";
        cout << "Enter Account Number: ";
        cin >> accNo;
        
        Customer* cust = findCustomerByAccount(accNo);
        if (cust == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        Account* acc = cust->getAccount(accNo);
        if (acc == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        cout << "Enter Withdrawal Amount: $";
        cin >> amount;
        
        acc->withdraw(amount);
    }
    
    // FIXED TRANSFER FUNCTION
    void transfer() {
        string fromAccNo, toAccNo;
        double amount;
        
        cout << "\n========== TRANSFER MONEY ==========\n";
        cout << "Enter Source Account Number: ";
        cin >> fromAccNo;
        cout << "Enter Destination Account Number: ";
        cin >> toAccNo;
        
        if (fromAccNo == toAccNo) {
            cout << "[ERROR] Cannot transfer to same account!\n";
            return;
        }
        
        Account* fromAcc = nullptr;
        Account* toAcc = nullptr;
        
        // Find both accounts
        for (auto &cust : customers) {
            Account* acc = cust.getAccount(fromAccNo);
            if (acc != nullptr) {
                fromAcc = acc;
            }
            
            Account* acc2 = cust.getAccount(toAccNo);
            if (acc2 != nullptr) {
                toAcc = acc2;
            }
        }
        
        if (fromAcc == nullptr) {
            cout << "[ERROR] Source account not found!\n";
            return;
        }
        
        if (toAcc == nullptr) {
            cout << "[ERROR] Destination account not found!\n";
            return;
        }
        
        cout << "Enter Transfer Amount: $";
        cin >> amount;
        
        fromAcc->transfer(*toAcc, amount);
    }
    
    Customer* findCustomerByAccount(string accNo) {
        for (auto &cust : customers) {
            if (cust.getAccount(accNo) != nullptr) {
                return &cust;
            }
        }
        return nullptr;
    }
    
    void showTransactionHistory() {
        string accNo;
        
        cout << "\n========== TRANSACTION HISTORY ==========\n";
        cout << "Enter Account Number: ";
        cin >> accNo;
        
        Customer* cust = findCustomerByAccount(accNo);
        if (cust == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        Account* acc = cust->getAccount(accNo);
        if (acc == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        acc->showTransactionHistory();
    }
    
    void displayAccountInfo() {
        string accNo;
        
        cout << "\n========== ACCOUNT INFORMATION ==========\n";
        cout << "Enter Account Number: ";
        cin >> accNo;
        
        Customer* cust = findCustomerByAccount(accNo);
        if (cust == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        Account* acc = cust->getAccount(accNo);
        if (acc == nullptr) {
            cout << "[ERROR] Account not found!\n";
            return;
        }
        
        acc->displayAccountInfo();
    }
    
    void displayCustomerInfo() {
        string custId;
        
        cout << "\n========== CUSTOMER INFORMATION ==========\n";
        cout << "Enter Customer ID: ";
        cin >> custId;
        
        Customer* cust = getCustomer(custId);
        if (cust == nullptr) {
            cout << "[ERROR] Customer not found!\n";
            return;
        }
        
        cust->displayCustomerInfo();
    }
    
    void displayAllCustomers() {
        cout << "\n========== ALL CUSTOMERS ==========\n";
        if (customers.empty()) {
            cout << "No customers found!\n";
            return;
        }
        
        for (auto &cust : customers) {
            cout << "ID: " << cust.getCustomerId() << " | Name: " << cust.getName() 
                 << " | Phone: " << cust.getPhone() << endl;
        }
        cout << "Total Customers: " << customers.size() << endl;
    }
};

// ==================== MAIN FUNCTION ====================

void showMainMenu() {
    cout << "\n============================================\n";
    cout << "   CODE ALPHA INTERNSHIP - TASK 4          \n";
    cout << "         BANKING SYSTEM (C++)               \n";
    cout << "============================================\n";
    cout << "\n1. Create Customer\n";
    cout << "2. Create Account\n";
    cout << "3. Deposit Money\n";
    cout << "4. Withdraw Money\n";
    cout << "5. Transfer Money\n";
    cout << "6. View Transaction History\n";
    cout << "7. View Account Information\n";
    cout << "8. View Customer Information\n";
    cout << "9. View All Customers (Admin)\n";
    cout << "10. Exit\n";
    cout << "\nEnter your choice: ";
}

int main() {
    BankingSystem bank;
    int choice;
    
    cout << "\n============================================\n";
    cout << "   WELCOME TO CODE ALPHA BANKING SYSTEM     \n";
    cout << "============================================\n";
    
    do {
        showMainMenu();
        cin >> choice;
        
        switch(choice) {
            case 1:
                bank.createCustomer();
                break;
            case 2:
                bank.createAccount();
                break;
            case 3:
                bank.deposit();
                break;
            case 4:
                bank.withdraw();
                break;
            case 5:
                bank.transfer();
                break;
            case 6:
                bank.showTransactionHistory();
                break;
            case 7:
                bank.displayAccountInfo();
                break;
            case 8:
                bank.displayCustomerInfo();
                break;
            case 9:
                bank.displayAllCustomers();
                break;
            case 10:
                cout << "\n[INFO] Thank you for using Code Alpha Banking System!\n";
                cout << "[INFO] Goodbye!\n";
                break;
            default:
                cout << "\n[ERROR] Invalid choice! Please enter 1-10.\n";
        }
    } while(choice != 10);
    
    return 0;
}
