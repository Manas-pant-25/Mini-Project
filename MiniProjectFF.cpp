#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
using namespace std;

// Clears invalid input from buffer
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ======================== BASE CLASS ========================
class Transaction {
protected:
    string type;      // Stores whether transaction is Income or Expense
    double amount;    // Stores the amount of the transaction

public:
    // Simple constructor
    Transaction(string t = "Unknown", double a = 0.0) {
        type = t;
        amount = a;
    }

    // Displays transaction details
    virtual void display() const {
        cout << type << " : " << amount << endl;
    }

    // Getter functions to safely access private/protected data
    string getType() const {
        return type;
    }

    double getAmount() const {
        return amount;
    }
};

// ======================== DERIVED CLASSES ========================
class Income : public Transaction {
public:
    // Constructor for Income
    Income(double amt) {
        type = "Income";
        amount = amt;
    }
};

class Expense : public Transaction {
public:
    // Constructor for Expense
    Expense(double amt) {
        type = "Expense";
        amount = amt;
    }
};

// ======================== FINANCE MANAGER ========================
class FinanceManager {
private:
    vector<Transaction*> records; // Stores list of transactions
    string filename;              // File used to save/load transactions

public:
    // Constructor to set filename
    FinanceManager(string file = "transactions.csv") {
        filename = file;
    }

    // Check if records list is empty
    bool isEmpty() const {
        return records.empty();
    }

    // Add a transaction to the list
    void addTransaction(Transaction* t) {
        if (t->getAmount() <= 0)
            throw invalid_argument("Amount must be greater than 0");

        records.push_back(t);
        saveToFile(); // Save immediately
    }

    // Remove a transaction at given index
    void removeTransaction(int index) {
        if (index < 0 || index >= records.size())
            throw out_of_range("Invalid index");

        delete records[index];                // Free memory
        records.erase(records.begin() + index); // Remove from list
        saveToFile();
    }

    // Show all stored transactions
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

    // Save transactions to file
    void saveToFile() {
        ofstream file(filename);

        for (int i = 0; i < records.size(); i++) {
            file << records[i]->getType() << "," 
                 << records[i]->getAmount() << "\n";
        }

        file.close();
    }

    // Load transactions from file
    void loadFromFile() {
        records.clear();
        ifstream file(filename);
        if (!file.is_open()) return;

        string type;
        double amt;

        while (getline(file, type, ',') && file >> amt) {
            file.ignore(); // Skip newline

            // Create correct object based on type
            if (type == "Income")
                records.push_back(new Income(amt));
            else
                records.push_back(new Expense(amt));
        }
        file.close();
    }

    // Destructor to clean memory
    ~FinanceManager() {
        for (int i = 0; i < records.size(); i++) {
            delete records[i];
        }
    }
};

// ======================== MAIN FUNCTION ========================
int main() {
    FinanceManager fm;  // Manager object
    fm.loadFromFile();  // Load saved transactions

    int choice, n;
    double amount;

    while (true) {
        // Menu
        cout << "\n1. Add Transactions"
             << "\n2. Display Transactions"
             << "\n3. Remove Transaction"
             << "\n4. Exit"
             << "\nEnter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid input! Please enter a number.\n";
            clearInput();
            continue;
        }

        try {
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

                    // Create appropriate object
                    if (t == 1)
                        fm.addTransaction(new Income(amount));
                    else if (t == 2)
                        fm.addTransaction(new Expense(amount));
                    else
                        throw invalid_argument("Invalid type!");
                }
            }
            else if (choice == 2) {
                fm.displayAll();
            }
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
            else if (choice == 4) {
                break; // Exit loop
            }
            else {
                cout << "Invalid choice!\n";
            }
        }
        catch (exception &e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
