#ifndef SENTINELVAULT_H
#define SENTINELVAULT_H

#include <vector>
#include <string>
#include "LogEntry.h"
#include "ThreatEngine.h"
#include "Sorting.h"
#include "VaultHeader.h" 

class SentinelVault {
private:
    vector<LogEntry> logs;
    node* blacklist;

public:
    SentinelVault();
    void addToBlacklist(string ip);
    void loadLogsFromFile(string filename);
    void performSort();
    void runSecurityScan();

    void saveToBinaryVault(string filename);
    void loadFromBinaryVault(string filename);
};

#endif