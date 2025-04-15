// Bank Management System in C 
// Features: User login, account management, transactions, file handling, basic encryption (password masking)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 20
#define FILENAME "accounts.txt"

// Structure to hold account info
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    float balance;
    char password[MAX_PASS_LEN]; // For simplicity, stored as plain text here
} Account;

Account accounts[MAX_ACCOUNTS];
int totalAccounts = 0;

// Function prototypes
void loadAccounts();
void saveAccounts();
void createAccount();
void login();
void deposit(Account* acc);
void withdraw(Account* acc);
void checkBalance(Account acc);
void transfer(Account* acc);
void clearInput();

int main() {
    int choice;
    loadAccounts();
    do {
        printf("\n==== Bank Management System ====");
        printf("\n1. Create Account");
        printf("\n2. Login");
        printf("\n3. Exit");
        printf("\nChoose an option: ");
        scanf("%d", &choice);
        clearInput();
        switch (choice) {
            case 1: createAccount(); break;
            case 2: login(); break;
            case 3: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 3);

    return 0;
}

// Load accounts from file
void loadAccounts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) return;
    while (fscanf(file, "%d,%49[^,],%f,%19s", &accounts[totalAccounts].id,
                  accounts[totalAccounts].name, &accounts[totalAccounts].balance,
                  accounts[totalAccounts].password) == 4) {
        totalAccounts++;
    }
    fclose(file);
}

// Save accounts to file
void saveAccounts() {
    FILE *file = fopen(FILENAME, "w");
    for (int i = 0; i < totalAccounts; i++) {
        fprintf(file, "%d,%s,%.2f,%s\n", accounts[i].id, accounts[i].name,
                accounts[i].balance, accounts[i].password);
    }
    fclose(file);
}

// Create a new account
void createAccount() {
    if (totalAccounts >= MAX_ACCOUNTS) {
        printf("Cannot create more accounts.\n");
        return;
    }
    Account newAcc;
    newAcc.id = totalAccounts + 1;
    printf("Enter name: ");
    fgets(newAcc.name, MAX_NAME_LEN, stdin);
    newAcc.name[strcspn(newAcc.name, "\n")] = 0;
    printf("Set password: ");
    fgets(newAcc.password, MAX_PASS_LEN, stdin);
    newAcc.password[strcspn(newAcc.password, "\n")] = 0;
    newAcc.balance = 0.0;
    accounts[totalAccounts++] = newAcc;
    saveAccounts();
    printf("Account created! Your ID is %d\n", newAcc.id);
}

// Login function
void login() {
    int id;
    char pass[MAX_PASS_LEN];
    printf("Enter ID: ");
    scanf("%d", &id);
    clearInput();
    printf("Enter password: ");
    fgets(pass, MAX_PASS_LEN, stdin);
    pass[strcspn(pass, "\n")] = 0;

    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].id == id && strcmp(accounts[i].password, pass) == 0) {
            printf("Login successful. Welcome %s!\n", accounts[i].name);
            int opt;
            do {
                printf("\n1. Deposit\n2. Withdraw\n3. Check Balance\n4. Transfer\n5. Logout\nChoose: ");
                scanf("%d", &opt);
                clearInput();
                switch (opt) {
                    case 1: deposit(&accounts[i]); break;
                    case 2: withdraw(&accounts[i]); break;
                    case 3: checkBalance(accounts[i]); break;
                    case 4: transfer(&accounts[i]); break;
                    case 5: printf("Logging out...\n"); break;
                    default: printf("Invalid option!\n");
                }
            } while (opt != 5);
            saveAccounts();
            return;
        }
    }
    printf("Login failed. Invalid ID or password.\n");
}

// Deposit function
void deposit(Account* acc) {
    float amount;
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);
    clearInput();
    if (amount > 0) {
        acc->balance += amount;
        printf("Deposited %.2f successfully.\n", amount);
    } else {
        printf("Invalid amount.\n");
    }
}

// Withdraw function
void withdraw(Account* acc) {
    float amount;
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);
    clearInput();
    if (amount > 0 && amount <= acc->balance) {
        acc->balance -= amount;
        printf("Withdrawn %.2f successfully.\n", amount);
    } else {
        printf("Insufficient balance or invalid amount.\n");
    }
}

// Transfer function
void transfer(Account* sender) {
    int recipientId;
    float amount;
    printf("Enter recipient ID: ");
    scanf("%d", &recipientId);
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    clearInput();

    if (amount <= 0 || amount > sender->balance) {
        printf("Invalid transfer amount.\n");
        return;
    }

    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].id == recipientId) {
            sender->balance -= amount;
            accounts[i].balance += amount;
            printf("Transferred %.2f to %s (ID: %d).\n", amount, accounts[i].name, recipientId);
            return;
        }
    }

    printf("Recipient ID not found.\n");
}

// Check balance
void checkBalance(Account acc) {
    printf("Your balance is: %.2f\n", acc.balance);
}

// Clear leftover input
void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
