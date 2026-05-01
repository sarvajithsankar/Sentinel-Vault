#include "../include/SentinelVault.h"
#include <iostream>
#include <string>

using namespace std;

void displayMenu() {
    cout << "\n========================================" << endl;
    cout << "       SENTINEL-VAULT TERMINAL          " << endl;
    cout << "========================================" << endl;
    cout << "1. Ingest Raw Logs (text)" << endl;
    cout << "2. Manage Blacklist (AVL Tree)" << endl;
    cout << "3. Sort Timeline (Merge Sort)" << endl;
    cout << "4. Secure Vault (Save Binary + RAID 1)" << endl;
    cout << "5. Restore Vault (Load Binary/RAID)" << endl;
    cout << "6. Run Security Scan" << endl;
    cout << "0. Exit System" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Enter command: ";
}

int main() {
    SentinelVault myVault;
    int choice;
    string ip;

    while (true) {
        displayMenu();
        cin >> choice;

        if (choice == 0) {
            cout << "[!] System shutting down. Stay secure." << endl;
            break;
        }

        switch (choice) {
            case 1:
                myVault.loadLogsFromFile("data/auth.log");
                break;
            case 2:
                cout << "Enter IP to blacklist: ";
                cin >> ip;
                myVault.addToBlacklist(ip);
                break;
            case 3:
                myVault.performSort();
                break;
            case 4:
                myVault.saveToBinaryVault("data/secure.vault");
                break;
            case 5:
                myVault.loadFromBinaryVault("data/secure.vault");
                break;
            case 6:
                myVault.runSecurityScan();
                break;
            default:
                cout << "[?] Invalid command. Access Denied." << endl;
        }
    }

    return 0;
}