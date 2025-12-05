#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <ctime>
#include <cstring>
using namespace std;

// Safely clears wrong user input
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ======================== BASE CLASS ========================
// Parent class for all transactions (Income / Expense)
class Transaction {
protected:
    string type;        // Income / Expense
    double amount;      // Amount of transaction
    string dateTime;    // Timestamp of transaction
    string note;        // Optional note

public:
    // Constructor
    Transaction(string t = "Unknown", double a = 0.0, string dt = "N/A", string nt = "No note") {
        type = t;
        amount = a;
        dateTime = dt;
        note = nt;
    }

    // Displays transaction details
    virtual void display() const {
        cout << type << " : " << amount 
             << "  |  " << dateTime
             << "  |  Note: " << note << endl;
    }

    // Getters
    string getType() const { 
        return type; 
    }
    double getAmount() const { 
        return amount; 
    }
    string getDateTime() const { 
        return dateTime; 
    }
    string getNote() const { 
        return note; 
    }
};

// ======================== DERIVED CLASSES ========================
// Income inherits from Transaction
class Income : public Transaction {
public:
    Income(double amt, string dt, string nt) {
        type = "Income";
        amount = amt;
        dateTime = dt;
        note = nt;
    }
};

// Expense inherits from Transaction
class Expense : public Transaction {
public:
    Expense(double amt, string dt, string nt) {
        type = "Expense";
        amount = amt;
        dateTime = dt;
        note = nt;
    }
};

// ======================== FINANCE MANAGER ========================
// Handles all transactions + file operations
class FinanceManager {
private:
    vector<Transaction*> records;   // Stores pointers to all transactions
    string filename;                // CSV file name

public:
    FinanceManager(string file = "transactions.csv") {
        filename = file;
    }

    bool isEmpty() const {
        return records.empty();
    }

    // Returns current system date and time as string
    string getCurrentDateTime() {
        time_t now = time(0);
        char dt[30];
        strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(dt);
    }

    // Calculates total balance = income - expense
    double getBalance() {
        double income = 0, expense = 0;
        for (auto t : records) {
            if (t->getType() == "Income")
                income += t->getAmount();
            else
                expense += t->getAmount();
        }
        return income - expense;
    }

    // Adds a transaction to vector + saves to file
    void addTransaction(Transaction* t) {
        if (t->getAmount() <= 0)
            throw invalid_argument("Amount must be greater than 0");

        records.push_back(t);
        saveToFile();
    }

    // Deletes selected transaction
    void removeTransaction(int index) {
        if (index < 0 || index >= records.size())
            throw out_of_range("Invalid index");

        delete records[index];
        records.erase(records.begin() + index);
        saveToFile();
    }

    // Displays all transactions in list form
    void displayAll() const {
        if (records.empty()) {
            cout << "\nNo transactions found.\n";
            return;
        }

        cout << "\n--- Transaction List ---\n";
        for (int i = 0; i < records.size(); i++) {
            cout << i << ". ";
            records[i]->display();
        }
    }

    // Writes all data to CSV file
    void saveToFile() {
        ofstream file(filename);

        for (int i = 0; i < records.size(); i++) {
            file << records[i]->getType() << ","
                 << records[i]->getAmount() << ","
                 << records[i]->getDateTime() << ","
                 << records[i]->getNote() << "\n";
        }

        file.close();
    }

    // Loads saved data from CSV file when program starts
    void loadFromFile() {
        records.clear();
        ifstream file(filename);
        if (!file.is_open()) return;

        string type, amtStr, dt, nt;
        double amt;

        // CSV reading using commas
        while (getline(file, type, ',') &&
               getline(file, amtStr, ',') &&
               getline(file, dt, ',') &&
               getline(file, nt))
        {
            amt = stod(amtStr);

            if (type == "Income")
                records.push_back(new Income(amt, dt, nt));
            else
                records.push_back(new Expense(amt, dt, nt));
        }

        file.close();
    }

    // Destructor — to free dynamically allocated memory
    ~FinanceManager() {
        for (int i = 0; i < records.size(); i++)
            delete records[i];
    }
};

// ======================== MAIN FUNCTION ========================
int main() {
    FinanceManager fm;
    fm.loadFromFile();     // Load old data from file

    int choice, n;
    double amount;

    while (true) {

        // Menu
        cout << "\n1. Add Transactions"
             << "\n2. Display Transactions"
             << "\n3. Remove Transaction"
             << "\n4. Check Balance"
             << "\n5. Exit"
             << "\nEnter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid input! Please enter a number.\n";
            clearInput();
            continue;
        }

        try {
            // ===== OPTION 1: ADD TRANSACTIONS =====
            if (choice == 1) {

                cout << "How many entries do you want? ";
                cin >> n;

                if (cin.fail()) {
                    cout << "Invalid number!\n";
                    clearInput();
                    continue;
                }

                for (int i = 0; i < n; i++) {

                    cout << "\n1. Income\n2. Expense\nEnter type: ";
                    int t;
                    cin >> t;

                    if (cin.fail()) {
                        cout << "Invalid input!\n";
                        clearInput();
                        continue;
                    }

                    cout << "Enter amount: ";
                    cin >> amount;

                    if (cin.fail()) {
                        cout << "Invalid amount!\n";
                        clearInput();
                        continue;
                    }

                    clearInput();
                    cout << "Enter note: ";
                    string note;
                    getline(cin, note);
                    if (note == "") note = "No note";

                    // Auto timestamp
                    string timeNow = fm.getCurrentDateTime();

                    // Create object dynamically
                    if (t == 1)
                        fm.addTransaction(new Income(amount, timeNow, note));
                    else if (t == 2)
                        fm.addTransaction(new Expense(amount, timeNow, note));
                    else
                        throw invalid_argument("Invalid type!");
                }
            }

            // ===== OPTION 2: DISPLAY =====
            else if (choice == 2) {
                fm.displayAll();
            }

            // ===== OPTION 3: DELETE =====
            else if (choice == 3) {
                if (fm.isEmpty()) {
                    cout << "\nNo transactions to remove!\n";
                    continue;
                }

                fm.displayAll();
                cout << "Enter index to delete: ";
                cin >> n;

                if (cin.fail()) {
                    cout << "Invalid input!\n";
                    clearInput();
                    continue;
                }

                fm.removeTransaction(n);
                cout << "Removed successfully!\n";
            }

            // ===== OPTION 4: BALANCE =====
            else if (choice == 4) {
                cout << "\nCurrent Balance = " << fm.getBalance() << endl;
            }

            // ===== OPTION 5: EXIT =====
            else if (choice == 5) {
                break;
            }

            else {
                cout << "Invalid choice!\n";
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
