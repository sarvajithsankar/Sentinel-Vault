#include "../include/SentinelVault.h"
#include "../include/VaultHeader.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

SentinelVault::SentinelVault() {
    blacklist = nullptr; 
    cout << "[SYSTEM] SentinelVault initialized and secured." << endl;
}

void SentinelVault::addToBlacklist(string ip) {
    blacklist = insert(blacklist, ip);
    cout << "[+] Surveillance active for IP: " << ip << endl;
}

void SentinelVault::loadLogsFromFile(string filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string temp;
        LogEntry entry;

        getline(ss, temp, ',');
        entry.timestamp = stoll(temp);
        getline(ss, entry.ipAddress, ',');
        getline(ss, entry.action, ',');
        getline(ss, temp, ',');
        entry.severity = stoi(temp);

        logs.push_back(entry);
    }
    file.close();
    cout << logs.size() << " records are ingested into the secure vault." << endl;
}

void SentinelVault::performSort()
{
    if (logs.empty())
    {
        cerr << "No logs to sort." << endl;
        return;
    }
    mergeSort(logs, 0, logs.size() - 1);
    cout << "Logs have been sorted by timestamp." << endl;
}
void SentinelVault::runSecurityScan()
{
    cout << "Running security scan on the logs..." << endl;
    for (const auto &log : logs)
    {
        if (isblacklisted(blacklist, log.ipAddress))
        {
            cout << "ALERT: Suspicious activity detected from IP: " << log.ipAddress << endl;
        }
        log.printLog();
    }
    cout << "---------------------------------------------------------------------------" << endl;
}
void SentinelVault::saveToBinaryVault(string filename)
{
    FILE *primary = fopen(filename.c_str(), "wb");
    FILE *mirror = fopen((filename + ".bak").c_str(), "wb");

    if (!primary || !mirror)
    {
        throw runtime_error("RAID Failure: Could not access storage for mirroring.");
    }

    VaultHeader header = {1, 0, 1, 1}; // Encrypted, Not compressed, Integrity OK, v1
    fwrite(&header, sizeof(VaultHeader), 1, primary);
    fwrite(&header, sizeof(VaultHeader), 1, mirror);

    for (const auto &log : logs)
    {
        fwrite(&log, sizeof(LogEntry), 1, primary);
        fwrite(&log, sizeof(LogEntry), 1, mirror);
    }

    fclose(primary);
    fclose(mirror);
    cout << "[RAID 1] Vault locked and mirrored to " << filename << ".bak" << endl;
}

void SentinelVault::loadFromBinaryVault(string filename)
{
    FILE *file = fopen(filename.c_str(), "rb");

    if (!file)
    {
        cout << "[!] PRIMARY VAULT CORRUPTED. Attempting RAID Recovery..." << endl;
        file = fopen((filename + ".bak").c_str(), "rb");

        if (!file)
        {
            throw runtime_error("CRITICAL SYSTEM FAILURE: All vaults lost.");
        }
        cout << "[+] RAID Recovery Successful. Mirror restored." << endl;
    }

    VaultHeader header;
    fread(&header, sizeof(VaultHeader), 1, file);

    LogEntry temp;
    while (fread(&temp, sizeof(LogEntry), 1, file))
    {
        logs.push_back(temp);
    }

    fclose(file);
}