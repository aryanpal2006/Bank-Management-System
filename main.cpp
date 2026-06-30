#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Account {
    int id;
    string ownerName;
    double balance;
    bool active;
    stack<string> transactionHistory;
};

struct User {
    string username;
    string password;
    string role;
    int accountId;
    bool active;
};

struct ServiceRequest {
    int id;
    int accountId;
    string type;
    string description;
    bool resolved;
};

struct BSTNode {
    Account* account;
    BSTNode* left;
    BSTNode* right;

    explicit BSTNode(Account* acc) : account(acc), left(nullptr), right(nullptr) {}
};

class AccountBST {
public:
    AccountBST() : root(nullptr) {}

    void insert(Account* account) {
        root = insert(root, account);
    }

    Account* find(int accountId) const {
        return find(root, accountId);
    }

    void printAll() const {
        printInOrder(root);
    }

private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, Account* account) {
        if (!node) return new BSTNode(account);
        if (account->id < node->account->id) {
            node->left = insert(node->left, account);
        } else if (account->id > node->account->id) {
            node->right = insert(node->right, account);
        }
        return node;
    }

    Account* find(BSTNode* node, int accountId) const {
        if (!node) return nullptr;
        if (accountId == node->account->id) return node->account;
        if (accountId < node->account->id) return find(node->left, accountId);
        return find(node->right, accountId);
    }

    void printInOrder(BSTNode* node) const {
        if (!node) return;
        printInOrder(node->left);
        cout << "  [Account " << node->account->id << "] " << node->account->ownerName
             << " | Balance: $" << fixed << setprecision(2) << node->account->balance
             << " | Status: " << (node->account->active ? "Active" : "Inactive") << endl;
        printInOrder(node->right);
    }
};

class BankSystem {
public:
    BankSystem() {
        seedData();
    }

    void run() {
        while (true) {
            cout << "\n===== Bank Management System =====" << endl;
            cout << "1. Register Customer" << endl;
            cout << "2. Customer Login" << endl;
            cout << "3. Employee Login" << endl;
            cout << "4. Exit" << endl;
            cout << "Choose an option: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                registerCustomer();
            } else if (choice == 2) {
                customerDashboard();
            } else if (choice == 3) {
                employeeDashboard();
            } else if (choice == 4) {
                cout << "Thank you for using the Bank Management System." << endl;
                break;
            } else {
                cout << "Invalid option. Please try again." << endl;
            }
        }
    }

