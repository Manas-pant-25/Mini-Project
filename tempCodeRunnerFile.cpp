#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

// ======================== BASE CLASS ========================
class Transaction {
protected:
    string type;
    double amount;
    string note;
public:
    Transaction(string t = "Unknown", double a = 0.0, string n = "")
        : type(t), amount(a), note(n) {}

    virtual void display() const {
        cout << type << " : " << amount << "  | Note: " << note << endl;
    }

    string getType() const { return type; }
    double getAmount() const { return amount; }
    string getNote() const { return note; }
};

// ======================== DERIVED CLASSES ========================
class Income : public Transaction {
public:
    Income(double amt, string nt) : Transaction("Income", amt, nt) {}
};

class Expense : public Transaction {
public:
    Expense(double amt, string nt) : Transaction("Expense", amt, nt) {}
};


// ======================== FINANCE MANAGER ========================
class FinanceManager {
private:
    vector<Transaction*> records;
    string filename;

public:
    FinanceManager(string file = "finance.csv") : filename(file) {} // UPDATED NAME

    void addTransaction(Transaction* t) {
        if (t->getAmount() <= 0)
            throw invalid_argument("Amount must be greater than 0");
        records.push_back(t);
        saveToFile();
    }

    void removeTransaction(int index) {
        if (index < 0 || index >= (int)records.size())
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
        for (int i = 0; i < (int)records.size(); i++) {
            cout << i << ". ";
            records[i]->display();
        }
    }

    void saveToFile() {
        ofstream file(filename);
        for (auto t : records) {
            file << t->getType() << ","
                 << t->getAmount() << ","
                 << t->getNote() << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        records.clear();
        ifstream file(filename);
        if (!file.is_open()) return;

        string type, note, amtStr;
        double amt;

        while (getline(file, type, ',') &&
               getline(file, amtStr, ',') &&
               getline(file, note))
        {
            amt = stod(amtStr);

            if (type == "Income")
                records.push_back(new Income(amt, note));
            else
                records.push_back(new Expense(amt, note));
        }
        file.close();
    }

    ~FinanceManager() {
        for (auto t : records) delete t;
    }
};


// ======================== MAIN ========================
int main() {
    FinanceManager fm("finance.csv"); // UPDATED NAME
    fm.loadFromFile();

    int choice, n;
    double amount;
    string note;

    while (true) {
        cout << "\n1. Add Transactions"
             << "\n2. Display Transactions"
             << "\n3. Remove Transaction"
             << "\n4. Exit"
             << "\nEnter choice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                cout << "How many entries do you want? ";
                cin >> n;

                for (int i = 0; i < n; i++) {
                    cout << "\n1. Income\n2. Expense\nEnter type: ";
                    int t;
                    cin >> t;

                    cout << "Enter amount: ";
                    cin >> amount;

                    cin.ignore();
                    cout << "Enter note: ";
                    getline(cin, note);

                    if (t == 1)
                        fm.addTransaction(new Income(amount, note));
                    else if (t == 2)
                        fm.addTransaction(new Expense(amount, note));
                    else
                        throw invalid_argument("Invalid type!");
                }
            }

            else if (choice == 2) {
                fm.displayAll();
            }

            else if (choice == 3) {
                fm.displayAll();
                cout << "Enter index to delete: ";
                cin >> n;
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

