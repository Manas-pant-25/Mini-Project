#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
using namespace std;

// Clears bad input
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ======================== BASE CLASS ========================
class Transaction {
protected:
    string type;
    double amount;
public:
    Transaction(string t = "Unknown", double a = 0.0) : type(t), amount(a) {}

    virtual void display() const {
        cout << type << " : " << amount << endl;
    }

    string getType() const { return type; }
    double getAmount() const { return amount; }
};

// ======================== DERIVED CLASSES ========================
class Income : public Transaction {
public:
    Income(double amt) : Transaction("Income", amt) {}
};

class Expense : public Transaction {
public:
    Expense(double amt) : Transaction("Expense", amt) {}
};

// ======================== FINANCE MANAGER ========================
class FinanceManager {
private:
    vector<Transaction*> records;
    string filename;

public:
    FinanceManager(string file = "transactions.csv") : filename(file) {}

    bool isEmpty() const {
        return records.empty();
    }

    void addTransaction(Transaction* t) {
        if (t->getAmount() <= 0)
            throw invalid_argument("Amount must be greater than 0");
        records.push_back(t);
        saveToFile();
    }

    void removeTransaction(int index) {
        if (index < 0 || index >= records.size())
            throw out_of_range("Invalid index");

        delete records[index];
        records.erase(records.begin() + index);
        saveToFile();
    }

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

    void saveToFile() {
        ofstream file(filename);

        for (vector<Transaction*>::iterator it = records.begin(); it != records.end(); ++it) {
            file << (*it)->getType() << "," << (*it)->getAmount() << "\n";
        }

        file.close();
    }

    void loadFromFile() {
        records.clear();
        ifstream file(filename);
        if (!file.is_open()) return;

        string type;
        double amt;

        while (getline(file, type, ',') && file >> amt) {
            file.ignore();
            if (type == "Income")
                records.push_back(new Income(amt));
            else
                records.push_back(new Expense(amt));
        }
        file.close();
    }

    ~FinanceManager() {
        for (vector<Transaction*>::iterator it = records.begin(); it != records.end(); ++it) {
            delete *it;
        }
    }
};

// ======================== MAIN FUNCTION ========================
int main() {
    FinanceManager fm;
    fm.loadFromFile();

    int choice, n;
    double amount;

    while (true) {
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
                break;
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
