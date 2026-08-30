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
    int calculateThreatScore(const LogEntry& entry) const;
    bool ingestLogLine(const string& line);

public:
    SentinelVault();
    void addToBlacklist(string ip);
    void loadLogsFromFile(string filename);
    void watchLogs(const string& filename, const string& vaultFilename);
    void performSort();
    void runSecurityScan();

    void saveToBinaryVault(string filename);
    void loadFromBinaryVault(string filename);
};

#endif
