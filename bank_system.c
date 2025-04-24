#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define MAX_ACCOUNTS 100
#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 5  // 4 digits + null terminator
#define FILENAME "accounts.txt"
#define HISTORY_FILENAME "history_%08d.txt"

typedef struct {
    int accountNumber;
    char name[MAX_NAME_LEN];
    float balance;
    char password[MAX_PASS_LEN];
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
int generateUniqueAccountNumber();
int isValidPassword(const char* pass);
void getPassword(char* password);
void logTransaction(int accNum, const char* action, float amount);

int main() {
    int choice;
    srand(time(NULL)); // Seed for random account number
    loadAccounts();

    printf("====================================\n");
    printf("      Welcome to Jamii Bank!\n");
    printf("====================================\n");

    do {
        printf("\n==== Main Menu ====\n");
        printf("1. Create Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
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

void getPassword(char* password) {
    int index = 0;
    char ch;
#ifdef _WIN32
    while ((ch = _getch()) != '\r') {
        if (ch >= '0' && ch <= '9' && index < MAX_PASS_LEN - 1) {
            password[index++] = ch;
            printf("*");
        } else if (ch == '\b' && index > 0) {
            index--;
            printf("\b \b");
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while ((ch = getchar()) != '\n') {
        if (ch >= '0' && ch <= '9' && index < MAX_PASS_LEN - 1) {
            password[index++] = ch;
            printf("*");
        } else if (ch == 127 && index > 0) {
            index--;
            printf("\b \b");
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    password[index] = '\0';
    printf("\n");
}

// Load accounts from file
void loadAccounts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) return;
    while (fscanf(file, "%d,%49[^,],%f,%4s", &accounts[totalAccounts].accountNumber,
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
        fprintf(file, "%08d,%s,%.2f,%s\n", accounts[i].accountNumber, accounts[i].name,
                accounts[i].balance, accounts[i].password);
    }
    fclose(file);
}

// Generate unique 8-digit account number
int generateUniqueAccountNumber() {
    int accNum;
    int exists;
    do {
        accNum = 10000000 + rand() % 90000000;
        exists = 0;
        for (int i = 0; i < totalAccounts; i++) {
            if (accounts[i].accountNumber == accNum) {
                exists = 1;
                break;
            }
        }
    } while (exists);
    return accNum;
}

// Validate password (must be exactly 4 digits)
int isValidPassword(const char* pass) {
    if (strlen(pass) != 4) {
        printf("Please enter a four digit pin.\n");
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (pass[i] < '0' || pass[i] > '9') {
            printf("Please enter a four digit pin.\n");
            return 0;
        }
    }
    return 1;
}

void logTransaction(int accNum, const char* action, float amount) {
    char filename[30];
    sprintf(filename, HISTORY_FILENAME, accNum);
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        time_t now = time(NULL);
        fprintf(file, "%s - %s: %.2f\n", ctime(&now), action, amount);
        fclose(file);
    }
}

// Create a new account
void createAccount() {
    if (totalAccounts >= MAX_ACCOUNTS) {
        printf("Cannot create more accounts.\n");
        return;
    }

    Account newAcc;
    newAcc.accountNumber = generateUniqueAccountNumber();
    printf("Enter name: ");
    fgets(newAcc.name, MAX_NAME_LEN, stdin);
    newAcc.name[strcspn(newAcc.name, "\n")] = 0;

    do {
        printf("Set 4-digit numeric password: ");
        getPassword(newAcc.password);
    } while (!isValidPassword(newAcc.password));

    newAcc.balance = 0.0;
    accounts[totalAccounts++] = newAcc;
    saveAccounts();

    printf("Account created successfully!\n");
    printf("Your 8-digit Account Number is: %08d\n", newAcc.accountNumber);
}

// Login function
void login() {
    int accNum;
    char pass[MAX_PASS_LEN];
    char accInput[20];
    printf("Enter 8-digit Account Number: ");
    fgets(accInput, sizeof(accInput), stdin);
    accNum = atoi(accInput);

    do {
        printf("Enter 4-digit password: ");
        getPassword(pass);
    } while (!isValidPassword(pass));

    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].accountNumber == accNum && strcmp(accounts[i].password, pass) == 0) {
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
                    case 5: 
                        printf("Logging out...\n");
                        printf("Thank you for Banking with us.\n");
                        break;
                    default: printf("Invalid option!\n");
                }
            } while (opt != 5);
            saveAccounts();
            return;
        }
    }
    printf("Login failed. Invalid account number or password.\n");
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
        printf("New balance: %.2f\n", acc->balance);
        logTransaction(acc->accountNumber, "Deposit", amount);
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
        printf("New balance: %.2f\n", acc->balance);
        logTransaction(acc->accountNumber, "Withdraw", amount);
    } else {
        printf("Insufficient balance or invalid amount.\n");
    }
}

// Transfer function
void transfer(Account* sender) {
    int recipientId;
    float amount;
    printf("Enter recipient's 8-digit Account Number: ");
    scanf("%d", &recipientId);
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    clearInput();

    if (amount <= 0 || amount > sender->balance) {
        printf("Invalid transfer amount.\n");
        return;
    }

    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].accountNumber == recipientId) {
            sender->balance -= amount;
            accounts[i].balance += amount;
            printf("Transferred %.2f to %s (Account: %08d).\n", amount, accounts[i].name, recipientId);
            printf("New balance: %.2f\n", sender->balance);
            logTransaction(sender->accountNumber, "Transfer Sent", amount);
            logTransaction(accounts[i].accountNumber, "Transfer Received", amount);
            return;
        }
    }

    printf("Recipient account not found.\n");
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