private:
    vector<unique_ptr<Account>> accounts;
    vector<User> users;
    queue<ServiceRequest> serviceRequests;
    AccountBST accountBST;
    int nextAccountId = 1001;
    int nextRequestId = 1;

    void seedData() {
        createAccount("Alice Johnson", 2500.00, "alice", "pass123", "customer");
        createAccount("Bob Carter", 1800.00, "bob", "pass123", "customer");
        createAccount("Clara Diaz", 4200.00, "clara", "pass123", "customer");
        users.push_back({"admin", "admin123", "employee", 0, true});
    }

    Account* createAccount(const string& ownerName, double initialBalance, const string& username, const string& password, const string& role) {
        auto account = make_unique<Account>();
        account->id = nextAccountId++;
        account->ownerName = ownerName;
        account->balance = initialBalance;
        account->active = true;
        account->transactionHistory.push("Account created with initial balance $" + formatMoney(initialBalance));

        Account* raw = account.get();
        accounts.push_back(move(account));
        accountBST.insert(raw);

        if (!username.empty()) {
            users.push_back({username, password, role, raw->id, true});
        }
        return raw;
    }

    string formatMoney(double value) const {
        ostringstream oss;
        oss << fixed << setprecision(2) << value;
        return oss.str();
    }

    User* authenticateUser(const string& username, const string& password) {
        for (auto& user : users) {
            if (user.username == username && user.password == password && user.active) {
                return &user;
            }
        }
        return nullptr;
    }

    Account* findAccount(int accountId) const {
        return accountBST.find(accountId);
    }

    void addTransaction(Account* account, const string& log) {
        if (!account) return;
        account->transactionHistory.push(log);
    }

    void showTransactionHistory(Account* account) const {
        if (!account) {
            cout << "Account not found." << endl;
            return;
        }
        if (account->transactionHistory.empty()) {
            cout << "No transactions recorded yet." << endl;
            return;
        }

        stack<string> temp = account->transactionHistory;
        vector<string> entries;
        while (!temp.empty()) {
            entries.push_back(temp.top());
            temp.pop();
        }
        reverse(entries.begin(), entries.end());

        cout << "Transaction History" << endl;
        for (const auto& entry : entries) {
            cout << "- " << entry << endl;
        }
    }

    void registerCustomer() {
        cout << "\nRegister New Customer" << endl;
        string username, password, fullName;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        cout << "Enter full name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, fullName);

        for (const auto& user : users) {
            if (user.username == username) {
                cout << "Username already exists." << endl;
                return;
            }
        }

        Account* newAccount = createAccount(fullName, 0.0, username, password, "customer");
        cout << "Account created successfully with ID " << newAccount->id << endl;
    }

    void customerDashboard() {
        string username, password;
        cout << "\nCustomer Login" << endl;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User* user = authenticateUser(username, password);
        if (!user || user->role != "customer") {
            cout << "Invalid credentials or access denied." << endl;
            return;
        }

        while (true) {
            cout << "\nCustomer Menu" << endl;
            cout << "1. View Balance" << endl;
            cout << "2. Deposit Funds" << endl;
            cout << "3. Withdraw Funds" << endl;
            cout << "4. Transfer Funds" << endl;
            cout << "5. View Transaction History" << endl;
            cout << "6. Submit Service Request" << endl;
            cout << "7. View Request Status" << endl;
            cout << "8. Logout" << endl;
            cout << "Choose an option: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Account* account = findAccount(user->accountId);
            if (!account) {
                cout << "Account missing." << endl;
                break;
            }

            if (choice == 1) {
                cout << "Current balance: $" << fixed << setprecision(2) << account->balance << endl;
            } else if (choice == 2) {
                double amount;
                cout << "Enter deposit amount: ";
                cin >> amount;
                if (amount > 0) {
                    account->balance += amount;
                    addTransaction(account, "Deposit of $" + formatMoney(amount));
                    cout << "Deposit successful." << endl;
                } else {
                    cout << "Amount must be positive." << endl;
                }
            } else if (choice == 3) {
                double amount;
                cout << "Enter withdrawal amount: ";
                cin >> amount;
                if (amount > 0 && amount <= account->balance) {
                    account->balance -= amount;
                    addTransaction(account, "Withdrawal of $" + formatMoney(amount));
                    cout << "Withdrawal successful." << endl;
                } else {
                    cout << "Insufficient balance or invalid amount." << endl;
                }
            } else if (choice == 4) {
                int targetId;
                double amount;
                cout << "Enter target account ID: ";
                cin >> targetId;
                cout << "Enter amount: ";
                cin >> amount;

                Account* target = findAccount(targetId);
                if (target && target->id != account->id && amount > 0 && amount <= account->balance) {
                    account->balance -= amount;
                    target->balance += amount;
                    addTransaction(account, "Transferred $" + formatMoney(amount) + " to account " + to_string(target->id));
                    addTransaction(target, "Received $" + formatMoney(amount) + " from account " + to_string(account->id));
                    cout << "Transfer successful." << endl;
                } else {
                    cout << "Transfer failed. Check account ID and balance." << endl;
                }
            } else if (choice == 5) {
                showTransactionHistory(account);
            } else if (choice == 6) {
                string requestType, description;
                cout << "Enter request type: ";
                getline(cin, requestType);
                cout << "Enter description: ";
                getline(cin, description);
                serviceRequests.push({nextRequestId++, user->accountId, requestType, description, false});
                cout << "Service request submitted." << endl;
            } else if (choice == 7) {
                queue<ServiceRequest> temp = serviceRequests;
                bool found = false;
                while (!temp.empty()) {
                    const ServiceRequest& req = temp.front();
                    if (req.accountId == user->accountId) {
                        cout << "Request " << req.id << " | Type: " << req.type << " | Status: "
                             << (req.resolved ? "Resolved" : "Pending") << " | " << req.description << endl;
                        found = true;
                    }
                    temp.pop();
                }
                if (!found) {
                    cout << "No requests found." << endl;
                }
            } else if (choice == 8) {
                break;
            } else {
                cout << "Invalid option." << endl;
            }
        }
    }

    void employeeDashboard() {
        string username, password;
        cout << "\nEmployee Login" << endl;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User* user = authenticateUser(username, password);
        if (!user || user->role != "employee") {
            cout << "Invalid credentials or access denied." << endl;
            return;
        }

        while (true) {
            cout << "\nEmployee Menu" << endl;
            cout << "1. View All Accounts" << endl;
            cout << "2. Create Customer Account" << endl;
            cout << "3. Deposit to Account" << endl;
            cout << "4. Withdraw from Account" << endl;
            cout << "5. Transfer Between Accounts" << endl;
            cout << "6. List Pending Service Requests" << endl;
            cout << "7. Resolve Service Request" << endl;
            cout << "8. View Transaction Summary" << endl;
            cout << "9. Toggle Account Status" << endl;
            cout << "10. Logout" << endl;
            cout << "Choose an option: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                accountBST.printAll();
            } else if (choice == 2) {
                string name;
                cout << "Enter customer full name: ";
                getline(cin, name);
                double openingBalance;
                cout << "Enter opening balance: ";
                cin >> openingBalance;
                Account* acc = createAccount(name, openingBalance, "", "", "customer");
                cout << "Customer account created with ID " << acc->id << endl;
            } else if (choice == 3) {
                int accountId;
                double amount;
                cout << "Enter account ID: ";
                cin >> accountId;
                cout << "Enter amount: ";
                cin >> amount;
                Account* account = findAccount(accountId);
                if (account && account->active) {
                    account->balance += amount;
                    addTransaction(account, "Employee deposit of $" + formatMoney(amount));
                    cout << "Deposit successful." << endl;
                } else {
                    cout << "Account not found or inactive." << endl;
                }
            } else if (choice == 4) {
                int accountId;
                double amount;
                cout << "Enter account ID: ";
                cin >> accountId;
                cout << "Enter amount: ";
                cin >> amount;
                Account* account = findAccount(accountId);
                if (account && account->active && amount > 0 && amount <= account->balance) {
                    account->balance -= amount;
                    addTransaction(account, "Employee withdrawal of $" + formatMoney(amount));
                    cout << "Withdrawal successful." << endl;
                } else {
                    cout << "Account not found, inactive, or insufficient balance." << endl;
                }
            } else if (choice == 5) {
                int fromId, toId;
                double amount;
                cout << "Enter source account ID: ";
                cin >> fromId;
                cout << "Enter destination account ID: ";
                cin >> toId;
                cout << "Enter amount: ";
                cin >> amount;
                Account* from = findAccount(fromId);
                Account* to = findAccount(toId);
                if (from && to && from->active && to->active && from->id != to->id && amount > 0 && amount <= from->balance) {
                    from->balance -= amount;
                    to->balance += amount;
                    addTransaction(from, "Employee transfer of $" + formatMoney(amount) + " to account " + to_string(to->id));
                    addTransaction(to, "Employee receipt of $" + formatMoney(amount) + " from account " + to_string(from->id));
                    cout << "Transfer completed." << endl;
                } else {
                    cout << "Transfer failed. Check account IDs and balance." << endl;
                }
            } else if (choice == 6) {
                queue<ServiceRequest> temp = serviceRequests;
                bool pending = false;
                while (!temp.empty()) {
                    const ServiceRequest& req = temp.front();
                    if (!req.resolved) {
                        cout << "Request " << req.id << " | Account " << req.accountId << " | Type: " << req.type
                             << " | " << req.description << endl;
                        pending = true;
                    }
                    temp.pop();
                }
                if (!pending) cout << "No pending service requests." << endl;
            } else if (choice == 7) {
                if (serviceRequests.empty()) {
                    cout << "No pending requests." << endl;
                } else {
                    ServiceRequest request = serviceRequests.front();
                    serviceRequests.pop();
                    request.resolved = true;
                    cout << "Resolved request " << request.id << " for account " << request.accountId << endl;
                }
            } else if (choice == 8) {
                int totalAccounts = 0;
                double totalBalance = 0.0;
                for (const auto& acc : accounts) {
                    totalAccounts++;
                    totalBalance += acc->balance;
                }
                cout << "Total accounts: " << totalAccounts << endl;
                cout << "Total bank balance: $" << fixed << setprecision(2) << totalBalance << endl;
            } else if (choice == 9) {
                int accountId;
                cout << "Enter account ID: ";
                cin >> accountId;
                Account* account = findAccount(accountId);
                if (account) {
                    account->active = !account->active;
                    cout << "Account status changed to " << (account->active ? "Active" : "Inactive") << endl;
                } else {
                    cout << "Account not found." << endl;
                }
            } else if (choice == 10) {
                break;
            } else {
                cout << "Invalid option." << endl;
            }
        }
    }
};

int main() {
    BankSystem bank;
    bank.run();
    return 0;
}
